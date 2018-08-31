#include "settings.h"

#include <QtCore/QSettings>
#include <QtCore/QDebug>


Settings::Settings()
{
    m_settings = std::make_unique<QSettings>();

    m_paramList.Add(FilePathParameter("Default Image", "", "Choose an image", ""));
    m_paramList.Add(FilePathParameter("Look Base Folder", "", "Choose a folder", "", FilePathParameter::PathType::Folder));

    LoadParameters();
}

Settings::~Settings()
{
    SaveParameters();
}

const ParameterList & Settings::Parameters() const
{
    return m_paramList;
}

bool Settings::SetParameter(const Parameter &p)
{
    if (m_paramList.Set(p)) {
        Save(p);
        return true;
    }

    return false;
}

void Settings::LoadParameters()
{
    for (auto &p : m_paramList)
        if (m_settings->contains(QString::fromStdString(p->name)))
            Load(*p);
}

void Settings::SaveParameters()
{
    for (auto &p : m_paramList)
        Save(*p);
}

void Settings::Load(Parameter &p)
{
    QString paramName = QString::fromStdString(p.name);
    qDebug() << "Loading setting : " << paramName << m_settings->value(paramName).toString();

    switch (p.type) {
        case Parameter::Type::Text: {
            TextParameter *tp = static_cast<TextParameter *>(&p);
            tp->value = m_settings->value(paramName).toString().toStdString();

        } break;
        case Parameter::Type::Select: {
            SelectParameter *sp = static_cast<SelectParameter *>(&p);
            sp->value = m_settings->value(paramName).toString().toStdString();
        } break;
        case Parameter::Type::FilePath: {
            FilePathParameter *fp = static_cast<FilePathParameter *>(&p);
            fp->value = m_settings->value(paramName).toString().toStdString();
        } break;
        case Parameter::Type::CheckBox: {
            CheckBoxParameter *cp = static_cast<CheckBoxParameter *>(&p);
            cp->value = m_settings->value(paramName).toBool();
        } break;
        case Parameter::Type::Slider: {
            SliderParameter *sp = static_cast<SliderParameter *>(&p);
            sp->value = m_settings->value(paramName).toFloat();
        } break;
        default:
            break;
    }
}

void Settings::Save(const Parameter &p)
{
    QString paramName = QString::fromStdString(p.name);

    switch (p.type) {
        case Parameter::Type::Text: {
            const TextParameter *tp = static_cast<const TextParameter *>(&p);
            m_settings->setValue(paramName, QString::fromStdString(tp->value));
        } break;
        case Parameter::Type::Select: {
            const SelectParameter *sp = static_cast<const SelectParameter *>(&p);
            m_settings->setValue(paramName, QString::fromStdString(sp->value));
        } break;
        case Parameter::Type::FilePath: {
            const FilePathParameter *fp = static_cast<const FilePathParameter *>(&p);
            m_settings->setValue(paramName, QString::fromStdString(fp->value));
        } break;
        case Parameter::Type::CheckBox: {
            const CheckBoxParameter *cp = static_cast<const CheckBoxParameter *>(&p);
            m_settings->setValue(paramName, cp->value);
        } break;
        case Parameter::Type::Slider: {
            const SliderParameter *sp = static_cast<const SliderParameter *>(&p);
            m_settings->setValue(paramName, sp->value);
        } break;
        default:
            break;
    }
}
