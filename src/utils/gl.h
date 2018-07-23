#pragma once

#include <QtCore/QDebug>
#include <QtGui/QOpenGLFunctions>


#define GL_CHECK(stmt)                                                                   \
    stmt;                                                                                \
    checkOpenGLError(#stmt, __FILE__, __LINE__);

inline void checkOpenGLError(const std::string &stmt, const std::string &file, int line)
{
    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    GLenum err;
    while ((err = glFuncs.glGetError()) != GL_NO_ERROR) {
        qCritical() << "OpenGL error " << err << " at " << QString::fromStdString(file)
                    << ":" << line << " for " << QString::fromStdString(stmt) << "\n";
    }
}

inline void printOpenGLInfo()
{
    QOpenGLFunctions glFuncs(QOpenGLContext::currentContext());
    auto gl_vendor   = QString(reinterpret_cast<char const *>(glFuncs.glGetString(GL_VENDOR)));
    auto gl_renderer = QString(reinterpret_cast<char const *>(glFuncs.glGetString(GL_RENDERER)));
    auto gl_version  = QString(reinterpret_cast<char const *>(glFuncs.glGetString(GL_VERSION)));
    auto gl_glsl_version =
        QString(reinterpret_cast<char const *>(glFuncs.glGetString(GL_SHADING_LANGUAGE_VERSION)));

    qInfo() << "OpenGL Context :\n"
            << "\tVendor : " << gl_vendor << "\n"
            << "\tRenderer : " << gl_renderer << "\n"
            << "\tVersion : " << gl_version << "\n"
            << "\tGLSL Version : " << gl_glsl_version << "\n";
}

enum AttributeLocation { Position = 0, Color, TextureCoord };