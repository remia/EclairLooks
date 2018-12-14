#include "imagewidget.h"
#include "../image.h"
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

    uniform sampler2D imgTexA;
    uniform sampler2D imgTexB;
    uniform float sliderPosition;

    void main() {
       if (texCoord.x > sliderPosition)
           fragColor = texture(imgTexA, texCoord);
       else
           fragColor = texture(imgTexB, texCoord);

       if (texCoord.x > sliderPosition - 0.001f && texCoord.x < sliderPosition + 0.001f)
           fragColor = vec4(0.7, 0.7, 0.7, 1.0);
    }
)";


ImageWidget::ImageWidget(QWidget *parent)
    : TextureView(parent), m_textureA(QOpenGLTexture::Target2D),
      m_textureB(QOpenGLTexture::Target2D), m_sliderPosition(0.5f)
{
    setAcceptDrops(true);
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::NoModifier) {
        setMouseTracking(true);
        m_sliderPosition = widgetToNorm(event->localPos()).x();
    }

    TextureView::mousePressEvent(event);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::NoModifier) {
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
        if (img)
            EmitEvent<DropImage>(img);
    }

    update();
}

void ImageWidget::initializeGL()
{
    TextureView::initializeGL();

    m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, defaultVertexShader());
    m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());
    m_program.link();

    GL_CHECK(m_matrixUniform = m_program.uniformLocation("matrix"));
    GL_CHECK(m_textureUniformA = m_program.uniformLocation("imgTexA"));
    GL_CHECK(m_textureUniformB = m_program.uniformLocation("imgTexB"));
    GL_CHECK(m_sliderPosUniform = m_program.uniformLocation("sliderPosition"));
}

void ImageWidget::resizeGL(int w, int h)
{
    updateAspectRatio();
    TextureView::resizeGL(w, h);
}

void ImageWidget::paintGL()
{
    GL_CHECK(glClearColor(0.0, 0.0, 0.0, 0.0));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    bool texComplete = m_textureA.isStorageAllocated();
    if (!texComplete)
        return;

    vaoObject().bind();
    m_program.bind();

    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    m_textureA.bind();
    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    m_textureB.bind();

    GL_CHECK(m_program.setUniformValue(m_matrixUniform, setupMVP()));
    GL_CHECK(m_program.setUniformValue(m_sliderPosUniform, m_sliderPosition));
    GL_CHECK(m_program.setUniformValue(m_textureUniformA, 0));
    GL_CHECK(m_program.setUniformValue(m_textureUniformB, 1));

    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

    if (texComplete && m_textureA.isBound()) {
        m_textureA.release();
        m_textureB.release();
    }
    m_program.release();
    vaoObject().release();
}

bool ImageWidget::hasImage() const
{
    return m_textureA.isStorageAllocated();
}

void ImageWidget::resetImage(const Image &img)
{
    makeCurrent();

    m_textureA.destroy();
    m_textureB.destroy();

    createTexture(m_textureA, img);
    createTexture(m_textureB, img);

    updateAspectRatio();

    resetView();

    doneCurrent();
}

void ImageWidget::updateImage(SideBySide sbs, const Image &img)
{
    QOpenGLTexture::PixelType pixelType;
    QOpenGLTexture::PixelFormat pixelFormat;
    std::array<QOpenGLTexture::SwizzleValue, 4> sw;
    if (!guessPixelsParameters(img, pixelType, pixelFormat, sw))
        return;

    QOpenGLTexture *texture = nullptr;
    switch (sbs) {
        case SideBySide::A:
            texture = &m_textureA;
            break;
        case SideBySide::B:
            texture = &m_textureB;
            break;
    }

    if (texture->width() != img.width() || texture->height() != img.height())
        resetImage(img);

    texture->setData(pixelFormat, pixelType, img.pixels());

    update();

    EmitEvent<Evt::Update>(*texture);
}

GLint ImageWidget::texture()
{
    return m_textureA.textureId();
}

void ImageWidget::updateAspectRatio()
{
    // Aspect ratio adaptation
    QSize dstSize = this->size();
    QSize srcSize = QSize(m_textureA.width(), m_textureA.height());
    float srcRatio = 1.0f * srcSize.width() / srcSize.height();
    float dstRatio = 1.0f * dstSize.width() / dstSize.height();

    if (dstRatio > srcRatio)
        setTextureRatio((srcRatio * dstSize.height()) / dstSize.width(), 1.0);
    else
        setTextureRatio(1.0, (dstSize.width() / srcRatio) / dstSize.height());
}

QMatrix4x4 ImageWidget::setupMVP() const
{
    // 1. Model
    QMatrix4x4 model;

    // 2. View
    QMatrix4x4 view = viewMatrix();

    // 3. Projection
    QMatrix4x4 projection;

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
