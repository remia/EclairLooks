#include "vectorscope.h"

#include <cassert>
#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#include <core/image.h>
#include <utils/generic.h>
#include <utils/gl.h>

static std::string commonFunctions = R"(
    // RGB to Y'PbPr (BT.601)
    // See Digital Video and HD 2nd, Poynton, Eq 29.2
    vec3 RGBtoYUV_601(vec3 rgb)
    {
        vec3 yuv;
        yuv.x = (rgb.x * 0.299) + (rgb.y * 0.587) + (rgb.z * 0.114);
        yuv.y = ((rgb.z - yuv.x) * 0.564) + 0.5;
        yuv.z = ((rgb.x - yuv.x) * 0.713) + 0.5;
        return yuv;
    }

    // RGB to Y'PbPr (BT.709)
    // See Digital Video and HD 2nd, Poynton, Eq 30.1
    vec3 RGBtoYUV_709(vec3 rgb)
    {
        vec3 yuv;
        yuv.x = (rgb.x * 0.2126) + (rgb.y * 0.7152) + (rgb.z * 0.0722);
        yuv.y = ((rgb.z - yuv.x) * 0.5389) + 0.5;
        yuv.z = ((rgb.x - yuv.x) * 0.6350) + 0.5;
        return yuv;
    }
)";

static std::string vertexShaderRasterSource = R"(
    #version 410 core
    out vec4 vertexColor;
    out vec2 prevPos;
    out vec4 prevColor;

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
        gl_Position.xy = (gl_Position.xy * 2.0) - 1.;
        gl_Position.y *= -1.;
        gl_Position = matrix * gl_Position;
        vertexColor = rgb;

        rgb = texture(v_tex, vec2((x - 1.0) / width, 1.0f - pix.y));
        yuv = RGBtoYUV_709(rgb.xyz);
        vec4 pos = vec4(yuv.yz, 0.0, 1.0);
        pos.xy = (pos.xy * 2.0) - 1.;
        pos.y *= -1.;
        pos = matrix * pos;
        prevColor = rgb;
        prevPos = pos.xy;
    }
)";

static std::string geometryShaderRasterSource = R"(
    #version 410 core

    layout(points) in;
    layout(line_strip, max_vertices = 2) out;

    in vec4 vertexColor[];
    in vec2 prevPos[];
    in vec4 prevColor[];
    out vec4 color;

    void main()
    {
        color = prevColor[0];
        color.a = 0.2;
        gl_Position = vec4(prevPos[0], 0.0, 1.0);
        EmitVertex();

        color = vertexColor[0];
        color.a = 0.2;
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();

        EndPrimitive();
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
        gl_Position.xy = (gl_Position.xy * 2.0) - 1.;
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
        float angle = 2.0 * M_PI * gl_VertexID / (length - 1);

        gl_Position = vec4(cos(angle), sin(angle), 0.0, 1.0);
        gl_Position = matrix * gl_Position;

        color = vec4(0.3,0.2, 0.05, 1.0);
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

        if (gl_VertexID % 2 == 0 && id % (length / 4) == 0)
            gl_Position.xy = vec2(0.0, 0.0);
        else if (gl_VertexID % 4 == 0)
            gl_Position.xy *= 0.9;
        else if (gl_VertexID % 2 == 0)
            gl_Position.xy *= 0.95;

        gl_Position = matrix * gl_Position;

        color = vec4(0.3,0.2, 0.05, 1.0);
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
    : TextureView(parent), m_rasterScan(false), m_alpha(0.1f)
{

}

void VectorScopeWidget::resizeGL(int w, int h)
{
    setAspectRatio(1.0f);

    TextureView::resizeGL(w, h);
}

void VectorScopeWidget::keyPressEvent(QKeyEvent *event)
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
      case Qt::Key_T:
        m_rasterScan = not m_rasterScan;
        update();
        break;
      default:
        QWidget::keyPressEvent(event);
  }

  TextureView::keyPressEvent(event);
}

void VectorScopeWidget::initializeGL()
{
    initializeOpenGLFunctions();

    initLegend();
    initScope();
}

void VectorScopeWidget::paintGL()
{
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    drawGraph(viewMatrix());
}

void VectorScopeWidget::updateTexture(GLint tex)
{
    makeCurrent();

    m_textureId = tex;

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));
    GL_CHECK(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_textureSize.rwidth()));
    GL_CHECK(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_textureSize.rheight()));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    doneCurrent();

    update();
}

