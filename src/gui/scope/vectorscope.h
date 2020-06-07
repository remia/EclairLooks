#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>

#include <gui/common/textureview.h>
#include <gui/scope/glscope.h>


class VectorScopeWidget : public GLScopeWidget
{
  public:
    VectorScopeWidget(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;

    void initScopeGL() override;
    void resizeScopeGL(int w, int h) override;
    void paintScopeGL(const QMatrix4x4 &m) override;

  private:
    void initLegend();
    void initScope();

  private:
    const uint16_t m_circleLines = 360;
    const uint16_t m_tickLines = 360 / 5;
    bool m_rasterScan;

    QOpenGLVertexArrayObject m_emptyVao;

    QOpenGLShaderProgram m_programScope;
    UniformMap m_scopeUniforms;

    QOpenGLShaderProgram m_programCircle;
    UniformMap m_circleUniforms;
    QOpenGLShaderProgram m_programTick;
    UniformMap m_tickUniforms;
    QOpenGLShaderProgram m_programTarget;
    UniformMap m_targetUniforms;
};