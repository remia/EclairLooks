#include "parameter.h"
#include "widget.h"


ShortcutParameter::ShortcutParameter(const std::string &name, const std::string& display_name)
: Parameter(name, display_name)
{
}

ShortcutParameter::ShortcutParameter(const std::string &name, const std::string& display_name, const QKeySequence &seq)
: Parameter(name, display_name), m_value(seq)
{
}

QKeySequence ShortcutParameter::value() const { return m_value; }

void ShortcutParameter::setValue(const QKeySequence &v)
{
    qDebug() << v.toString();
    m_value = v;
    EmitEvent<UpdateValue>(*this);
}

ParameterWidget *ShortcutParameter::newWidget(QWidget * parent)
{
    return new ParameterShortcutWidget(this, parent);
}

void ShortcutParameter::load(const QSettings *setting)
{
    setValue(QKeySequence(setting->value(QString::fromStdString(name())).toString()));
}

void ShortcutParameter::save(QSettings *setting) const
{
    setting->setValue(QString::fromStdString(name()), value().toString());
}