#include "parameter_widget.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>


ParameterWidget::ParameterWidget(Parameter *param, QWidget *parent)
:   QWidget(parent), m_param(param), m_layout(nullptr)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
}

void ParameterWidget::UpdateUi(const Parameter &p)
{
    if (p.name() != m_param->name())
        return;

    UpdateWidget(p);
}

ParameterWidget* WidgetFromParameter(Parameter *p)
{
    switch (p->type()) {
        case Parameter::Type::Text:
            return new ParameterTextWidget(p);
            break;
        case Parameter::Type::Select:
            return new ParameterSelectWidget(p);
            break;
        case Parameter::Type::FilePath:
            return new ParameterFilePathWidget(p);
            break;
        case Parameter::Type::CheckBox:
            return new ParameterCheckBoxWidget(p);
            break;
        case Parameter::Type::Slider:
            return new ParameterSliderWidget(p);
            break;
        default:
            return new ParameterWidget(p);
    }
}
