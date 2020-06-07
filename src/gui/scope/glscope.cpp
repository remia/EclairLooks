#include "glscope.h"

#include <cassert>
#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#include <context.h>
#include <core/image.h>
#include <utils/generic.h>
#include <utils/gl.h>


GLScopeWidget::GLScopeWidget(QWidget *parent)
    : TextureView(parent), m_filtering(true), m_alpha(0.01f)
{

}

void GLScopeWidget::resizeGL(int w, int h)
{
    resizeScopeGL(w, h);
    TextureView::resizeGL(w, h);
}

void GLScopeWidget::keyPressEvent(QKeyEvent *event)
{
    auto& ctx = Context::getInstance();
    auto keySeq = QKeySequence(event->key() | event->modifiers());

    if (keySeq == ctx.shortcut("Scope_AlphaUp")) {
        m_alpha *= 1.2f;
        m_alpha = std::clamp(m_alpha, 0.001f, 1.0f);
        update();
    }
    else if (keySeq == ctx.shortcut("Scope_AlphaDown")) {
        m_alpha *= 0.8f;
        m_alpha = std::clamp(m_alpha, 0.001f, 1.0f);
        update();
    }
    else if (keySeq == ctx.shortcut("Scope_FilteringToggle")) {
        m_filtering = not m_filtering;
        update();
    }
    else {
        QWidget::keyPressEvent(event);
    }

    TextureView::keyPressEvent(event);
}

void GLScopeWidget::initializeGL()
{
    initializeOpenGLFunctions();
    initScopeGL();
}

void GLScopeWidget::paintGL()
{
    GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    paintScopeGL(viewMatrix());
}

void GLScopeWidget::updateTexture(GLint tex)
{
    makeCurrent();

    m_textureId = tex;

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_textureId));
    GL_CHECK(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_textureSize.rwidth()));
    GL_CHECK(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_textureSize.rheight()));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    doneCurrent();

    update();
}

// Note about filtering in scopes:
// Turn off any filtering that could produce colors not in the original
// image, this is needed because we access the texture using normalized
// coordinates. Then restore originals parameters. The main drawback is
// that for heavily compressed images, it makes disturbing quantization
// patterns appear in the vectorscope.

// Target texture have to be bound to use this class
GLAutoFilterMode::GLAutoFilterMode(bool filter)
: filter(filter)
{
    if (not filter) {
        GL_CHECK(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter));
        GL_CHECK(glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }
}

GLAutoFilterMode::~GLAutoFilterMode()
{
    if (not filter) {
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
    }
}