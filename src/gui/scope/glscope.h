#pragma once

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLFunctions>

#include <gui/common/textureview.h>

#include <map>


class GLScopeWidget : public TextureView
{
  public:
    typedef std::map<std::string, GLuint> UniformMap;

  public:
    GLScopeWidget(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    virtual void initScopeGL() {}
    virtual void resizeScopeGL(int w, int h) {}
    virtual void paintScopeGL(const QMatrix4x4 &m) = 0;

    void updateTexture(GLint tex);

  protected:
    bool m_filtering;
    float m_alpha;

    GLint m_textureId = -1;
    QSize m_textureSize;
};

struct GLAutoFilterMode
{
    GLAutoFilterMode(bool filter);
    ~GLAutoFilterMode();

    bool filter;
    GLint minFilter, magFilter;
};