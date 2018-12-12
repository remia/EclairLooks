#include "operatorwidget.h"
#include "parameter/parameter_widget.h"
#include "../operator/imageoperator.h"

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
                if (p->name() == name) {
                    QLabel * label = new QLabel(QString::fromStdString(p->displayName()));
                    ParameterWidget *paramWidget = WidgetFromParameter(p.get());
                    paramWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

                    auto c = m_operator->Subscribe<IOP::UpdateParam>(std::bind(&PW::UpdateUi, paramWidget, _1));
                    QObject::connect(
                        paramWidget, &QWidget::destroyed,
                        [&, op = m_operator, c]() {
                            op->Unsubscribe<IOP::UpdateParam>(c);
                        }
                    );

                    formLayout->addRow(label, paramWidget);
                }

        addTab(tab, QString::fromStdString(cat));

        if (cat == m_operator->DefaultCategory())
            tabBar()->moveTab(count() - 1, 0);
    }
}
