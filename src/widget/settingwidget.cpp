#include "settingwidget.h"
#include "parameter/parameter_widget.h"
#include "../settings.h"

#include <QtWidgets/QtWidgets>

using std::placeholders::_1;
using PW = ParameterWidget;


SettingWidget::SettingWidget(Settings *settings, QWidget *parent)
:   QWidget(parent), m_settings(settings)
{
    QFormLayout *fLayout = new QFormLayout(this);
    fLayout->setContentsMargins(4, 4, 4, 4);
    fLayout->setLabelAlignment(Qt::AlignLeft);
    fLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    fLayout->addRow("<b>General</b>", new QWidget());
    for (auto & p : m_settings->Parameters()) {
        QLabel * label = new QLabel(QString::fromStdString(p->displayName()));
        ParameterWidget *paramWidget = WidgetFromParameter(p.get());
        fLayout->addRow(label, paramWidget);
    }
}