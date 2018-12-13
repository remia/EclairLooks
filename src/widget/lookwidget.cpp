#include "lookwidget.h"
#include "uiloader.h"
#include "browserwidget.h"
#include "lookviewtabwidget.h"
#include "lookviewwidget.h"
#include "lookdetailwidget.h"
#include "lookselectionwidget.h"
#include "settingwidget.h"
#include "../settings.h"
#include "../image.h"
#include "../imagepipeline.h"
#include "../mainwindow.h"
#include "../operator/imageoperatorlist.h"
#include "../operator/ociofiletransform_operator.h"
#include "../utils/types.h"

#include <QtWidgets/QtWidgets>
#include <QFile>

using std::placeholders::_1;
using BW = BrowserWidget;
using LV = LookViewTabWidget;
using LD = LookDetailWidget;
using P = Parameter;


LookWidget::LookWidget(MainWindow *mw, QWidget *parent)
    : QWidget(parent), m_mainWindow(mw), m_isFullScreen(false), m_proxySize(125, 125)
{
    m_pipeline = std::make_unique<ImagePipeline>();
    m_pipeline->SetName("look");
    m_imageRamp = std::make_unique<Image>(Image::Ramp1D(4096));
    m_imageLattice = std::make_unique<Image>(Image::Lattice(17));

    //
    // Setup
    //

    QWidget * w = setupUi();
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(w);

    m_browserWidget = findChild<BrowserWidget*>("lookBrowserWidget");
    m_viewTabWidget = findChild<LookViewTabWidget*>("lookViewWidget");
    m_detailWidget = findChild<LookDetailWidget*>("lookDetailWidget");
    m_selectWidget = findChild<LookSelectionWidget*>("lookSelectionWidget");
    m_settingWidget = findChild<QWidget*>("lookSettingWidget");

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    m_vSplitter = findChild<QSplitter*>("hSplitter");
    m_vSplitter->setSizes(QList<int>({15000, 85000}));
    m_hSplitter = findChild<QSplitter*>("vSplitter");
    m_hSplitter->setSizes(QList<int>({60000, 40000}));
    m_hSplitterView = findChild<QSplitter*>("hSplitterView");
    m_hSplitterView->setSizes(QList<int>({80000, 20000}));

    setupPipeline();
    setupBrowser();
    setupSetting();

    m_viewTabWidget->setLookWidget(this);
    m_detailWidget->setLookWidget(this);
    m_selectWidget->setLookWidget(this);

    //
    // Connections
    //

    auto lookRootPath = m_mainWindow->settings()->Get<FilePathParameter>("Look Base Folder");
    lookRootPath->Subscribe<P::UpdateValue>([this](auto &param){ m_browserWidget->setRootPath(lookBasePath()); });
    auto lookTonemap = m_mainWindow->settings()->Get<FilePathParameter>("Look Tonemap LUT");
    lookTonemap->Subscribe<P::UpdateValue>(std::bind(&LookWidget::updateToneMap, this));

    m_browserWidget->Subscribe<BW::Select>(std::bind(&LV::showFolder, m_viewTabWidget, _1));

    m_viewTabWidget->Subscribe<LV::Reset>(std::bind(&LD::clearView, m_detailWidget, SideBySide::A));
    m_viewTabWidget->Subscribe<LV::Select>(std::bind(&LD::showDetail, m_detailWidget, _1, SideBySide::A));

    m_selectWidget->Subscribe<LV::Reset>(std::bind(&LD::clearView, m_detailWidget, SideBySide::B));
    m_selectWidget->Subscribe<LV::Select>(std::bind(&LD::showDetail, m_detailWidget, _1, SideBySide::B));
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
                if (keyEvent->modifiers() & Qt::ShiftModifier)
                    wantedTarget = m_selectWidget->viewWidget();

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

QStringList LookWidget::supportedLookExtensions()
{
    QStringList extensions = OCIOFileTransform().SupportedExtensions();
    QListIterator<QString> itr(extensions);
    while (itr.hasNext()) {
        QString current = itr.next();
        extensions << "*." + current;
    }

    return extensions;
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

QString LookWidget::lookBasePath()
{
    std::string val = m_mainWindow->settings()->Get<FilePathParameter>("Look Base Folder")->value();
    return QString::fromStdString(val);
}

QString LookWidget::tonemapPath()
{
    std::string val = m_mainWindow->settings()->Get<FilePathParameter>("Look Tonemap LUT")->value();
    return QString::fromStdString(val);
}

void LookWidget::setImage(const Image &img)
{
    if (!img)
        return;

    m_image = std::make_unique<Image>(img);
    m_imageProxy = std::make_unique<Image>(*m_image);
    *m_imageProxy = m_imageProxy->resize(m_proxySize.width(), m_proxySize.height());
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

TupleT<bool, Image &> LookWidget::lookPreviewLattice(const QString &lookPath)
{
    return _lookPreview(lookPath, *m_imageLattice);
}

TupleT<bool, Image &> LookWidget::_lookPreview(const QString &lookPath, Image &img)
{
    ImageOperator &op = m_pipeline->GetOperator(0);
    auto opPath = op.GetParameter<FilePathParameter>("LUT");
    std::string currentPath = opPath->value();
    std::string requestPath = lookPath.toStdString();

    if (currentPath != requestPath)
        opPath->setValue(requestPath);

    m_pipeline->SetInput(img);
    return { true, m_pipeline->GetOutput() };
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
    setImage(m_mainWindow->pipeline()->GetInput());

    m_pipeline->AddOperator<OCIOFileTransform>();
    m_pipeline->AddOperator<OCIOFileTransform>();

    updateToneMap();
}

void LookWidget::setupBrowser()
{
    m_browserWidget->setRootPath(lookBasePath());
    m_browserWidget->setExtFilter(supportedLookExtensions());
}

void LookWidget::setupSetting()
{
    m_settings = new Settings();

    CheckBoxParameter *p = m_settings->Add<CheckBoxParameter>("Tone Mapping", true);
    toggleToneMap(p->value());

    p->Subscribe<Parameter::UpdateValue>([this](const Parameter &p){
        const CheckBoxParameter & cb = static_cast<const CheckBoxParameter&>(p);
        toggleToneMap(cb.value());
    });

    QVBoxLayout *layout = new QVBoxLayout(m_settingWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new SettingWidget(m_settings));
}

void LookWidget::updateViews()
{
    m_detailWidget->updateView(SideBySide::A);
    m_detailWidget->updateView(SideBySide::B);
    m_viewTabWidget->updateViews();
}

void LookWidget::toggleToneMap(bool v)
{
    ImageOperator &op = m_pipeline->GetOperator(1);
    op.GetParameter<CheckBoxParameter>("Enabled")->setValue(v);

    updateViews();
}

void LookWidget::updateToneMap()
{
    if (tonemapPath().isEmpty())
        return;

    if (auto op = m_mainWindow->operators()->CreateFromPath(tonemapPath().toStdString()))
        m_pipeline->ReplaceOperator(op, 1);

    updateViews();
}