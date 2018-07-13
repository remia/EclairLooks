#include "waveformwidget.h"
#include "../image.h"
#include "../utils/gl.h"

#include <cassert>
#include <cmath>
#include <boost/algorithm/string/replace.hpp>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>


static std::string vertexShaderSource = R"(
    #version 410 core
    in vec2 posAttr;

    uniform mat4 matrix;

    void main() {
       gl_Position = matrix * vec4(posAttr, 0.0f, 1.0f);
    }
)";

static std::string geometryShaderSource = R"(
    #version 410 core
    layout (points) in;
    layout (points, max_vertices = <p_max_vertices>) out;
    layout (invocations = <p_invocations>) in;

    out vec4 gColor;
    uniform mat4 gMatrix;
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
            gl_Position = gMatrix * gl_Position;
            gColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            EmitVertex();
            EndPrimitive();

            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = (colour.g * 2.0f) - 1.0f;
            gl_Position = gMatrix * gl_Position;
            gColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
            EmitVertex();
            EndPrimitive();

            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = (colour.b * 2.0f) - 1.0f;
            gl_Position = gMatrix * gl_Position;
            gColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
            EmitVertex();
            EndPrimitive();
        }
    }
)";

static std::string fragmentShaderSource = R"(
    #version 410 core
    in vec4 gColor;
    out vec4 fragColor;

    uniform float alpha;

    void main() {
       fragColor = vec4(gColor.rgb, alpha);
       fragColor.rgb = pow(fragColor.rgb, vec3(1./2.4));
    }
)";

static std::string fragmentShaderSolidSource = R"(
    #version 410 core
    out vec4 fragColor;

    uniform vec4 color;

    void main() {
       fragColor = color;
       fragColor.rgb = pow(color.rgb, vec3(1./2.4));
    }
)";

WaveformWidget::WaveformWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_alpha(0.1f), m_textureSrc(nullptr),
      m_imagePosition(0.f, 0.f), m_imageScale(1.f), m_clickPosition(0.f, 0.f),
      m_moveDelta(0.f, 0.f)
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

void WaveformWidget::mousePressEvent(QMouseEvent *event)
{
    setMouseTracking(true);
    m_clickPosition = widgetToNorm(event->localPos());

    update();
}

void WaveformWidget::mouseMoveEvent(QMouseEvent *event)
{

    m_moveDelta = widgetToNorm(event->localPos()) - m_clickPosition;

    update();
}

void WaveformWidget::mouseReleaseEvent(QMouseEvent *event)
{
    setMouseTracking(false);
    m_imagePosition += m_moveDelta;
    m_moveDelta = QPointF(0.f, 0.f);

    update();
}

void WaveformWidget::wheelEvent(QWheelEvent *event)
{
    m_imageScale += event->delta() / 120.0f;
    m_imageScale = std::clamp(m_imageScale, 0.5f, 25.f);

    update();
}

void WaveformWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_programLegend = new QOpenGLShaderProgram(this);
    m_programLegend->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
    m_programLegend->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSolidSource.c_str());
    m_programLegend->link();

    m_posAttr = m_programLegend->attributeLocation("posAttr");
    m_legendMatrixUniform = m_programLegend->uniformLocation("matrix");
    m_legendColorUniform = m_programLegend->uniformLocation("color");
    m_legendAlphaUniform = m_programLegend->uniformLocation("alpha");

    m_vaoLegend = new QOpenGLVertexArrayObject(this);
    GL_CHECK(m_vaoLegend->create());
    GL_CHECK(m_vaoLegend->bind());

    // 10 lines with 5 steps each
    std::vector<GLfloat> vertices;

    uint16_t line_count = 10;
    uint16_t line_step = 4;
    for (int i = 0; i < line_count; ++i) {
        float y = (i / (line_count - 1.f)) * 2.f - 1.f;
        vertices.push_back(-1.0f); // A.x
        vertices.push_back(y);     // A.y
        vertices.push_back(1.0f);  // B.x
        vertices.push_back(y);     // B.y

        if (i == line_count - 1)
            break;

        for (int j = 0; j < line_step; ++j) {
            float y_offset = j / (line_step - 1.f) * (2.f / line_count);
            vertices.push_back(-1.0f);                // A.x
            vertices.push_back(y + y_offset);         // A.y
            vertices.push_back(-1.0f + (1.f / 25.f)); // B.x
            vertices.push_back(y + y_offset);         // B.y
        }
    };

    GL_CHECK(m_verticesLegend.create());
    GL_CHECK(m_verticesLegend.bind());
    GL_CHECK(m_verticesLegend.allocate(vertices.data(), vertices.size() * sizeof(GLfloat)));
    GL_CHECK(glEnableVertexAttribArray(m_posAttr));
    GL_CHECK(glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vaoLegend->release());
}

void WaveformWidget::resizeGL(int w, int h)
{
    const qreal retinaScale = devicePixelRatio();
    GL_CHECK(glViewport(0, 0, w * retinaScale, h * retinaScale));
}

void WaveformWidget::paintGL()
{
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Zoom / Pan
    QMatrix4x4 model;
    QMatrix4x4 view;
    view.scale(m_imageScale, m_imageScale);
    view.translate(
        m_imagePosition.x() + m_moveDelta.x(),
        m_imagePosition.y() + m_moveDelta.y());
    QMatrix4x4 projection;
    // projection.ortho(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f);

    QMatrix4x4 mvp = projection * view * model;

    // Draw legend
    m_vaoLegend->bind();
    m_programLegend->bind();

    m_programLegend->setUniformValue(m_legendColorUniform, 1.f, 1.f, 0.6f, 1.f);
    m_programLegend->setUniformValue(m_legendMatrixUniform, mvp);
    glDrawArrays(GL_LINES, 0, m_verticesLegend.size() / sizeof(GLfloat));

    m_programLegend->release();
    m_vaoLegend->release();

    // Fill in waveform
    if (!m_textureSrc)
        return;

    bool texComplete = m_textureSrc->isStorageAllocated();

    m_vao->bind();
    m_program->bind();
    if (texComplete)
        m_textureSrc->bind();

    m_program->setUniformValue(m_alphaUniform, m_alpha);
    m_program->setUniformValue(m_legendMatrixUniform, QMatrix4x4());
    m_program->setUniformValue(m_matrixUniform, mvp);

    glDrawArrays(GL_POINTS, 0, m_vertices.size() / sizeof(GLfloat));

    if (texComplete && m_textureSrc->isBound())
        m_textureSrc->release();
    m_program->release();
    m_vao->release();
}

void WaveformWidget::resetTexture(const Image & img)
{
    makeCurrent();

    GLint gl_max_vertices, gl_max_invocations;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &gl_max_vertices);
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &gl_max_invocations);

    int max_vertices = 240;
    assert(max_vertices % 3 == 0);
    assert(max_vertices < gl_max_vertices);
    int invocations = std::ceil(img.height() / (max_vertices / 3.f));
    assert(invocations < gl_max_invocations);
    qInfo() << "Waveform using" << invocations << " invocations with" << max_vertices << "vertices each !\n";

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
    m_matrixUniform = m_program->uniformLocation("gMatrix");

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
    m_textureSrc = &tex;
    update();
}

QPointF WaveformWidget::widgetToNorm(const QPointF & pos) const
{
    return QPointF(1.f * pos.x() / width(), 1.f * pos.y() / height());
}

QPointF WaveformWidget::widgetToWorld(const QPointF & pos) const
{
    return widgetToNorm(pos) * 2.f - QPointF(1.f, 1.f);
}