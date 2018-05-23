#include "imagewidget.h"
#include "image.h"
#include "mainwindow.h"

#include <iostream>
#include <cstdlib>

#include <QtCore/qmath.h>
#include <QtCore/QMimeData>
#include <QtWidgets/QMainWindow>
#include <QtGui/QGuiApplication>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QWindow>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>


#define GL_CHECK(stmt)                                                                                       \
    stmt;                                                                                                    \
    ImageWidget::checkOpenGLError(#stmt, __FILE__, __LINE__);


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
      m_frame(0), m_imagePosition(0.f, 0.f), m_imageScale(1.f), m_clickPosition(0.f, 0.f),
      m_moveDelta(0.f, 0.f)
{
    m_frameTime.start();
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::AltModifier){
        m_imagePosition = widgetToWorld(event->localPos());
    }
    else {
        setMouseTracking(true);
        m_clickPosition = widgetToNorm(event->localPos());
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_moveDelta = widgetToNorm(event->localPos()) - m_clickPosition;
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    setMouseTracking(false);
    m_imagePosition += m_moveDelta;
    m_moveDelta = QPointF(0.f, 0.f);
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    m_imageScale += event->delta() / 120.0f;
    m_imageScale = std::clamp(m_imageScale, 0.1f, 25.f);
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

void ImageWidget::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Backspace:
        resetViewer();
        break;
      default:
        QWidget::keyPressEvent(event);
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
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
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

    QMatrix4x4 projection;
    projection.ortho(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f);

    QMatrix4x4 view;
    view.scale(m_imageScale, m_imageScale);
    view.translate(
        m_imagePosition.x() + m_moveDelta.x(),
        m_imagePosition.y() + m_moveDelta.y());

    QMatrix4x4 model;

    QMatrix4x4 mvp = projection * view * model;

    m_program->setUniformValue(m_matrixUniform, mvp);
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
    QOpenGLTexture::TextureFormat textureFormat = QOpenGLTexture::RGBA32F;
    QOpenGLTexture::PixelFormat pixelFormat;
    QOpenGLTexture::SwizzleValue swizzleRGBA[4] = {QOpenGLTexture::RedValue, QOpenGLTexture::GreenValue,
                                                   QOpenGLTexture::BlueValue, QOpenGLTexture::AlphaValue};

    switch (img.format())
    {
        case PixelFormat::GRAY: {
            swizzleRGBA[0] = swizzleRGBA[1] = swizzleRGBA[2] = QOpenGLTexture::RedValue;
            pixelFormat = QOpenGLTexture::Red;
            break;
        }
        case PixelFormat::RGB: {
            pixelFormat = QOpenGLTexture::RGB;
            break;
        }
        case PixelFormat::RGBA: {
            pixelFormat = QOpenGLTexture::RGBA;
            break;
        }
        default: {
            qInfo() << "Image pixel format not supported !\n";
            return;
        }
    }

    m_texture.destroy();
    m_texture.setSize(img.width(), img.height());
    m_texture.setFormat(textureFormat);
    m_texture.setSwizzleMask(swizzleRGBA[0], swizzleRGBA[1], swizzleRGBA[2], swizzleRGBA[3]);
    m_texture.setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture.setMagnificationFilter(QOpenGLTexture::Nearest);
    m_texture.allocateStorage();
    m_texture.setData(pixelFormat, pixelType, img.pixels());

    // Because swizzle mask are disabled on Qt macOS (do not know why.)
    // http://code.qt.io/cgit/qt/qtbase.git/tree/src/gui/opengl/qopengltexture.cpp?h=dev#n4009
    switch (img.format())
    {
        case PixelFormat::GRAY: {
            m_texture.bind();
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            m_texture.release();
            break;
        }
        default: {
            break;
        }
    }

    resetViewer();

    qInfo() << "Texture - " << img.width() << "X" << img.height() << "~" << img.channels() << "\n";
    qInfo() << "Texture Initialization done !\n";
}

void ImageWidget::clearImage()
{
    makeCurrent();

    m_texture.destroy();
}

void ImageWidget::resetViewer()
{
    m_imageScale = 1.f;
    m_imagePosition = QPointF(0.f, 0.f);
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

QPointF ImageWidget::widgetToNorm(const QPointF & pos) const
{
    return QPointF(1.f * pos.x() / width(), 1.f * pos.y() / height());
}

QPointF ImageWidget::widgetToWorld(const QPointF & pos) const
{
    return widgetToNorm(pos) * 2.f - QPointF(1.f, 1.f);
}
