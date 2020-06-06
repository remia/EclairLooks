#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>

#include <gui/common/textureview.h>

#include <map>


class Image;

class VectorScopeWidget : public TextureView
{
  public:
    VectorScopeWidget(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void updateTexture(GLint tex);

  private:
    void initLegend();
    void initScope();

    void drawGraph(const QMatrix4x4 &m);

  private:
    typedef std::map<std::string, GLuint> UniformMap;

  private:
    const uint16_t m_circleLines = 360;
    const uint16_t m_tickLines = 360 / 5;
    bool m_rasterScan;
    float m_alpha;

    GLint m_textureId = -1;
    QSize m_textureSize;

    QOpenGLVertexArrayObject m_emptyVao;

    QOpenGLShaderProgram m_programScope;
    UniformMap m_scopeUniforms;

    QOpenGLShaderProgram m_programRasterScope;
    UniformMap m_rasterScopeUniforms;

    QOpenGLShaderProgram m_programCircle;
    UniformMap m_circleUniforms;

    QOpenGLShaderProgram m_programTick;
    UniformMap m_tickUniforms;
};