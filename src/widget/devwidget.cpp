#include "devwidget.h"
#include "uiloader.h"
#include "pipelinewidget.h"
#include "imagewidget.h"
#include "operatorlistwidget.h"
#include "../mainwindow.h"
#include "../operator/imageoperatorlist.h"
#include "../scope/waveformwidget.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QFile>


DevWidget::DevWidget(MainWindow *mw, QWidget *parent)
    : QWidget(parent), m_mainWindow(mw)
{
    //
    // Setup
    //

    QWidget * w = setupUi();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(w);
    setLayout(layout);

    m_imageWidget = findChild<ImageWidget*>("imageWidget");
    m_pipelineWidget = findChild<PipelineWidget*>("pipelineWidget");
    m_operatorsWidget = findChild<OperatorListWidget*>("operatorListWidget");
    m_scopeTab = findChild<QTabBar*>("scopeBar");
    m_scopeWidget = findChild<WaveformWidget*>("scopeWidget");

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    QSplitter *vSplitter = findChild<QSplitter*>("vSplitter");
    vSplitter->setSizes(QList<int>({75000, 25000}));
    QSplitter *hSplitterTop = findChild<QSplitter*>("hSplitterTop");
    hSplitterTop->setSizes(QList<int>({15000, 15000, 70000}));
    QSplitter *hSplitterBottom = findChild<QSplitter*>("hSplitterBottom");
    hSplitterBottom->setSizes(QList<int>({15000, 45000, 40000}));

    initPipelineView();
    initScopeView();
    initOperatorsView();

    //
    // Connections
    //

    using std::placeholders::_1;
    using IP = ImagePipeline;
    using IW = ImageWidget;

    m_mainWindow->pipeline()->Subscribe<IP::NewInput>(std::bind(&ImageWidget::setImage, m_imageWidget, _1));
    m_mainWindow->pipeline()->Subscribe<IP::Update>(std::bind(&ImageWidget::updateImage, m_imageWidget, _1));

    m_mainWindow->pipeline()->Subscribe<IP::NewInput>(std::bind(&WaveformWidget::resetTexture, m_scopeWidget, _1));
    m_imageWidget->Subscribe<IW::Update>(std::bind(&WaveformWidget::updateTexture, m_scopeWidget, _1));

    m_imageWidget->Subscribe<IW::DropImage>(std::bind(&ImagePipeline::SetInput, m_mainWindow->pipeline(), _1));
}

QWidget * DevWidget::setupUi()
{
    UiLoader loader;
    QFile file(":/ui/devwidget.ui");
    file.open(QFile::ReadOnly);
    return loader.load(&file, this);
}

void DevWidget::initPipelineView()
{
    QScrollArea *operatorDetail = findChild<QScrollArea*>("operatorDetailWidget");

    m_pipelineWidget->setPipeline(m_mainWindow->pipeline());
    m_pipelineWidget->setOperators(m_mainWindow->operators());
    m_pipelineWidget->setOperatorDetailWidget(operatorDetail);
}

void DevWidget::initScopeView()
{
    // NOTE : ideally we should make no assomptions of what scope mode are
    // available and discover them from the ScopeWidget class directly.
    m_scopeTab->setExpanding(false);
    m_scopeTab->addTab("W");
    m_scopeTab->addTab("P");

    QObject::connect(
        m_scopeTab, &QTabBar::tabBarClicked,
        [&, this](int index) {
            QString tabText = this->m_scopeTab->tabText(index);

            if (tabText == "W")
                this->m_scopeWidget->setScopeType("Waveform");
            else if (tabText == "P")
                this->m_scopeWidget->setScopeType("Parade");
        }
    );
}

void DevWidget::initOperatorsView()
{
    m_operatorsWidget->setOperators(m_mainWindow->operators());
}