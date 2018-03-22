#include "imagewidget.h"

#include <iostream>
#include <cstdlib>

#include <QtGui/QWindow>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>

#include <OpenImageIO/imageio.h>


#define GL_CHECK(stmt) \
    stmt; \
    ImageWidget::checkOpenGLError(#stmt, __FILE__, __LINE__); \


static const char *vertexShaderSource =
    "#version 410\n"
    "in vec2 posAttr;\n"
    "in vec3 colAttr;\n"
    "in vec2 texCoordAttr;\n"
    "uniform mat4 matrix;\n"
    "out vec4 col;\n"
    "out vec2 texCoord;\n"
    "void main() {\n"
    "   col = vec4(colAttr, 1.0f);\n"
    "   texCoord = texCoordAttr;\n"
    "   gl_Position = matrix * vec4(posAttr, 0.0f, 1.0f);\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 410\n"
    "in vec4 col;\n"
    "in vec2 texCoord;\n"
    "uniform sampler2D imgTex;\n"
    "out vec4 fragcolor;\n"
    "void main() {\n"
    "   fragcolor = col;\n"
    "   fragcolor = texture(imgTex, texCoord);\n"
    "}\n";

ImageWidget::ImageWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_program(0), m_vao(0),
      m_texture(QOpenGLTexture::Target2D), m_frameCount(0), m_frame(0) {
  m_frameTime.start();
}

void ImageWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_texCoordAttr = m_program->attributeLocation("texCoordAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
    m_textureUniform = m_program->uniformLocation("imgTex");

    m_vao = new QOpenGLVertexArrayObject(this);
    GL_CHECK(m_vao->create());
    GL_CHECK(m_vao->bind());

    GLfloat vertices[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    GL_CHECK(m_vertices.create());
    GL_CHECK(m_vertices.bind());
    GL_CHECK(m_vertices.allocate(vertices, sizeof(vertices)));
    GL_CHECK(glEnableVertexAttribArray(m_posAttr));
    GL_CHECK(glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    GL_CHECK(m_colors.create());
    GL_CHECK(m_colors.bind());
    GL_CHECK(m_colors.allocate(colors, sizeof(colors)));
    GL_CHECK(glEnableVertexAttribArray(m_colAttr));
    GL_CHECK(glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, 0));

    GLfloat texCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    GL_CHECK(m_texCoords.create());
    GL_CHECK(m_texCoords.bind());
    GL_CHECK(m_texCoords.allocate(texCoords, sizeof(texCoords)));
    GL_CHECK(glEnableVertexAttribArray(m_texCoordAttr));
    GL_CHECK(glVertexAttribPointer(m_texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vao->release());

    initializeTexture();

    printOpenGLInfo();

    qInfo() << "OpenGL Initialization done !\n";
}

void ImageWidget::initializeTexture()
{
    // Experiment in progres...
    using namespace OIIO;
    std::string filename = "/Users/remi/ownCloud/Images/stresstest/LUT_Stress_Test_HD_20161224.tif";
    ImageInput *in = ImageInput::open(filename);
    if (! in)
       return;
    const ImageSpec &spec = in->spec();
    int xres = spec.width;
    int yres = spec.height;
    int channels = spec.nchannels;
    std::cerr << xres << "X" << yres << "~" << channels << "\n";
    std::vector<unsigned char> pixels(xres*yres*channels);
    in->read_image(TypeDesc::HALF, pixels.data());
    in->close();
    ImageInput::destroy(in);

    m_texture.setSize(xres, yres);
    m_texture.setFormat(QOpenGLTexture::RGB16F);
    m_texture.setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture.setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture.allocateStorage();
    m_texture.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float16, pixels.data());

    qInfo() << "Texture Initialization done !\n";
}

void ImageWidget::resizeGL(int w, int h)
{
    const qreal retinaScale = devicePixelRatio();
    GL_CHECK(glViewport(0, 0, w * retinaScale, h * retinaScale));
}

void ImageWidget::paintGL()
{
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    m_vao->bind();
    m_program->bind();
    m_texture.bind();

    QMatrix4x4 matrix;
    matrix.ortho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    // matrix.rotate(100.0f * m_frame / window()->windowHandle()->screen()->refreshRate(), 0, 1, 0);
    // matrix.rotate(100.0f * m_frame / window()->windowHandle()->screen()->refreshRate(), 0, 0, 1);

    m_program->setUniformValue(m_matrixUniform, matrix);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_texture.release();
    m_program->release();
    m_vao->release();

    ++m_frame;
    ++m_frameCount;

    if (m_frameTime.elapsed() >= 1000) {
      double fps = m_frameCount / ((double) m_frameTime.elapsed() / 1000.0);
      m_frameTime.restart();
      m_frameCount = 0;
      qInfo() << "FPS : " << fps << "\n";
    }

    update();
}

QSize ImageWidget::minimumSizeHint() const
{
    return QSize(1024, 728);
}

void ImageWidget::printOpenGLInfo()
{
    auto gl_vendor = QString(reinterpret_cast<char const *>(glGetString(GL_VENDOR)));
    auto gl_renderer = QString(reinterpret_cast<char const *>(glGetString(GL_RENDERER)));
    auto gl_version = QString(reinterpret_cast<char const *>(glGetString(GL_VERSION)));
    auto gl_glsl_version = QString(reinterpret_cast<char const *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

    qInfo()
        << "OpenGL Context :\n"
        << "\tVendor : " << gl_vendor << "\n"
        << "\tRenderer : " << gl_renderer << "\n"
        << "\tVersion : " << gl_version << "\n"
        << "\tGLSL Version : " << gl_glsl_version << "\n";
}

void ImageWidget::checkOpenGLError(const std::string &stmt,
                                  const std::string &file, int line) {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    qCritical()
        << "OpenGL error " << err
        << " at " << QString::fromStdString(file) << ":" << line
        << " for " << QString::fromStdString(stmt) << "\n";
  }
}