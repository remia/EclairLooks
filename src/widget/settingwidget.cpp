#include "settingwidget.h"
#include "parameterwidget.h"
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
        QLabel * label = new QLabel(QString::fromStdString(p->name));
        ParameterWidget *paramWidget = WidgetFromParameter(p.get());
        paramWidget->Subscribe<PW::Update>(std::bind(&Settings::SetParameter, m_settings, _1));

        fLayout->addRow(label, paramWidget);
    }
}