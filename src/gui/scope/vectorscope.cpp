#include "vectorscope.h"

#include <cassert>
#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#include <context.h>
#include <core/image.h>
#include <utils/generic.h>
#include <utils/gl.h>

static std::string commonFunctions = R"(
    // RGB to Y'PbPr (BT.601)
    // See Digital Video and HD 2nd, Poynton, Eq 29.2
    vec3 RGBtoYPbPr_601(vec3 rgb)
    {
        vec3 yuv;
        yuv.x = (rgb.x * 0.299) + (rgb.y * 0.587) + (rgb.z * 0.114);
        yuv.y = ((rgb.z - yuv.x) * 0.564) + 0.5;
        yuv.z = ((rgb.x - yuv.x) * 0.713) + 0.5;
        return yuv;
    }

    // RGB to Y'PbPr (BT.709)
    // See Digital Video and HD 2nd, Poynton, Eq 30.2
    vec3 RGBtoYPbPr_709(vec3 rgb)
    {
        vec3 yuv;
        yuv.x = (rgb.x * 0.2126) + (rgb.y * 0.7152) + (rgb.z * 0.0722);
        yuv.y = ((rgb.z - yuv.x) * 0.5389) + 0.5;
        yuv.z = ((rgb.x - yuv.x) * 0.6350) + 0.5;
        return yuv;
    }

    // RGB to Y'(B-Y)'(R-Y)' (BT.709)
    // See Digital Video and HD 2nd, Poynton, Eq 30.1
    vec3 RGBtoYUV_709(vec3 rgb)
    {
        mat3 m = mat3(
            0.2126,  0.7152,  0.0722,
           -0.2126, -0.7152,  0.9278,
            0.7874, -0.7152, -0.0722);
        return rgb * m;
    }
)";

static std::string vertexShaderSource = R"(
    #version 410 core
    out vec4 color;

    uniform mat4 matrix;
    uniform sampler2D v_tex;
    uniform int width;
    uniform int height;

    {{functions}}

    void main() {
        int x = gl_VertexID % width;
        int y = gl_VertexID / width;
        vec2 pix = vec2(x, y) / vec2(width, height);

        vec4 rgb = texture(v_tex, vec2(pix.x, 1.0f - pix.y));
        vec3 yuv = RGBtoYUV_709(rgb.xyz);

        gl_Position = vec4(yuv.yz, 0.0, 1.0);
        gl_Position.y *= -1.;
        gl_Position = matrix * gl_Position;

        color = rgb;
    }
)";

static std::string circleVertexShaderSource = R"(
    #version 410 core
    out vec4 color;

    uniform mat4 matrix;
    uniform int length;

    #define M_PI 3.1415926535897932384626433832795

    void main() {
        float angle = 2.0 * M_PI * gl_VertexID / length;

        gl_Position = vec4(cos(angle), sin(angle), 0.0, 1.0);
        gl_Position = matrix * gl_Position;

        color = vec4(0.3, 0.2, 0.05, 1.0);
    }
)";

static std::string tickVertexShaderSource = R"(
    #version 410 core
    out vec4 color;

    uniform mat4 matrix;
    uniform int length;

    #define M_PI 3.1415926535897932384626433832795

    void main() {
        int id = gl_VertexID / 2;
        float angle = 2.0 * M_PI * id / length;

        gl_Position = vec4(cos(angle), sin(angle), 0.0, 1.0);

        if (gl_VertexID % (length * 2 / 4) == 0)
            gl_Position.xy = vec2(0.0, 0.0);
        else if (gl_VertexID % 4 == 0)
            gl_Position.xy *= 0.9;
        else if (gl_VertexID % 2 == 0)
            gl_Position.xy *= 0.95;

        gl_Position = matrix * gl_Position;

        color = vec4(0.3, 0.2, 0.05, 1.0);
    }
)";

