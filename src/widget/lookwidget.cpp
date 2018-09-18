#include "lookwidget.h"
#include "uiloader.h"
#include "lookbrowserwidget.h"
#include "lookviewtabwidget.h"
#include "lookdetailwidget.h"
#include "lookselectionwidget.h"
#include "../settings.h"
#include "../image.h"
#include "../imagepipeline.h"
#include "../mainwindow.h"
#include "../operator/imageoperatorlist.h"
#include "../operator/ociofiletransform_operator.h"

#include <QtWidgets/QtWidgets>
#include <QFile>

using std::placeholders::_1;
using LB = LookBrowserWidget;
using LV = LookViewTabWidget;


LookWidget::LookWidget(MainWindow *mw, QWidget *parent)
    : QWidget(parent), m_mainWindow(mw), m_isFullScreen(false), m_proxySize(125, 125)
{
    m_pipeline = std::make_unique<ImagePipeline>();
    m_imageRamp = std::make_unique<Image>(Image::Ramp1D(65535));

    //
    // Setup
    //

    QWidget * w = setupUi();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(w);
    setLayout(layout);

    m_browserWidget = findChild<LookBrowserWidget*>("lookBrowserWidget");
    m_viewTabWidget = findChild<LookViewTabWidget*>("lookViewWidget");
    m_detailWidget = findChild<LookDetailWidget*>("lookDetailWidget");
    m_selectWidget = findChild<LookSelectionWidget*>("lookSelectionWidget");
    m_browserSearch = findChild<QLineEdit*>("lookBrowserSearch");

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    m_vSplitter = findChild<QSplitter*>("hSplitter");
    m_vSplitter->setSizes(QList<int>({15000, 85000}));
    m_hSplitter = findChild<QSplitter*>("vSplitter");
    m_hSplitter->setSizes(QList<int>({65000, 35000}));
    m_hSplitterView = findChild<QSplitter*>("hSplitterView");
    m_hSplitterView->setSizes(QList<int>({80000, 20000}));

    setupPipeline();

    m_browserWidget->setLookWidget(this);
    m_viewTabWidget->setLookWidget(this);
    m_detailWidget->setLookWidget(this);
    m_selectWidget->setLookWidget(this);

    //
    // Connections
    //

    QObject::connect(m_browserSearch, &QLineEdit::textChanged, m_browserWidget, &LookBrowserWidget::filterList);

    m_browserWidget->Subscribe<LB::Select>(std::bind(&LookViewTabWidget::showFolder, m_viewTabWidget, _1));
    m_viewTabWidget->Subscribe<LV::Reset>(std::bind(&LookDetailWidget::resetView, m_detailWidget));
    m_viewTabWidget->Subscribe<LV::Select>(std::bind(&LookDetailWidget::showDetail, m_detailWidget, _1));
}

bool LookWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key()) {
            case Qt::Key_Space: {
                toggleFullScreen();
                return true;
            }
            case Qt::Key_Up:
            case Qt::Key_Down: {
                QObject *wantedTarget = m_viewTabWidget->currentView();
                if (wantedTarget && obj != wantedTarget) {
                    QCoreApplication::sendEvent(wantedTarget, event);
                    return true;
                }
                return false;
            }
            default:
                return false;
        }
    } else {
        return QWidget::eventFilter(obj, event);
    }
}

LookViewTabWidget * LookWidget::lookViewTabWidget()
{
    return m_viewTabWidget;
}

void LookWidget::toggleFullScreen()
{
    if (!m_isFullScreen) {
        m_hSplitterState = m_hSplitter->saveState();
        m_vSplitterState = m_vSplitter->saveState();

        m_vSplitter->setSizes(QList<int>({00000, 100000}));
        m_hSplitter->setSizes(QList<int>({00000, 100000}));

        m_isFullScreen = true;
    }
    else {
        m_hSplitter->restoreState(m_hSplitterState);
        m_vSplitter->restoreState(m_vSplitterState);
        m_isFullScreen = false;
    }
}

QString LookWidget::rootPath()
{
    std::string val = m_mainWindow->settings()->GetParameter<FilePathParameter>("Look Base Folder").value;
    return QString::fromStdString(val);
}

QString LookWidget::tonemapPath()
{
    std::string val = m_mainWindow->settings()->GetParameter<FilePathParameter>("Look Tonemap LUT").value;
    return QString::fromStdString(val);
}

Image & LookWidget::fullImage()
{
    return *m_image;
}

Image & LookWidget::proxyImage()
{
    return *m_imageProxy;
}

TupleT<bool, Image &>LookWidget::lookPreview(const QString &lookPath)
{
    return _lookPreview(lookPath, fullImage());
}

TupleT<bool, Image &> LookWidget::lookPreviewProxy(const QString &lookPath)
{
    return _lookPreview(lookPath, proxyImage());
}

TupleT<bool, Image &> LookWidget::lookPreviewRamp(const QString &lookPath)
{
    return _lookPreview(lookPath, *m_imageRamp);
}

TupleT<bool, Image &> LookWidget::_lookPreview(const QString &lookPath, Image &img)
{
    if (auto op = m_mainWindow->operators()->CreateFromPath(lookPath.toStdString()); op != nullptr) {
        m_pipeline->ReplaceOperator(op, 0);
        m_pipeline->SetInput(img);
        return { true, m_pipeline->GetOutput() };
    }

    return { false, img };
}

QWidget * LookWidget::setupUi()
{
    UiLoader loader;
    QFile file(":/ui/lookwidget.ui");
    file.open(QFile::ReadOnly);
    return loader.load(&file, this);
}

void LookWidget::setupPipeline()
{
    m_image = std::make_unique<Image>(m_mainWindow->pipeline()->GetInput());
    m_imageProxy = std::make_unique<Image>(*m_image);
    *m_imageProxy = m_imageProxy->resize(m_proxySize.width(), m_proxySize.height());

    m_pipeline->AddOperator<OCIOFileTransform>();

    if (!tonemapPath().isEmpty()) {
        if (auto op = m_mainWindow->operators()->CreateFromPath(tonemapPath().toStdString()); op != nullptr) {
            m_pipeline->AddOperator(op);
        }
    }
}