#include "waveform.h"

#include <cassert>
#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#include <context.h>
#include <core/image.h>
#include <utils/generic.h>
#include <utils/gl.h>


static std::string vertexShaderSource = R"(
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
        gl_Position.xy = (gl_Position.xy * 2.0) - 1.;
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
    }
)";

static std::string fragmentShaderSolidSource = R"(
    #version 410 core
    layout(location = 0) out vec4 fragColor;

    uniform vec4 color;

    void main() {
       fragColor = color;
    }
)";

WaveformWidget::WaveformWidget(QWidget *parent)
    : GLScopeWidget(parent), m_scopeType("Waveform")
{

}

void WaveformWidget::setScopeType(const std::string &type)
{
    m_scopeType = type;
    update();
}

void WaveformWidget::initScopeGL()
{
    initLegend();
    initScope();
}

void WaveformWidget::initLegend()
{
    m_programLegend.removeAllShaders();
    m_programLegend.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader());
    m_programLegend.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSolidSource.c_str());
    m_programLegend.link();
    if (!m_programLegend.isLinked())
        qWarning() << m_programLegend.log() << "\n";

    GL_CHECK(m_legendUniforms["matrix"] = m_programLegend.uniformLocation("matrix"));
    GL_CHECK(m_legendUniforms["color"] = m_programLegend.uniformLocation("color"));
    GL_CHECK(m_legendUniforms["alpha"] = m_programLegend.uniformLocation("alpha"));

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
    GL_CHECK(glEnableVertexAttribArray(UnderlyingT(AttributeLocation::Position)));
    GL_CHECK(glVertexAttribPointer(
        UnderlyingT(AttributeLocation::Position), 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vaoLegend.release());
}

void WaveformWidget::initScope()
{
    m_programScope.removeAllShaders();
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programScope.link();
    if (!m_programScope.isLinked())
        qWarning() << m_programScope.log() << "\n";

    GL_CHECK(m_scopeUniforms["v_tex"] = m_programScope.uniformLocation("v_tex"));
    GL_CHECK(m_scopeUniforms["alpha"] = m_programScope.uniformLocation("alpha"));
    GL_CHECK(m_scopeUniforms["matrix"] = m_programScope.uniformLocation("matrix"));
    GL_CHECK(m_scopeUniforms["width"] = m_programScope.uniformLocation("width"));
    GL_CHECK(m_scopeUniforms["height"] = m_programScope.uniformLocation("height"));
    GL_CHECK(m_scopeUniforms["channel"] = m_programScope.uniformLocation("channel"));

    GL_CHECK(m_vaoScope.destroy());
    GL_CHECK(m_vaoScope.create());
}

void WaveformWidget::paintScopeGL(const QMatrix4x4 &m)
{
    if (m_scopeType == "Waveform") {
        for (uint8_t i = 0; i < 3; ++i)
            paintScope(m, i);
    }
    else if (m_scopeType == "Parade") {
        QMatrix4x4 mat = m;
        for (uint8_t i = 0; i < 3; ++i) {
            QMatrix4x4 subm = mat;
            subm.translate(-1. + i * 2./3., 0.0);
            subm.scale(1./3., 1.f);
            subm.translate(1., 0.);
            paintScope(subm, i);
        }
    }
}

void WaveformWidget::paintScope(const QMatrix4x4 &m, uint8_t mode)
{
    // Draw legend
    GL_CHECK(m_vaoLegend.bind());
    GL_CHECK(m_programLegend.bind());

        GL_CHECK(m_programLegend.setUniformValue(m_legendUniforms["color"], 1.f, 1.f, 0.6f, 1.f));
        GL_CHECK(m_programLegend.setUniformValue(m_legendUniforms["matrix"], m));
        GL_CHECK(glDrawArrays(GL_LINES, 0, 0.5 * m_verticesLegend.size() / sizeof(GLfloat)));

    GL_CHECK(m_programLegend.release());
    GL_CHECK(m_vaoLegend.release());

    // Fill in waveform
    if (m_textureId == -1)
        return;

    GL_CHECK(m_vaoScope.bind());
    GL_CHECK(m_programScope.bind());
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));

    {
        GLAutoFilterMode autoFilter(m_filtering);

        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["alpha"], m_alpha * 5));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["matrix"], m));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["channel"], mode));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["width"], m_textureSize.width()));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["height"], m_textureSize.height()));
        GL_CHECK(glDrawArrays(GL_POINTS, 0, m_textureSize.width() * m_textureSize.height()));
    }

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(m_programScope.release());
    GL_CHECK(m_vaoScope.release());
}