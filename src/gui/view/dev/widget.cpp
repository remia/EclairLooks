#include "widget.h"

#include <QtWidgets/QtWidgets>
#include <QFile>

#include <core/imagepipeline.h>
#include <operator/imageoperatorlist.h>
#include <parameter/parameterseriallist.h>
#include <gui/common/browser.h>
#include <gui/common/imageviewer.h>
#include <gui/mainwindow.h>
#include <gui/uiloader.h>
#include <gui/scope/waveform.h>
#include <gui/scope/neutral.h>
#include <gui/scope/cube.h>
#include "pipeline.h"
#include "operator.h"
#include "operatorlist.h"


DevWidget::DevWidget(MainWindow *mw, QWidget *parent)
    : QWidget(parent), m_mainWindow(mw)
{
    m_imageRamp = std::make_unique<Image>(Image::Ramp1D(4096));
    m_imageLattice = std::make_unique<Image>(Image::Lattice(17));
    m_imageCompute = std::make_unique<Image>();

    //
    // Setup
    //

    QWidget * w = setupUi();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(w);
    setLayout(layout);

    m_imageWidget = findChild<ImageWidget*>("imageWidget");
    m_pipelineWidget = findChild<PipelineWidget*>("pipelineWidget");
    m_operatorWidget = findChild<QStackedWidget*>("operatorDetailWidget");
    m_operatorsWidget = findChild<OperatorListWidget*>("operatorListWidget");
    m_scopeStack = findChild<QStackedWidget*>("scopeStack");
    m_scopeTab = findChild<QTabBar*>("scopeBar");
    m_lookBrowser = findChild<BrowserWidget*>("lookBrowserWidget");
    m_imageBrowser = findChild<BrowserWidget*>("imageBrowserWidget");

    m_waveformWidget = new WaveformWidget();
    m_neutralsWidget = new NeutralWidget();
    m_cubeWidget = new CubeWidget();

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    QSplitter *vSplitter = findChild<QSplitter*>("vSplitter");
    vSplitter->setSizes(QList<int>({65000, 35000}));
    QSplitter *hSplitterTop = findChild<QSplitter*>("hSplitterTop");
    hSplitterTop->setSizes(QList<int>({15000, 15000, 70000}));
    QSplitter *hSplitterBottom = findChild<QSplitter*>("hSplitterBottom");
    hSplitterBottom->setSizes(QList<int>({15000, 55000, 30000}));

    setupPipelineView();
    setupScopeView();
    setupOperatorsView();
    setupBrowser();

    //
    // Connections
    //

    using std::placeholders::_1;
    using IP = ImagePipeline;
    using IW = ImageWidget;
    using BW = BrowserWidget;
    using P = Parameter;

    m_mainWindow->pipeline()->Subscribe<IP::NewInput>(std::bind(&ImageWidget::resetImage, m_imageWidget, _1));
    m_mainWindow->pipeline()->Subscribe<IP::Update>(std::bind(&ImageWidget::updateImage, m_imageWidget, SideBySide::A, _1));
    m_mainWindow->pipeline()->Subscribe<IP::Update>(std::bind(&DevWidget::updateScope, this, _1));

    m_imageWidget->Subscribe<IW::DropImage>(std::bind(&ImagePipeline::SetInput, m_mainWindow->pipeline(), _1));

    auto lookRootPath = m_mainWindow->settings()->Get<FilePathParameter>("Look Base Folder");
    lookRootPath->Subscribe<P::UpdateValue>([this](auto &param){ m_lookBrowser->setRootPath(m_mainWindow->lookBasePath()); });
    auto imageRootPath = m_mainWindow->settings()->Get<FilePathParameter>("Image Base Folder");
    imageRootPath->Subscribe<P::UpdateValue>([this](auto &param){ m_imageBrowser->setRootPath(m_mainWindow->imageBasePath()); });

    m_lookBrowser->Subscribe<BW::Select>([this](const QString &path) {
        m_pipelineWidget->addFromFile(path.toStdString());
    });
    m_imageBrowser->Subscribe<BW::Select>([this](const QString &path) {
        m_mainWindow->pipeline()->SetInput(Image::FromFile(path.toStdString()));
    });
}