static std::string targetVertexShaderSource = R"(
    #version 410 core
    out vec4 color;

    uniform mat4 matrix;
    uniform vec3 tColor;
    uniform float tScale;

    {{functions}}

    vec2 points[4] = vec2[4](
        vec2(-1.0, -1.0),
        vec2(-1.0,  1.0),
        vec2( 1.0,  1.0),
        vec2( 1.0, -1.0)
    );

    void main() {
        vec3 yuv = RGBtoYUV_709(tColor);

        gl_Position = vec4(points[gl_VertexID] * tScale, 0.0, 1.0);
        gl_Position.xy += yuv.yz;
        gl_Position.y *= -1.;
        gl_Position = matrix * gl_Position;

        color = vec4(tColor, 0.45);
        color.rgb += vec3(0.3, 0.2, 0.05);
    }
)";

static std::string fragmentShaderSource = R"(
    #version 410 core

    in vec4 color;
    layout(location = 0) out vec4 fragColor;

    uniform float alpha;

    void main() {
       fragColor = color;
       fragColor.a *= alpha;
    }
)";

VectorScopeWidget::VectorScopeWidget(QWidget *parent)
    : GLScopeWidget(parent), m_rasterScan(false)
{

}

void VectorScopeWidget::keyPressEvent(QKeyEvent *event)
{
    auto& ctx = Context::getInstance();
    auto keySeq = QKeySequence(event->key() | event->modifiers());

    if (keySeq == ctx.shortcut("VectorScope_TraceToggle")) {
        m_rasterScan = not m_rasterScan;
        update();
    }
    else {
        QWidget::keyPressEvent(event);
    }

    GLScopeWidget::keyPressEvent(event);
}

void VectorScopeWidget::initScopeGL()
{
    initLegend();
    initScope();
}

void VectorScopeWidget::resizeScopeGL(int w, int h)
{
    setAspectRatio(1.0f);
}

void VectorScopeWidget::initLegend()
{
    // Circle
    m_programCircle.removeAllShaders();
    m_programCircle.addShaderFromSourceCode(QOpenGLShader::Vertex, circleVertexShaderSource.c_str());
    m_programCircle.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programCircle.link();
    if (!m_programCircle.isLinked())
        qWarning() << m_programCircle.log() << "\n";

    GL_CHECK(m_circleUniforms["length"] = m_programCircle.uniformLocation("length"));
    GL_CHECK(m_circleUniforms["alpha"] = m_programCircle.uniformLocation("alpha"));
    GL_CHECK(m_circleUniforms["matrix"] = m_programCircle.uniformLocation("matrix"));

    // Ticks
    m_programTick.removeAllShaders();
    m_programTick.addShaderFromSourceCode(QOpenGLShader::Vertex, tickVertexShaderSource.c_str());
    m_programTick.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programTick.link();
    if (!m_programTick.isLinked())
        qWarning() << m_programTick.log() << "\n";

    GL_CHECK(m_tickUniforms["length"] = m_programTick.uniformLocation("length"));
    GL_CHECK(m_tickUniforms["alpha"] = m_programTick.uniformLocation("alpha"));
    GL_CHECK(m_tickUniforms["matrix"] = m_programTick.uniformLocation("matrix"));

    // Target
    m_programTarget.removeAllShaders();
    QString vertexSource = QString::fromStdString(targetVertexShaderSource)
        .replace(QRegExp("\\{\\{functions\\}\\}"), QString::fromStdString(commonFunctions));
    m_programTarget.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    m_programTarget.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programTarget.link();
    if (!m_programTarget.isLinked())
        qWarning() << m_programTarget.log() << "\n";

    GL_CHECK(m_targetUniforms["tColor"] = m_programTarget.uniformLocation("tColor"));
    GL_CHECK(m_targetUniforms["tScale"] = m_programTarget.uniformLocation("tScale"));
    GL_CHECK(m_targetUniforms["alpha"] = m_programTarget.uniformLocation("alpha"));
    GL_CHECK(m_targetUniforms["matrix"] = m_programTarget.uniformLocation("matrix"));
}

