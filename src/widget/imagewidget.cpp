#include "imagewidget.h"
#include "../image.h"
#include "../mainwindow.h"
#include "../utils/gl.h"

#include <cstdlib>

#include <QtCore/qmath.h>
#include <QtCore/QMimeData>
#include <QtWidgets/QMainWindow>
#include <QtGui/QGuiApplication>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QWindow>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>


static std::string fragmentShaderSource = R"(
    #version 410 core
    in vec4 col;
    in vec2 texCoord;

    layout(location = 0) out vec4 fragColor;

    uniform sampler2D imgTexIn;
    uniform sampler2D imgTexOut;
    uniform float sliderPosition;

    void main() {
       if (texCoord.x > sliderPosition)
           fragColor = texture(imgTexOut, texCoord);
       else
           fragColor = texture(imgTexIn, texCoord);

       if (texCoord.x > sliderPosition - 0.001f && texCoord.x < sliderPosition + 0.001f)
           fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    }
)";


ImageWidget::ImageWidget(QWidget *parent)
    : TextureView(parent), m_textureIn(QOpenGLTexture::Target2D),
      m_textureOut(QOpenGLTexture::Target2D), m_sliderPosition(0.5f)
{
    setAcceptDrops(true);
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        setMouseTracking(true);
        m_sliderPosition = widgetToNorm(event->localPos()).x();
    }

    TextureView::mousePressEvent(event);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        m_sliderPosition = widgetToNorm(event->localPos()).x();
    }

    TextureView::mouseMoveEvent(event);
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
        MainWindow * mw = (MainWindow *) parent();
        mw->pipeline().SetInput(img);
    }

    update();
}

QSize ImageWidget::sizeHint() const
{
    return QSize(1024, 768);
}

void ImageWidget::initializeGL()
{
    TextureView::initializeGL();

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader());
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_program.link();

    GL_CHECK(m_matrixUniform = m_program.uniformLocation("matrix"));
    GL_CHECK(m_textureUniformIn = m_program.uniformLocation("imgTexIn"));
    GL_CHECK(m_textureUniformOut = m_program.uniformLocation("imgTexOut"));
    GL_CHECK(m_sliderPosUniform = m_program.uniformLocation("sliderPosition"));
}

void ImageWidget::paintGL()
{
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    bool texComplete = m_textureIn.isStorageAllocated();
    if (!texComplete)
        return;

    vaoObject().bind();
    m_program.bind();

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    m_textureIn.bind();
    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    m_textureOut.bind();

    GL_CHECK(m_program.setUniformValue(m_matrixUniform, setupMVP()));
    GL_CHECK(m_program.setUniformValue(m_sliderPosUniform, m_sliderPosition));
    GL_CHECK(m_program.setUniformValue(m_textureUniformIn, 0));
    GL_CHECK(m_program.setUniformValue(m_textureUniformOut, 1));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (texComplete && m_textureIn.isBound()) {
        m_textureIn.release();
        m_textureOut.release();
    }
    m_program.release();
    vaoObject().release();
}

void ImageWidget::setImage(const Image &img)
{
    makeCurrent();

    createTexture(m_textureIn, img);
    createTexture(m_textureOut, img);

    resetView();

    qInfo() << "Texture - " << img.width() << "X" << img.height() << "~" << img.channels() << "\n";
    qInfo() << "Texture Initialization done !\n";
}

void ImageWidget::updateImage(const Image &img)
{
    QOpenGLTexture::PixelType pixelType;
    QOpenGLTexture::PixelFormat pixelFormat;
    std::array<QOpenGLTexture::SwizzleValue, 4> sw;
    if (!guessPixelsParameters(img, pixelType, pixelFormat, sw))
        return;

    m_textureOut.setData(pixelFormat, pixelType, img.pixels());

    update();

    EmitEvent<Evt::Update>(m_textureOut);
}

void ImageWidget::clearImage()
{
    makeCurrent();

    m_textureIn.destroy();
    m_textureOut.destroy();

    update();
}

QMatrix4x4 ImageWidget::setupMVP() const
{
    // 1. Model
    QMatrix4x4 model;

    // 2. View
    QMatrix4x4 view;

    // Aspect ratio adaptation
    QSize dstSize = this->size();
    QSize srcSize = QSize(m_textureIn.width(), m_textureIn.height());
    float srcRatio = 1.0f * srcSize.width() / srcSize.height();
    float dstRatio = 1.0f * dstSize.width() / dstSize.height();

    if (dstRatio > srcRatio)
        view.scale((srcRatio * dstSize.height()) / dstSize.width(), 1.0);
    else
        view.scale(1.0, (dstSize.width() / srcRatio) / dstSize.height());

    // Viewer settings
    view *= viewMatrix();

    // 3. Projection
    QMatrix4x4 projection;
    // projection.ortho(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f);

    return projection * view * model;
}

void ImageWidget::createTexture(QOpenGLTexture &tex, const Image &img)
{
    QOpenGLTexture::TextureFormat textureFormat = QOpenGLTexture::RGBA32F;
    QOpenGLTexture::PixelType pixelType;
    QOpenGLTexture::PixelFormat pixelFormat;
    std::array<QOpenGLTexture::SwizzleValue, 4> sw;
    if (!guessPixelsParameters(img, pixelType, pixelFormat, sw))
        return;

    tex.destroy();
    tex.setSize(img.width(), img.height());
    tex.setFormat(textureFormat);
    tex.setSwizzleMask(sw[0], sw[1], sw[2], sw[3]);
    tex.setMinificationFilter(QOpenGLTexture::Linear);
    tex.setMagnificationFilter(QOpenGLTexture::Linear);
    tex.allocateStorage();
    tex.setData(pixelFormat, pixelType, img.pixels());

    // Because swizzle mask are disabled on Qt macOS (do not know why.)
    // http://code.qt.io/cgit/qt/qtbase.git/tree/src/gui/opengl/qopengltexture.cpp?h=dev#n4009
    switch (img.format())
    {
        case PixelFormat::GRAY: {
            tex.bind();
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
            tex.release();
            break;
        }
        default: {
            break;
        }
    }
}

bool ImageWidget::guessPixelsParameters(const Image &img, QOpenGLTexture::PixelType &pt,
                                        QOpenGLTexture::PixelFormat &pf,
                                        std::array<QOpenGLTexture::SwizzleValue, 4> &sw)
{
    if (!img) {
        qWarning() << "Could not load image !\n";
        return false;
    }
    else if (img.type() != PixelType::Float) {
        qWarning() << "Image pixel type not supported (must be Float) !\n";
        return false;
    }

    pt = QOpenGLTexture::Float32;
    sw = {{QOpenGLTexture::RedValue, QOpenGLTexture::GreenValue, QOpenGLTexture::BlueValue,
          QOpenGLTexture::AlphaValue}};

    switch (img.format()) {
        case PixelFormat::GRAY: {
            sw[0] = sw[1] = sw[2] = QOpenGLTexture::RedValue;
            pf                    = QOpenGLTexture::Red;
            break;
        }
        case PixelFormat::RGB: {
            pf = QOpenGLTexture::RGB;
            break;
        }
        case PixelFormat::RGBA: {
            pf = QOpenGLTexture::RGBA;
            break;
        }
        default: {
            qInfo() << "Image pixel format not supported !\n";
            return false;
        }
    }

    return true;
}
