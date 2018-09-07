#include "lookselectionwidget.h"
#include "lookviewwidget.h"

#include <QtWidgets/QtWidgets>
#include <QFile>


LookSelectionWidget::LookSelectionWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    m_viewWidget = new LookViewWidget();
    vLayout->addWidget(m_viewWidget);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);

    m_saveBtn = new QToolButton();
    m_saveBtn->setText("Save");
    m_saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hLayout->addWidget(m_saveBtn);

    m_clearBtn = new QToolButton();
    m_clearBtn->setText("Clear");
    m_clearBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hLayout->addWidget(m_clearBtn);

    m_loadBtn = new QToolButton();
    m_loadBtn->setText("Load ...");
    m_loadBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hLayout->addWidget(m_loadBtn);

    vLayout->addLayout(hLayout);
}