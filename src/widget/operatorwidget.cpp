#include "operatorwidget.h"
#include "../operator/imageoperator.h"
#include "parameterwidget.h"

#include <QtWidgets/QtWidgets>

using std::placeholders::_1;
using PW = ParameterWidget;
using IOP = ImageOperator;


OperatorWidget::OperatorWidget(ImageOperator *op, QWidget *parent)
:   QTabWidget(parent), m_operator(op)
{
    setupUi();
}

void OperatorWidget::setupUi()
{
    for (auto &[cat, plist] : m_operator->Categories()) {
        QWidget *tab = new QWidget();
        QFormLayout *formLayout = new QFormLayout(tab);
        formLayout->setVerticalSpacing(2);
        formLayout->setHorizontalSpacing(2);
        formLayout->setContentsMargins(6, 6, 6, 6);
        formLayout->setLabelAlignment(Qt::AlignLeft);
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

        for (auto & name : plist)
            for (auto & p : m_operator->Parameters())
                if (p->name == name) {
                    QLabel * label = new QLabel(QString::fromStdString(name));
                    ParameterWidget *paramWidget = WidgetFromParameter(p.get());
                    paramWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

                    paramWidget->Subscribe<PW::Update>(std::bind(&IOP::SetParameter, m_operator, _1));
                    auto c = m_operator->Subscribe<IOP::UpdateGui>(std::bind(&PW::UpdateUi, paramWidget, _1));
                    QObject::connect(
                        paramWidget, &QWidget::destroyed,
                        [&, c]() {
                            m_operator->Unsubscribe<IOP::UpdateGui>(c);
                        }
                    );

                    formLayout->addRow(label, paramWidget);
                }

        addTab(tab, QString::fromStdString(cat));

        if (cat == m_operator->DefaultCategory())
            tabBar()->moveTab(count() - 1, 0);
    }
}
