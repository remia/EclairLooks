#pragma once

#include <QtCore/QTime>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>
#include <QtWidgets/QOpenGLWidget>


class ImageWidget : public QOpenGLWidget, public QOpenGLFunctions
{
  public:
    ImageWidget(QWidget *parent = nullptr);

    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    virtual QSize minimumSizeHint() const override;

    void initializeTexture(const std::string &path);
    void clearTexture();

    void printOpenGLInfo();
    void checkOpenGLError(const std::string &stmt, const std::string &file, int line);

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

    QTime m_frameTime;
    int m_frameCount;
    int m_frame;
};
