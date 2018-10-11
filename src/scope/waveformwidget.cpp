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
    out vec3 color;

    uniform mat4 matrix;
    uniform sampler2D v_tex;
    uniform int width;
    uniform int height;
    uniform int channel;

    void main() {
        int x = gl_VertexID % width;
        int y = gl_VertexID / width;
        vec2 pix = vec2(x, y) / vec2(width, height);

        float col = texture(v_tex, vec2(pix.x, 1.0f - pix.y))[channel];
        gl_Position = vec4(pix.x, col, 0.0, 1.0);
        gl_Position.xyz = (gl_Position.xyz * 2.0) - 1.;
        gl_Position.y *= -1.;
        gl_Position = matrix * gl_Position;

        color = vec3(0.0, 0.0, 0.0);
        color[channel] = 1.0;
    }
)";

static std::string fragmentShaderSource = R"(
    #version 410 core
    in vec3 color;
    layout(location = 0) out vec4 fragColor;

    uniform float alpha;

    void main() {
       fragColor = vec4(color.rgb, alpha);
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
    : TextureView(parent), m_alpha(0.1f), m_scopeType("Waveform"), m_textureSrc(nullptr)
{
    setDefaultScale(1.0f);
}

void WaveformWidget::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Plus:
        m_alpha *= 1.2f;
        m_alpha = std::clamp(m_alpha, 0.001f, 1.0f);
        update();
        break;
      case Qt::Key_Minus:
        m_alpha *= 0.8f;
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
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


    if (m_scopeType == "Waveform") {
        drawGraph(viewMatrix(), 0);
        drawGraph(viewMatrix(), 1);
        drawGraph(viewMatrix(), 2);
    }
    else if (m_scopeType == "Parade") {
        QMatrix4x4 m = viewMatrix();
        for (uint8_t i = 0; i < 3; ++i) {
            QMatrix4x4 subm = m;
            subm.translate(-1. + i * 2./3., 0.0);
            subm.scale(1./3., 1.f);
            subm.translate(1., 0.);
            drawGraph(subm, i);
        }
    }
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

void WaveformWidget::setScopeType(const std::string &type)
{
    m_scopeType = type;
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

    m_programScope.removeAllShaders();
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programScope.link();
    if (!m_programScope.isLinked())
        qWarning() << m_programScope.log() << "\n";

    GL_CHECK(m_scopeTextureUniform = m_programScope.uniformLocation("v_tex"));
    GL_CHECK(m_scopeAlphaUniform = m_programScope.uniformLocation("alpha"));
    GL_CHECK(m_scopeMatrixUniform = m_programScope.uniformLocation("matrix"));
    GL_CHECK(m_scopeResolutionWUniform = m_programScope.uniformLocation("width"));
    GL_CHECK(m_scopeResolutionHUniform = m_programScope.uniformLocation("height"));
    GL_CHECK(m_scopeChannelUniform = m_programScope.uniformLocation("channel"));

    GL_CHECK(m_vaoScope.destroy());
    GL_CHECK(m_vaoScope.create());
    GL_CHECK(m_vaoScope.bind());

    GL_CHECK(m_vaoScope.release());
}

void WaveformWidget::drawGraph(const QMatrix4x4 &m, uint8_t mode)
{
    // Draw legend
    m_vaoLegend.bind();
    m_programLegend.bind();

        GL_CHECK(m_programLegend.setUniformValue(m_legendColorUniform, 1.f, 1.f, 0.6f, 1.f));
        GL_CHECK(m_programLegend.setUniformValue(m_legendMatrixUniform, m));
        GL_CHECK(glDrawArrays(GL_LINES, 0, 0.5 * m_verticesLegend.size() / sizeof(GLfloat)));

    m_programLegend.release();
    m_vaoLegend.release();

    // Fill in waveform
    bool texComplete = m_textureSrc && m_textureSrc->isStorageAllocated();
    if (!texComplete)
        return;

    float alpha = m_alpha;
    alpha /= 3.f;

    GL_CHECK(m_vaoScope.bind());
    GL_CHECK(m_programScope.bind());
    GL_CHECK(m_textureSrc->bind());

        GL_CHECK(m_programScope.setUniformValue(m_scopeAlphaUniform, alpha));
        GL_CHECK(m_programScope.setUniformValue(m_scopeMatrixUniform, m));
        GL_CHECK(m_programScope.setUniformValue(m_scopeChannelUniform, mode));
        GL_CHECK(m_programScope.setUniformValue(m_scopeResolutionWUniform, m_textureSrc->width()));
        GL_CHECK(m_programScope.setUniformValue(m_scopeResolutionHUniform, m_textureSrc->height()));
        GL_CHECK(glDrawArrays(GL_POINTS, 0, m_textureSrc->width() * m_textureSrc->height()));

    GL_CHECK(m_textureSrc->release());
    GL_CHECK(m_programScope.release());
    GL_CHECK(m_vaoScope.release());
}