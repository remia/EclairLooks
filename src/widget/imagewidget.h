#pragma once

#include "../utils/event_source.h"

#include <array>

#include <QtCore/QTime>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtWidgets/QOpenGLWidget>


enum class ImageWidgetEvent { Update };

class Image;

class ImageWidget : public QOpenGLWidget, public QOpenGLFunctions, public EventSource<ImageWidgetEvent>
{
  public:
    using UpdateT = FuncT<void(QOpenGLTexture &tex)>;

  public:
    ImageWidget(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    virtual QSize sizeHint() const override;

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    void setImage(const Image &img);
    void updateImage(const Image &img);
    void clearImage();
    void resetViewer();

  private:
    QPointF widgetToNorm(const QPointF & pos) const;
    QPointF widgetToWorld(const QPointF & pos) const;

    void createTexture(QOpenGLTexture &tex, const Image &img);

    bool guessPixelsParameters(
            const Image &img,
            QOpenGLTexture::PixelType & pt,
            QOpenGLTexture::PixelFormat &pf,
            std::array<QOpenGLTexture::SwizzleValue, 4> &sw);

  private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_texCoordAttr;
    GLuint m_matrixUniform;
    GLuint m_textureUniformIn;
    GLuint m_textureUniformOut;
    GLuint m_textureCompleteUniform;
    GLuint m_sliderPosUniform;

    QOpenGLShaderProgram *m_program;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLBuffer m_vertices;
    QOpenGLBuffer m_colors;
    QOpenGLBuffer m_texCoords;
    QOpenGLTexture m_textureIn;
    QOpenGLTexture m_textureOut;

    QPointF m_imagePosition;
    float m_imageScale;

    QPointF m_clickPosition;
    QPointF m_moveDelta;

    float m_sliderPosition;
};
