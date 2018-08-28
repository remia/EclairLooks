#include "lookwidget.h"
#include "uiloader.h"
#include "../imagepipeline.h"
#include "lookbrowserwidget.h"
#include "lookviewwidget.h"
#include "lookdetailwidget.h"

#include <QtWidgets/QtWidgets>
#include <QFile>


LookWidget::LookWidget(ImagePipeline *pipeline, ImageOperatorList *list, QWidget *parent)
    : QWidget(parent), m_pipeline(pipeline), m_operators(list)
{
    //
    // Setup
    //

    QWidget * w = setupUi();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(w);
    setLayout(layout);

    m_browserWidget = findChild<LookBrowserWidget*>("lookBrowserWidget");
    m_viewWidget = findChild<LookViewWidget*>("lookViewWidget");
    m_detailWidget = findChild<LookDetailWidget*>("lookDetailWidget");
    m_browserSearch = findChild<QLineEdit*>("lookBrowserSearch");

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    QSplitter *vSplitter = findChild<QSplitter*>("hSplitter");
    vSplitter->setSizes(QList<int>({15000, 85000}));
    QSplitter *hSplitter = findChild<QSplitter*>("vSplitter");
    hSplitter->setSizes(QList<int>({75000, 25000}));

    initLookBrowser();
    initLookView();
    initLookDetail();

    //
    // Connections
    //

    QObject::connect(m_browserSearch, &QLineEdit::textChanged, m_browserWidget, &LookBrowserWidget::filterList);

    using std::placeholders::_1;
    using LB = LookBrowserWidget;
    using LV = LookViewWidget;

    m_browserWidget->Subscribe<LB::Select>(std::bind(&LookViewWidget::showPreview, m_viewWidget, _1));
    m_viewWidget->Subscribe<LV::Select>(std::bind(&LookDetailWidget::showDetail, m_detailWidget, _1));
}

QWidget * LookWidget::setupUi()
{
    UiLoader loader;
    QFile file(":/ui/lookwidget.ui");
    file.open(QFile::ReadOnly);
    return loader.load(&file, this);
}

void LookWidget::initLookBrowser()
{
    // TODO : temporary
    m_browserWidget->setBrowserRootPath("/Users/remi/Desktop/3_LUT");
}

void LookWidget::initLookView()
{
    m_viewWidget->setPipeline(m_pipeline);
    m_viewWidget->setOperators(m_operators);
}

void LookWidget::initLookDetail()
{
    m_detailWidget->setPipeline(m_pipeline);
    m_detailWidget->setOperators(m_operators);
}