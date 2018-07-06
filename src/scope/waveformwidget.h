#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>


class Image;

class WaveformWidget : public QOpenGLWidget, public QOpenGLFunctions
{
  public:
    WaveformWidget(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    void resetTexture(const Image & img);
    void updateTexture(QOpenGLTexture & tex);

  private:
    QPointF widgetToNorm(const QPointF &pos) const;
    QPointF widgetToWorld(const QPointF &pos) const;

  private:
    float m_alpha;

    GLuint m_posAttr;
    GLuint m_textureSrcUniform;
    GLuint m_alphaUniform;
    GLuint m_matrixUniform;
    GLuint m_legendColorUniform;
    GLuint m_legendAlphaUniform;
    GLuint m_legendMatrixUniform;

    QOpenGLShaderProgram *m_program;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLBuffer m_vertices;
    QOpenGLTexture * m_textureSrc;

    QOpenGLShaderProgram *m_programLegend;
    QOpenGLVertexArrayObject *m_vaoLegend;
    QOpenGLBuffer m_verticesLegend;

    QPointF m_imagePosition;
    float m_imageScale;
    QPointF m_clickPosition;
    QPointF m_moveDelta;
};