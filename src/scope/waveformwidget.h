#pragma once

#include "../widget/textureview.h"

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>


class Image;

class WaveformWidget : public TextureView
{
  public:
    WaveformWidget(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;

    void initializeGL() override;
    void paintGL() override;

    QSize sizeHint() const override;

    void resetTexture(const Image & img);
    void updateTexture(QOpenGLTexture &tex);

  private:
    void initLegend();
    void initScope(uint16_t w, uint16_t h);

  private:
    float m_alpha;

    QOpenGLTexture * m_textureSrc;

    QOpenGLShaderProgram m_programScope;
    QOpenGLVertexArrayObject m_vaoScope;
    QOpenGLBuffer m_verticesScope;
    GLuint m_scopeAlphaUniform;
    GLuint m_scopeMatrixUniform;
    GLuint m_scopegMatrixUniform;
    GLuint m_scopeTextureUniform;

    QOpenGLShaderProgram m_programLegend;
    QOpenGLVertexArrayObject m_vaoLegend;
    QOpenGLBuffer m_verticesLegend;
    GLuint m_legendColorUniform;
    GLuint m_legendAlphaUniform;
    GLuint m_legendMatrixUniform;
};