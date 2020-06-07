#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>

#include <gui/common/textureview.h>
#include <gui/scope/glscope.h>


class WaveformWidget : public GLScopeWidget
{
  public:
    WaveformWidget(QWidget *parent = nullptr);

  public:
    void initScopeGL() override;
    void paintScopeGL(const QMatrix4x4 &m) override;

    void setScopeType(const std::string &type);

  private:
    void initLegend();
    void initScope();
    void paintScope(const QMatrix4x4 &m, uint8_t mode);

  private:
    std::string m_scopeType;

    QOpenGLShaderProgram m_programScope;
    QOpenGLVertexArrayObject m_vaoScope;
    UniformMap m_scopeUniforms;

    QOpenGLShaderProgram m_programLegend;
    QOpenGLVertexArrayObject m_vaoLegend;
    QOpenGLBuffer m_verticesLegend;
    UniformMap m_legendUniforms;
};