#include "parameterseriallist.h"

#include <QtCore/QSettings>
#include <QtCore/QDebug>


ParameterSerialList::ParameterSerialList()
{
    m_settings = std::make_unique<QSettings>();
    qDebug() << "Load application settings from" << m_settings->fileName();
}

ParameterSerialList::~ParameterSerialList()
{
    SaveParameters();
}

const ParameterList & ParameterSerialList::Parameters() const
{
    return m_paramList;
}

void ParameterSerialList::LoadParameter(Parameter* p)
{
    QString n = QString::fromStdString(p->name());
    if ( m_settings->contains(n) and m_settings->value(n) != QVariant() )
        p->load(m_settings.get());
}

void ParameterSerialList::LoadParameters()
{
    for (auto &p : m_paramList)
        LoadParameter(p.get());
}

void ParameterSerialList::SaveParameters()
{
    for (auto &p : m_paramList)
        p->save(m_settings.get());
}
