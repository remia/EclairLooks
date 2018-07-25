#include "textureview.h"
#include "../utils/gl.h"

#include <cstdlib>

#include <QtCore/qmath.h>
#include <QtCore/QMimeData>
#include <QtWidgets/QMainWindow>
#include <QtGui/QGuiApplication>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QWindow>
#include <QtGui/QMatrix4x4>
#include <QtGui/QScreen>


TextureView::TextureView(QWidget *parent)
    : QOpenGLWidget(parent), m_imagePosition(0.f, 0.f), m_imageScale(1.f),
      m_clickPosition(0.f, 0.f), m_moveDelta(0.f, 0.f)
{
    setFocusPolicy(Qt::ClickFocus);
}

void TextureView::mousePressEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::AltModifier) {
        m_imagePosition = widgetToWorld(event->localPos());
    }
    else if (QGuiApplication::keyboardModifiers() == Qt::NoModifier) {
        setMouseTracking(true);
        m_clickPosition = widgetToNorm(event->localPos());
    }

    update();
}

void TextureView::mouseMoveEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::AltModifier)
        return;
    else if (QGuiApplication::keyboardModifiers() == Qt::NoModifier)
        m_moveDelta = widgetToNorm(event->localPos()) - m_clickPosition;

    update();
}

void TextureView::mouseReleaseEvent(QMouseEvent *event)
{
    setMouseTracking(false);
    m_imagePosition += m_moveDelta;
    m_moveDelta = QPointF(0.f, 0.f);

    update();
}

void TextureView::mouseDoubleClickEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton) {
        resetView();
    }

    update();
}

void TextureView::wheelEvent(QWheelEvent *event)
{
    m_imageScale += event->delta() / 120.0f;
    m_imageScale = std::clamp(m_imageScale, 0.1f, 25.f);

    update();
}

void TextureView::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Backspace:
        resetView();
        break;
      default:
        QWidget::keyPressEvent(event);
  }
}

void TextureView::initializeGL()
{
    initializeOpenGLFunctions();

    GL_CHECK(m_vao.create());
    GL_CHECK(m_vao.bind());

    GLfloat vertices[] = {
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
    };

    GL_CHECK(m_vertices.create());
    GL_CHECK(m_vertices.bind());
    GL_CHECK(m_vertices.allocate(vertices, sizeof(vertices)));
    GL_CHECK(glEnableVertexAttribArray(AttributeLocation::Position));
    GL_CHECK(glVertexAttribPointer(AttributeLocation::Position, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    GL_CHECK(m_colors.create());
    GL_CHECK(m_colors.bind());
    GL_CHECK(m_colors.allocate(colors, sizeof(colors)));
    GL_CHECK(glEnableVertexAttribArray(AttributeLocation::Color));
    GL_CHECK(glVertexAttribPointer(AttributeLocation::Color, 3, GL_FLOAT, GL_FALSE, 0, 0));

    GLfloat texCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    GL_CHECK(m_texCoords.create());
    GL_CHECK(m_texCoords.bind());
    GL_CHECK(m_texCoords.allocate(texCoords, sizeof(texCoords)));
    GL_CHECK(glEnableVertexAttribArray(AttributeLocation::TextureCoord));
    GL_CHECK(glVertexAttribPointer(AttributeLocation::TextureCoord, 2, GL_FLOAT, GL_FALSE, 0, 0));

    GL_CHECK(m_vao.release());

    printOpenGLInfo();
    qInfo() << "OpenGL Initialization done !\n";
}

void TextureView::resizeGL(int w, int h)
{
    const qreal retinaScale = devicePixelRatio();
    GL_CHECK(glViewport(0, 0, w * retinaScale, h * retinaScale));
}

QString TextureView::defaultVertexShader() const
{
    std::string source = R"(
        #version 410 core
        layout(location = 0) in vec2 posAttr;
        layout(location = 1) in vec3 colAttr;
        layout(location = 2) in vec2 texCoordAttr;

        out vec4 col;
        out vec2 texCoord;

        uniform mat4 matrix;

        void main() {
           col = vec4(colAttr, 1.0f);
           texCoord = texCoordAttr;
           gl_Position = matrix * vec4(posAttr, 0.0f, 1.0f);
        }
    )";

    return QString::fromStdString(source);
}

QString TextureView::defaultFragmentShader() const
{
    std::string source = R"(
        #version 410 core
        in vec4 col;
        in vec2 texCoord;

        layout(location = 0) out vec4 fragColor;

        uniform sampler2D imgTex;
        uniform bool imgTexComplete;

        void main() {
           if (!imgTexComplete)
               fragColor = col;
           else
               fragColor = texture(imgTex, texCoord);
        }
    )";

    return QString::fromStdString(source);
}


void TextureView::resetView()
{
    m_imageScale = 1.f;
    m_imagePosition = QPointF(0.f, 0.f);

    update();
}

QPointF TextureView::widgetToNorm(const QPointF & pos) const
{
    return QPointF(1.f * pos.x() / width(), 1.f * pos.y() / height());
}

QPointF TextureView::widgetToWorld(const QPointF & pos) const
{
    return widgetToNorm(pos) * 2.f - QPointF(1.f, 1.f);
}

QOpenGLVertexArrayObject & TextureView::vaoObject()
{
    return m_vao;
}

QMatrix4x4 TextureView::viewMatrix() const
{
    QMatrix4x4 view;

    // Inverse Y scale to account for OpenGL Y axis (bottom top)
    view.scale(m_imageScale, -m_imageScale);

    view.translate(
        m_imagePosition.x() + m_moveDelta.x(),
        m_imagePosition.y() + m_moveDelta.y());

    return view;
}
