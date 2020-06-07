#include "context.h"

#include <operator/ocio/filetransform.h>


Context& Context::getInstance()
{
    static Context instance;
    return instance;
}

Context::Context() :
    m_settings(new ParameterSerialList()),
    m_shortcuts(new ParameterSerialList()),
    m_pipeline(new ImagePipeline()),
    m_operators(new ImageOperatorList())
{
}

ParameterSerialList& Context::settings() { return *m_settings; }

ParameterSerialList& Context::shortcuts() { return *m_shortcuts; }

ImagePipeline& Context::pipeline() { return *m_pipeline; }

ImageOperatorList& Context::operators() { return *m_operators; }

QKeySequence Context::shortcut(const QString& name)
{
    if (auto p = m_shortcuts->Get<ShortcutParameter>(name.toStdString()))
        return p->value();

    return QKeySequence();
}

QStringList Context::supportedLookExtensions()
{
    QStringList res;
    for (QString &ext : OCIOFileTransform().SupportedExtensions())
        res << "*." + ext;

    return res;
}