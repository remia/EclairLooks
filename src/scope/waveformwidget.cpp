#include "waveformwidget.h"
#include "../image.h"
#include "../utils/gl.h"

#include <cassert>
#include <cmath>
#include <boost/algorithm/string/replace.hpp>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>


std::string vertexShaderSource = R"(
    #version 410 core
    in vec2 posAttr;

    void main() {
       gl_Position = vec4(posAttr, 0.0f, 1.0f);
    }
)";

std::string geometryShaderSource = R"(
    #version 410 core
    layout (points) in;
    layout (points, max_vertices = <p_max_vertices>) out;
    layout (invocations = <p_invocations>) in;

    out vec4 gColor;
    uniform sampler2D imgTex;

    void main() {
        int primitive_count = <p_max_vertices> / 3;
        float start_offset = gl_InvocationID / (1.f * <p_invocations>);

        for (int i = 0; i < primitive_count; ++i) {
            // Sample colour
            float x = (gl_in[0].gl_Position.x + 1.0) / 2.0f;
            float y = start_offset + (i / (primitive_count - 1.f)) * (1.f / <p_invocations>);
            vec4 colour = texture(imgTex, vec2(x, y));

            // Emit points according to colour
            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = (colour.r * 2.0f) - 1.0f;
            gColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            EmitVertex();
            EndPrimitive();

            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = (colour.g * 2.0f) - 1.0f;
            gColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
            EmitVertex();
            EndPrimitive();

            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = (colour.b * 2.0f) - 1.0f;
            gColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
            EmitVertex();
            EndPrimitive();
        }
    }
)";

std::string fragmentShaderSource = R"(
    #version 410 core
    in vec4 gColor;
    out vec4 fragColor;

    uniform float alpha;

    void main() {
       fragColor = vec4(gColor.rgb, alpha);
    }
)";

WaveformWidget::WaveformWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_alpha(0.1f), m_textureSrc(nullptr)
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
}

void WaveformWidget::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Plus:
        m_alpha *= 1.05f;
        m_alpha = std::clamp(m_alpha, 0.001f, 1.0f);
        update();
        break;
      case Qt::Key_Minus:
        m_alpha *= 0.95f;
        m_alpha = std::clamp(m_alpha, 0.001f, 1.0f);
        update();
        break;
      default:
        QWidget::keyPressEvent(event);
  }
}

void WaveformWidget::initializeGL()
{
    initializeOpenGLFunctions();
}

void WaveformWidget::resizeGL(int w, int h)
{
    const qreal retinaScale = devicePixelRatio();
    GL_CHECK(glViewport(0, 0, w * retinaScale, h * retinaScale));
}

void WaveformWidget::paintGL()
{
    if (!m_textureSrc)
        return;

    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    bool texComplete = m_textureSrc->isStorageAllocated();

    m_vao->bind();
    m_program->bind();
    if (texComplete)
        m_textureSrc->bind();

    GL_CHECK(m_program->setUniformValue(m_alphaUniform, m_alpha));

    glDrawArrays(GL_POINTS, 0, m_vertices.size() / sizeof(GLfloat));

    if (texComplete && m_textureSrc->isBound())
        m_textureSrc->release();
    m_program->release();
    m_vao->release();
}

void WaveformWidget::resetTexture(const Image & img)
{
    qInfo() << "Waveform reset signal !";

    makeCurrent();

    GLint gl_max_vertices, gl_max_invocations;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &gl_max_vertices);
    qInfo() << "GL_MAX_GEOMETRY_OUTPUT_VERTICES :" << gl_max_vertices;
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &gl_max_invocations);
    qInfo() << "GL_MAX_GEOMETRY_SHADER_INVOCATIONS :" << gl_max_invocations;

    int max_vertices = 240;
    assert(max_vertices % 3 == 0);
    assert(max_vertices < gl_max_vertices);
    int invocations = std::ceil(img.height() / (max_vertices / 3.f));
    assert(invocations < gl_max_invocations);
    qInfo() << "Using" << invocations << " invocations with" << max_vertices << "vertices each !";

    boost::replace_all(geometryShaderSource, "<p_max_vertices>", std::to_string(max_vertices));
    boost::replace_all(geometryShaderSource, "<p_invocations>", std::to_string(invocations));

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
    m_program->addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderSource.c_str());
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_program->link();

    m_posAttr = m_program->attributeLocation("posAttr");
    m_textureSrcUniform = m_program->uniformLocation("imgTex");
    m_alphaUniform = m_program->uniformLocation("alpha");

    m_vao = new QOpenGLVertexArrayObject(this);
    GL_CHECK(m_vao->create());
    GL_CHECK(m_vao->bind());

    uint32_t w = img.width();
    GLfloat vertices[w * 2];
    for (int i = 0; i < w; ++i) {
        vertices[i*2] = 2.0f * i / (w - 1.0f) - 1.0f;
        vertices[i*2+1] = -1.0f;
    };

    GL_CHECK(m_vertices.create());
    GL_CHECK(m_vertices.bind());
    GL_CHECK(m_vertices.allocate(vertices, sizeof(vertices)));
    GL_CHECK(glEnableVertexAttribArray(m_posAttr));
    GL_CHECK(glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vao->release());
}

void WaveformWidget::updateTexture(QOpenGLTexture & tex)
{
    qInfo() << "Waveform update signal !";

    m_textureSrc = &tex;
    update();
}
