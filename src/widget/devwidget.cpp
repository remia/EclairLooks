#include "devwidget.h"
#include "uiloader.h"
#include "pipelinewidget.h"
#include "imagewidget.h"
#include "../scope/waveformwidget.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QFile>


DevWidget::DevWidget(ImagePipeline *pipeline, QWidget *parent)
    : QWidget(parent), m_pipeline(pipeline)
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
    m_scopeTab = findChild<QTabBar*>("scopeBar");
    m_scopeWidget = findChild<WaveformWidget*>("scopeWidget");

    initPipelineView();
    initScopeView();

    //
    // Connections
    //

    using std::placeholders::_1;
    using IP = ImagePipeline;
    using IW = ImageWidget;

    m_pipeline->Subscribe<IP::Reset>(std::bind(&ImageWidget::setImage, m_imageWidget, _1));
    m_pipeline->Subscribe<IP::Update>(std::bind(&ImageWidget::updateImage, m_imageWidget, _1));

    m_pipeline->Subscribe<IP::Reset>(std::bind(&WaveformWidget::resetTexture, m_scopeWidget, _1));
    m_imageWidget->Subscribe<IW::Update>(std::bind(&WaveformWidget::updateTexture, m_scopeWidget, _1));

    m_imageWidget->Subscribe<IW::DropImage>(std::bind(&ImagePipeline::SetInput, m_pipeline, _1));
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

    m_pipelineWidget->setPipeline(m_pipeline);
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