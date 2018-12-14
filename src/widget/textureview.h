#pragma once

#include <QtGui/QOpenGLExtraFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtWidgets/QOpenGLWidget>


class TextureView : public QOpenGLWidget, public QOpenGLExtraFunctions
{
  public:
    TextureView(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent * e) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;

  protected:
    void setDefaultScale(float s);
    void setTextureRatio(float x, float y);

    QString defaultVertexShader() const;
    QString defaultFragmentShader() const;

    QPointF widgetToNorm(const QPointF &pos) const;
    QPointF widgetToWorld(const QPointF & pos) const;
    QPointF widgetToTexture(const QPointF & pos) const;

    QOpenGLVertexArrayObject & vaoObject();
    QMatrix4x4 viewMatrix() const;

    void resetView();

  private:
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vertices;
    QOpenGLBuffer m_colors;
    QOpenGLBuffer m_texCoords;

    QPointF m_imagePosition;
    float m_imageScale;
    QPointF m_textureRatio;
    QPointF m_clickPosition;
    QPointF m_moveDelta;

    float m_defaultScale = 1.f;
};