ImagePipeline *DevWidget::pipeline()
{
    return m_mainWindow->pipeline();
}

ImageOperatorList *DevWidget::operators()
{
    return m_mainWindow->operators();
}

QStackedWidget* DevWidget::operatorArea()
{
    return m_operatorWidget;
}

PipelineWidget *DevWidget::pipelineWidget()
{
    return m_pipelineWidget;
}

QWidget* DevWidget::setupUi()
{
    UiLoader loader;
    QFile file(":/ui/devwidget.ui");
    file.open(QFile::ReadOnly);
    return loader.load(&file, this);
}

void DevWidget::setupBrowser()
{
    m_lookBrowser->setRootPath(m_mainWindow->lookBasePath());
    m_lookBrowser->setExtFilter(m_mainWindow->supportedLookExtensions());

    QStringList imgExts;
    for (auto ext : Image::SupportedExtensions())
        imgExts << "*." + QString::fromStdString(ext);
    m_imageBrowser->setRootPath(m_mainWindow->imageBasePath());
    m_imageBrowser->setExtFilter(imgExts);
}

void DevWidget::setupPipelineView()
{
    m_pipelineWidget->setDevWidget(this);
}

void DevWidget::setupScopeView()
{
    m_scopeStack->addWidget(m_waveformWidget);
    m_scopeStack->addWidget(m_neutralsWidget);
    m_scopeStack->addWidget(m_cubeWidget);
    m_scopeStack->setCurrentWidget(m_waveformWidget);

    // NOTE : ideally we should make no assomptions of what scope mode are
    // available and discover them from the ScopeWidget class directly.
    m_scopeTab->setExpanding(false);
    m_scopeTab->addTab("W");
    m_scopeTab->addTab("P");
    m_scopeTab->addTab("N");
    m_scopeTab->addTab("C");

    QObject::connect(
        m_scopeTab, &QTabBar::tabBarClicked,
        [&, this](int index) {
            QString tabText = m_scopeTab->tabText(index);

            if (tabText == "W") {
                m_scopeStack->setCurrentWidget(m_waveformWidget);
                m_waveformWidget->setScopeType("Waveform");
            }
            else if (tabText == "P") {
                m_scopeStack->setCurrentWidget(m_waveformWidget);
                m_waveformWidget->setScopeType("Parade");
            }
            else if (tabText == "N") {
                m_scopeStack->setCurrentWidget(m_neutralsWidget);
            }
            else if (tabText == "C") {
                m_scopeStack->setCurrentWidget(m_cubeWidget);
            }

            // Need to manually update the scope because it's not updated
            // when not visible.
            updateScope(m_mainWindow->pipeline()->GetOutput());
        }
    );
}

void DevWidget::setupOperatorsView()
{
    m_operatorsWidget->setDevWidget(this);
}

void DevWidget::updateScope(const Image &img)
{
    if (m_scopeStack->currentWidget() == m_neutralsWidget) {
        qInfo() << "Compute Ramp (curve scope)";
        *m_imageCompute = *m_imageRamp;
        pipeline()->ComputeImage(*m_imageCompute);
        m_neutralsWidget->drawCurve(0, *m_imageCompute);
    }
    else if (m_scopeStack->currentWidget() == m_cubeWidget) {
        qInfo() << "Compute Lattice (cube scope)";
        *m_imageCompute = *m_imageLattice;
        pipeline()->ComputeImage(*m_imageCompute);
        m_cubeWidget->drawCube(*m_imageCompute);
    }
    else if (m_scopeStack->currentWidget() == m_waveformWidget) {
        m_waveformWidget->updateTexture(m_imageWidget->texture());
    }
}