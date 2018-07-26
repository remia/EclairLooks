#pragma once

#include "textureview.h"
#include "../utils/event_source.h"

#include <array>


class Image;

typedef EventDesc <
    FuncT<void(QOpenGLTexture &tex)>,
    FuncT<void(const Image &img)>> IWEvtDesc;

class ImageWidget : public TextureView, public EventSource<IWEvtDesc>
{
  public:
    enum Evt { Update = 0, DropImage };

  public:
    ImageWidget(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QSize sizeHint() const override;

    void initializeGL() override;
    void paintGL() override;

  public:
    void setImage(const Image &img);
    void updateImage(const Image &img);
    void clearImage();

  private:
    QMatrix4x4 setupMVP() const;

    void createTexture(QOpenGLTexture &tex, const Image &img);
    bool guessPixelsParameters(
            const Image &img,
            QOpenGLTexture::PixelType & pt,
            QOpenGLTexture::PixelFormat &pf,
            std::array<QOpenGLTexture::SwizzleValue, 4> &sw);

  private:
    GLuint m_matrixUniform;
    GLuint m_textureUniformIn;
    GLuint m_textureUniformOut;
    GLuint m_sliderPosUniform;

    QOpenGLShaderProgram m_program;
    QOpenGLTexture m_textureIn;
    QOpenGLTexture m_textureOut;

    float m_sliderPosition;
};
