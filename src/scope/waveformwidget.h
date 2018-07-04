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
    void keyPressEvent(QKeyEvent *event) override;

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    void resetTexture(const Image & img);
    void updateTexture(QOpenGLTexture & tex);

  private:
    float m_alpha;

    GLuint m_posAttr;
    GLuint m_textureSrcUniform;
    GLuint m_alphaUniform;

    QOpenGLShaderProgram *m_program;
    QOpenGLVertexArrayObject *m_vao;
    QOpenGLBuffer m_vertices;
    QOpenGLTexture * m_textureSrc;
};