void VectorScopeWidget::initLegend()
{
    m_programCircle.removeAllShaders();
    m_programCircle.addShaderFromSourceCode(QOpenGLShader::Vertex, circleVertexShaderSource.c_str());
    m_programCircle.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programCircle.link();
    if (!m_programCircle.isLinked())
        qWarning() << m_programCircle.log() << "\n";

    GL_CHECK(m_circleUniforms["length"] = m_programCircle.uniformLocation("length"));
    GL_CHECK(m_circleUniforms["alpha"] = m_programCircle.uniformLocation("alpha"));
    GL_CHECK(m_circleUniforms["matrix"] = m_programCircle.uniformLocation("matrix"));

    m_programTick.removeAllShaders();
    m_programTick.addShaderFromSourceCode(QOpenGLShader::Vertex, tickVertexShaderSource.c_str());
    m_programTick.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programTick.link();
    if (!m_programTick.isLinked())
        qWarning() << m_programTick.log() << "\n";

    GL_CHECK(m_tickUniforms["length"] = m_programTick.uniformLocation("length"));
    GL_CHECK(m_tickUniforms["alpha"] = m_programTick.uniformLocation("alpha"));
    GL_CHECK(m_tickUniforms["matrix"] = m_programTick.uniformLocation("matrix"));
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

    m_programRasterScope.removeAllShaders();
    QString vertexSourceRaster = QString::fromStdString(vertexShaderRasterSource)
        .replace(QRegExp("\\{\\{functions\\}\\}"), QString::fromStdString(commonFunctions));
    m_programRasterScope.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSourceRaster);
    m_programRasterScope.addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderRasterSource.c_str());
    m_programRasterScope.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_programRasterScope.link();
    if (!m_programRasterScope.isLinked())
        qWarning() << m_programRasterScope.log() << "\n";

    GL_CHECK(m_rasterScopeUniforms["v_tex"] = m_programRasterScope.uniformLocation("v_tex"));
    GL_CHECK(m_rasterScopeUniforms["alpha"] = m_programRasterScope.uniformLocation("alpha"));
    GL_CHECK(m_rasterScopeUniforms["matrix"] = m_programRasterScope.uniformLocation("matrix"));
    GL_CHECK(m_rasterScopeUniforms["width"] = m_programRasterScope.uniformLocation("width"));
    GL_CHECK(m_rasterScopeUniforms["height"] = m_programRasterScope.uniformLocation("height"));

    GL_CHECK(m_emptyVao.destroy());
    GL_CHECK(m_emptyVao.create());
}

void VectorScopeWidget::drawGraph(const QMatrix4x4 &m)
{
    // Draw legend
    GL_CHECK(m_emptyVao.bind());

        // Draw circle
        GL_CHECK(m_programCircle.bind());

            GL_CHECK(m_programCircle.setUniformValue(m_circleUniforms["length"], m_circleLines));
            GL_CHECK(m_programCircle.setUniformValue(m_circleUniforms["matrix"], m));
            GL_CHECK(m_programCircle.setUniformValue(m_circleUniforms["alpha"], 1.0f));
            GL_CHECK(glDrawArrays(GL_LINE_STRIP, 0, m_circleLines));

        GL_CHECK(m_programCircle.release());

        // Draw ticks
        GL_CHECK(m_programTick.bind());

            GL_CHECK(m_programTick.setUniformValue(m_tickUniforms["length"], m_tickLines));
            GL_CHECK(m_programTick.setUniformValue(m_tickUniforms["matrix"], m));
            GL_CHECK(m_programTick.setUniformValue(m_tickUniforms["alpha"], 1.0f));
            GL_CHECK(glDrawArrays(GL_LINES, 0, m_tickLines * 2));

        GL_CHECK(m_programTick.release());

    GL_CHECK(m_emptyVao.release());

    // Draw image
    if (m_textureId == -1)
        return;

    QOpenGLShaderProgram* program;
    UniformMap* uniforms;
    if (m_rasterScan) {
        program = &m_programRasterScope;
        uniforms = &m_rasterScopeUniforms;
    }
    else {
        program = &m_programScope;
        uniforms = &m_scopeUniforms;
    }

    GL_CHECK(m_emptyVao.bind());
    GL_CHECK(program->bind());
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));

    // Turn off any filtering that could produce colors not in the original
    // image, this is needed because we access the texture using normalized
    // coordinates. Then restore originals parameters.
    GLint minFilter, magFilter;
    GL_CHECK(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter));
    GL_CHECK(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GL_CHECK(program->setUniformValue((*uniforms)["alpha"], m_alpha));
        GL_CHECK(program->setUniformValue((*uniforms)["matrix"], m));
        GL_CHECK(program->setUniformValue((*uniforms)["width"], m_textureSize.width()));
        GL_CHECK(program->setUniformValue((*uniforms)["height"], m_textureSize.height()));
        GL_CHECK(glDrawArrays(GL_POINTS, 0, m_textureSize.width() * m_textureSize.height()));

    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(program->release());
    GL_CHECK(m_emptyVao.release());
}