void VectorScopeWidget::initScope()
{
    m_programScope.removeAllShaders();
    QString vertexSource = QString::fromStdString(vertexShaderSource)
        .replace(QRegExp("\\{\\{functions\\}\\}"), QString::fromStdString(commonFunctions));
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    m_programScope.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programScope.link();
    if (!m_programScope.isLinked())
        qWarning() << m_programScope.log() << "\n";

    GL_CHECK(m_scopeUniforms["v_tex"] = m_programScope.uniformLocation("v_tex"));
    GL_CHECK(m_scopeUniforms["alpha"] = m_programScope.uniformLocation("alpha"));
    GL_CHECK(m_scopeUniforms["matrix"] = m_programScope.uniformLocation("matrix"));
    GL_CHECK(m_scopeUniforms["width"] = m_programScope.uniformLocation("width"));
    GL_CHECK(m_scopeUniforms["height"] = m_programScope.uniformLocation("height"));

    GL_CHECK(m_emptyVao.destroy());
    GL_CHECK(m_emptyVao.create());
}

void VectorScopeWidget::paintScopeGL(const QMatrix4x4 &m)
{
    // Draw legend
    GL_CHECK(m_emptyVao.bind());

        // Draw circle
        GL_CHECK(m_programCircle.bind());

            GL_CHECK(m_programCircle.setUniformValue(m_circleUniforms["length"], m_circleLines));
            GL_CHECK(m_programCircle.setUniformValue(m_circleUniforms["matrix"], m));
            GL_CHECK(m_programCircle.setUniformValue(m_circleUniforms["alpha"], 1.0f));
            GL_CHECK(glDrawArrays(GL_LINE_LOOP, 0, m_circleLines));

        GL_CHECK(m_programCircle.release());

        // Draw ticks
        GL_CHECK(m_programTick.bind());

            GL_CHECK(m_programTick.setUniformValue(m_tickUniforms["length"], m_tickLines));
            GL_CHECK(m_programTick.setUniformValue(m_tickUniforms["matrix"], m));
            GL_CHECK(m_programTick.setUniformValue(m_tickUniforms["alpha"], 1.0f));
            GL_CHECK(glDrawArrays(GL_LINES, 0, m_tickLines * 2));

        GL_CHECK(m_programTick.release());

        // Draw targets
        QVector3D targets[6] = {
            QVector3D(1.0, 0.0, 0.0),
            QVector3D(0.0, 1.0, 0.0),
            QVector3D(0.0, 0.0, 1.0),
            QVector3D(1.0, 1.0, 0.0),
            QVector3D(1.0, 0.0, 1.0),
            QVector3D(0.0, 1.0, 1.0),
        };

        GL_CHECK(m_programTarget.bind());

            GL_CHECK(m_programTarget.setUniformValue(m_targetUniforms["matrix"], m));
            GL_CHECK(m_programTarget.setUniformValue(m_targetUniforms["alpha"], 1.0f));

            for (auto& t : targets) {
                GL_CHECK(m_programTarget.setUniformValue(m_targetUniforms["tColor"], t));
                GL_CHECK(m_programTarget.setUniformValue(m_targetUniforms["tScale"], 0.05f));
                GL_CHECK(glDrawArrays(GL_LINE_LOOP, 0, 4));

                GL_CHECK(m_programTarget.setUniformValue(m_targetUniforms["tColor"], t * 0.75f));
                GL_CHECK(m_programTarget.setUniformValue(m_targetUniforms["tScale"], 0.025f));
                GL_CHECK(glDrawArrays(GL_LINE_LOOP, 0, 4));
            }

        GL_CHECK(m_programTarget.release());

    GL_CHECK(m_emptyVao.release());

    // Draw image
    if (m_textureId == -1)
        return;

    GL_CHECK(m_emptyVao.bind());
    GL_CHECK(m_programScope.bind());
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));

    {
        GLAutoFilterMode autoFilter(m_filtering);

        // Limit sampling to 1920 * 1080
        uint16_t fixed_width = std::min(1920, m_textureSize.width());
        uint16_t fixed_height = std::min(1080, m_textureSize.height());

        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["alpha"], m_alpha * 5));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["matrix"], m));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["width"], fixed_width));
        GL_CHECK(m_programScope.setUniformValue(m_scopeUniforms["height"], fixed_height));
        GL_CHECK(glDrawArrays(m_rasterScan ? GL_LINE_STRIP : GL_POINTS, 0, fixed_width * fixed_height));
    }

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(m_programScope.release());
    GL_CHECK(m_emptyVao.release());
}
