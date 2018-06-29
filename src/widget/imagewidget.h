#pragma once

#include <array>

#include <QtCore/QTime>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtWidgets/QOpenGLWidget>


class Image;

class ImageWidget : public QOpenGLWidget, public QOpenGLFunctions
{
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
    void printOpenGLInfo();
    void checkOpenGLError(const std::string &stmt, const std::string &file, int line);

    QPointF widgetToNorm(const QPointF & pos) const;
    QPointF widgetToWorld(const QPointF & pos) const;

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
    GLuint m_textureUniform;
    GLuint m_textureCompleteUniform;

    QOpenGLShaderProgram *m_program;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLBuffer m_vertices;
    QOpenGLBuffer m_colors;
    QOpenGLBuffer m_texCoords;
    QOpenGLTexture m_texture;

    QPointF m_imagePosition;
    float m_imageScale;

    QPointF m_clickPosition;
    QPointF m_moveDelta;
};
