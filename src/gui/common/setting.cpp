#include "setting.h"

#include <QtWidgets/QtWidgets>

#include <parameter/parameterseriallist.h>
#include <parameter/parameterwidget.h>


SettingWidget::SettingWidget(QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QFormLayout(this);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setLabelAlignment(Qt::AlignLeft);
    m_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
}

void SettingWidget::addParameters(ParameterSerialList &settings, const QString &header)
{
    if (not header.isEmpty())
        m_layout->addRow("<b>" + header + "</b>", new QWidget());

    for (auto & p : settings.Parameters()) {
        QLabel * label = new QLabel(QString::fromStdString(p->displayName()));
        ParameterWidget *paramWidget = p->createWidget(this);
        paramWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        m_layout->addRow(label, paramWidget);
    }
}
