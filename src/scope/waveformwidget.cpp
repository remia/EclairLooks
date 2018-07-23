#include "waveformwidget.h"
#include "../image.h"
#include "../utils/gl.h"

#include <cassert>
#include <cmath>
#include <boost/algorithm/string/replace.hpp>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>


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
            vec4 colour = texture(imgTex, vec2(x, 1.0f - y));

            // Emit points according to colour
            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = -1.f * ((colour.r * 2.0f) - 1.0f);
            gl_Position = gMatrix * gl_Position;
            gColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
            EmitVertex();
            EndPrimitive();

            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = -1.f * ((colour.g * 2.0f) - 1.0f);
            gl_Position = gMatrix * gl_Position;
            gColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
            EmitVertex();
            EndPrimitive();

            gl_Position = gl_in[0].gl_Position;
            gl_Position.y = -1.f * ((colour.b * 2.0f) - 1.0f);
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
    layout(location = 0) out vec4 fragColor;

    uniform float alpha;

    void main() {
       fragColor = vec4(gColor.rgb, alpha);
       fragColor.rgb = pow(fragColor.rgb, vec3(1./2.4));
    }
)";

static std::string fragmentShaderSolidSource = R"(
    #version 410 core
    layout(location = 0) out vec4 fragColor;

    uniform vec4 color;

    void main() {
       fragColor = color;
       fragColor.rgb = pow(color.rgb, vec3(1./2.4));
    }
)";

WaveformWidget::WaveformWidget(QWidget *parent)
    : TextureView(parent), m_alpha(0.1f), m_textureSrc(nullptr)
{

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

  TextureView::keyPressEvent(event);
}

void WaveformWidget::initializeGL()
{
    initializeOpenGLFunctions();

    initLegend();
}

void WaveformWidget::paintGL()
{
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));

    // Zoom / Pan
    QMatrix4x4 model;
    QMatrix4x4 view = viewMatrix();
    QMatrix4x4 projection;
    // projection.ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    QMatrix4x4 mvp = projection * view * model;

    // Draw legend
    m_vaoLegend.bind();
    m_programLegend.bind();

        GL_CHECK(m_programLegend.setUniformValue(m_legendColorUniform, 1.f, 1.f, 0.6f, 1.f));
        GL_CHECK(m_programLegend.setUniformValue(m_legendMatrixUniform, mvp));
        GL_CHECK(glDrawArrays(GL_LINES, 0, m_verticesLegend.size() / sizeof(GLfloat)));

    m_programLegend.release();
    m_vaoLegend.release();

    // Fill in waveform
    bool texComplete = m_textureSrc && m_textureSrc->isStorageAllocated();
    if (!texComplete)
        return;

    GL_CHECK(m_vaoScope.bind());
    GL_CHECK(m_programScope.bind());
    GL_CHECK(m_textureSrc->bind());

        GL_CHECK(m_programScope.setUniformValue(m_scopeMatrixUniform, QMatrix4x4()));
        GL_CHECK(m_programScope.setUniformValue(m_scopeAlphaUniform, m_alpha));
        GL_CHECK(m_programScope.setUniformValue(m_scopegMatrixUniform, mvp));
        GL_CHECK(glDrawArrays(GL_POINTS, 0, m_verticesScope.size() / sizeof(GLfloat)));

    GL_CHECK(m_textureSrc->release());
    GL_CHECK(m_programScope.release());
    GL_CHECK(m_vaoScope.release());
}

void WaveformWidget::resetTexture(const Image & img)
{
    initScope(img.width(), img.height());
}

void WaveformWidget::updateTexture(QOpenGLTexture & tex)
{
    m_textureSrc = &tex;
    update();
}

void WaveformWidget::initLegend()
{
    makeCurrent();

    m_programLegend.removeAllShaders();
    m_programLegend.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader());
    m_programLegend.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSolidSource.c_str());
    m_programLegend.link();
    if (!m_programLegend.isLinked())
        qWarning() << m_programLegend.log() << "\n";

    GL_CHECK(m_legendMatrixUniform = m_programLegend.uniformLocation("matrix"));
    GL_CHECK(m_legendColorUniform = m_programLegend.uniformLocation("color"));
    GL_CHECK(m_legendAlphaUniform = m_programLegend.uniformLocation("alpha"));

    GL_CHECK(m_vaoLegend.destroy());
    GL_CHECK(m_vaoLegend.create());
    GL_CHECK(m_vaoLegend.bind());

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

    GL_CHECK(m_verticesLegend.destroy());
    GL_CHECK(m_verticesLegend.create());
    GL_CHECK(m_verticesLegend.bind());
    GL_CHECK(m_verticesLegend.allocate(vertices.data(), vertices.size() * sizeof(GLfloat)));
    GL_CHECK(glEnableVertexAttribArray(AttributeLocation::Position));
    GL_CHECK(glVertexAttribPointer(AttributeLocation::Position, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vaoLegend.release());
}

void WaveformWidget::initScope(uint16_t w, uint16_t h)
{
    makeCurrent();

    GLint gl_max_vertices, gl_max_invocations;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &gl_max_vertices);
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &gl_max_invocations);

    int max_vertices = 240;
    assert(max_vertices % 3 == 0);
    assert(max_vertices < gl_max_vertices);
    int invocations = std::ceil(h / (max_vertices / 3.f));
    assert(invocations < gl_max_invocations);
    qInfo() << "Waveform using" << invocations << " invocations with" << max_vertices << "vertices each !\n";

    boost::replace_all(geometryShaderSource, "<p_max_vertices>", std::to_string(max_vertices));
    boost::replace_all(geometryShaderSource, "<p_invocations>", std::to_string(invocations));

    m_programScope.removeAllShaders();
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader());
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderSource.c_str());
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programScope.link();
    if (!m_programScope.isLinked())
        qWarning() << m_programScope.log() << "\n";

    GL_CHECK(m_scopeTextureUniform = m_programScope.uniformLocation("imgTex"));
    GL_CHECK(m_scopeAlphaUniform = m_programScope.uniformLocation("alpha"));
    GL_CHECK(m_scopeMatrixUniform = m_programScope.uniformLocation("matrix"));
    GL_CHECK(m_scopegMatrixUniform = m_programScope.uniformLocation("gMatrix"));

    GL_CHECK(m_vaoScope.destroy());
    GL_CHECK(m_vaoScope.create());
    GL_CHECK(m_vaoScope.bind());

    GLfloat vertices[w * 2];
    for (int i = 0; i < w; ++i) {
        vertices[i*2] = 2.0f * i / (w - 1.0f) - 1.0f;
        vertices[i*2+1] = -1.0f;
    };

    GL_CHECK(m_verticesScope.destroy());
    GL_CHECK(m_verticesScope.create());
    GL_CHECK(m_verticesScope.bind());
    GL_CHECK(m_verticesScope.allocate(vertices, sizeof(vertices)));
    GL_CHECK(glEnableVertexAttribArray(AttributeLocation::Position));
    GL_CHECK(glVertexAttribPointer(AttributeLocation::Position, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vaoScope.release());
}
