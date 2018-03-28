#include "imagewidget.h"
#include "image.h"
#include "mainwindow.h"

#include <iostream>
#include <cstdlib>

#include <QtCore/qmath.h>
#include <QtCore/QMimeData>
#include <QtWidgets/QMainWindow>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QWindow>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>


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
    "uniform bool imgTexComplete;\n"
    "out vec4 fragcolor;\n"
    "void main() {\n"
    "   if (imgTexComplete)"
    "       fragcolor = texture(imgTex, texCoord);\n"
    "   else\n"
    "       fragcolor = col;\n"
    "}\n";

ImageWidget::ImageWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_program(0), m_vao(0), m_texture(QOpenGLTexture::Target2D), m_frameCount(0),
      m_frame(0)
{
    m_frameTime.start();
    setAcceptDrops(true);
}

void ImageWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void ImageWidget::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName << "\n";

        Image img = Image::FromFile(fileName.toStdString());
        if (!img) {
            qWarning() << "Could not load image !\n";
            return;
        }
        else if (img.type() != PixelType::Float) {
            qWarning() << "Image pixel type not supported (must be half) !\n";
            return;
        }

        MainWindow * mw = (MainWindow *) parent();
        mw->pipeline().SetInput(img);
        setImage(mw->pipeline().GetOutput());
    }
}

QSize ImageWidget::minimumSizeHint() const
{
    return QSize(1024, 728);
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
    m_textureCompleteUniform = m_program->uniformLocation("imgTexComplete");

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

    printOpenGLInfo();

    qInfo() << "OpenGL Initialization done !\n";
}

void ImageWidget::resizeGL(int w, int h)
{
    const qreal retinaScale = devicePixelRatio();
    GL_CHECK(glViewport(0, 0, w * retinaScale, h * retinaScale));
}

void ImageWidget::paintGL()
{
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    bool texComplete = m_texture.isStorageAllocated();

    m_vao->bind();
    m_program->bind();
    if (texComplete)
        m_texture.bind();

    QMatrix4x4 matrix;
    if (!texComplete)
        matrix.scale(sin(m_frame / window()->windowHandle()->screen()->refreshRate()));
    matrix.ortho(0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);

    m_program->setUniformValue(m_matrixUniform, matrix);
    m_program->setUniformValue(m_textureCompleteUniform, texComplete);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (texComplete && m_texture.isBound())
        m_texture.release();
    m_program->release();
    m_vao->release();

    ++m_frame;
    ++m_frameCount;

    if (m_frameTime.elapsed() >= 1000) {
      double fps = m_frameCount / ((double) m_frameTime.elapsed() / 1000.0);
      m_frameTime.restart();
      m_frameCount = 0;

      QMainWindow * window = (QMainWindow *) parent();
      window->setWindowTitle(QString("%1 fps").arg(fps));
    }

    update();
}

void ImageWidget::setImage(const Image &img)
{
    makeCurrent();

    if (!img) {
        qWarning() << "Could not load image !\n";
        return;
    }
    else if (img.type() != PixelType::Float) {
        qWarning() << "Image pixel type not supported (must be half) !\n";
        return;
    }

    QOpenGLTexture::PixelType pixelType = QOpenGLTexture::Float32;
    QOpenGLTexture::TextureFormat textureFormat = QOpenGLTexture::RGB32F;
    QOpenGLTexture::PixelFormat pixelFormat = QOpenGLTexture::RGB;

    if (img.format() == PixelFormat::RGBA) {
        textureFormat = QOpenGLTexture::RGBA32F;
        pixelFormat = QOpenGLTexture::RGBA;
    }

    m_texture.destroy();
    m_texture.setSize(img.width(), img.height());
    m_texture.setFormat(textureFormat);
    m_texture.setMinificationFilter(QOpenGLTexture::Linear);
    m_texture.setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture.allocateStorage();
    m_texture.setData(pixelFormat, pixelType, img.pixels());

    qInfo() << "Texture - " << img.width() << "X" << img.height() << "~" << img.channels() << "\n";
    qInfo() << "Texture Initialization done !\n";
}

void ImageWidget::clearImage()
{
    makeCurrent();

    m_texture.destroy();
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