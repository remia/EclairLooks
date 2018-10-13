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

    void updateTexture(QOpenGLTexture &tex);

    void setScopeType(const std::string &type);

  private:
    void initLegend();
    void initScope();

    void drawGraph(const QMatrix4x4 &m, uint8_t mode);

  private:
    float m_alpha;
    std::string m_scopeType;

    QOpenGLTexture * m_textureSrc;

    QOpenGLShaderProgram m_programScope;
    QOpenGLVertexArrayObject m_vaoScope;
    GLuint m_scopeAlphaUniform;
    GLuint m_scopeMatrixUniform;
    GLuint m_scopeTextureUniform;
    GLuint m_scopeChannelUniform;
    GLuint m_scopeResolutionWUniform;
    GLuint m_scopeResolutionHUniform;

    QOpenGLShaderProgram m_programLegend;
    QOpenGLVertexArrayObject m_vaoLegend;
    QOpenGLBuffer m_verticesLegend;
    GLuint m_legendColorUniform;
    GLuint m_legendAlphaUniform;
    GLuint m_legendMatrixUniform;
};