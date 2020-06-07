#pragma once

#include <QtCore/QStringList>

#include <utils/generic.h>
#include <parameter/parameterseriallist.h>
#include <core/imagepipeline.h>
#include <operator/imageoperatorlist.h>


class Context
{
  public:
    static Context &getInstance();

  private:
    Context();

  public:
    Context(Context const &) = delete;
    void operator=(Context const &) = delete;

    ParameterSerialList& settings();
    ParameterSerialList& shortcuts();
    ImagePipeline& pipeline();
    ImageOperatorList& operators();

    QKeySequence shortcut(const QString& name);

    QStringList supportedLookExtensions();

  private:
    UPtr<ParameterSerialList> m_settings;
    UPtr<ParameterSerialList> m_shortcuts;
    UPtr<ImagePipeline> m_pipeline;
    UPtr<ImageOperatorList> m_operators;
};
