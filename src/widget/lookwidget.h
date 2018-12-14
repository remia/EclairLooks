#pragma once

#include "../utils/generic.h"
#include <QtWidgets/QWidget>
#include <QtCore/QByteArray>


class Settings;
class Image;
class ImagePipeline;
class MainWindow;
class BrowserWidget;
class LookViewTabWidget;
class LookDetailWidget;
class LookSelectionWidget;
class QSplitter;

class LookWidget : public QWidget
{
  public:
    LookWidget(MainWindow *mw, QWidget *parent = nullptr);

  public:
    bool eventFilter(QObject *obj, QEvent *event) override;

  public:
    LookViewTabWidget * lookViewTabWidget();

    QStringList supportedLookExtensions();

    void toggleFullScreen();

    QString lookBasePath() const;
    QString imageBasePath() const;
    QString tonemapPath() const;
    bool tonemapEnabled() const;

    void setImage(const Image &img);
    Image & fullImage();
    Image & proxyImage();

    TupleT<bool, Image &> lookPreview(const QString &lookPath);
    TupleT<bool, Image &> lookPreviewProxy(const QString &lookPath);
    TupleT<bool, Image &> lookPreviewRamp(const QString &lookPath);
    TupleT<bool, Image &> lookPreviewLattice(const QString &lookPath);

  private:
    void setupPipeline();
    void setupBrowser();
    void setupSetting();
    QWidget* setupUi();

    TupleT<bool, Image &> _lookPreview(const QString &lookPath, Image &img);

    void updateViews();
    void toggleToneMap(bool v);
    void updateToneMap();

  private:
    MainWindow *m_mainWindow;

    Settings *m_settings = nullptr;

    BrowserWidget *m_browserWidget;
    LookViewTabWidget *m_viewTabWidget;
    LookDetailWidget *m_detailWidget;
    LookSelectionWidget *m_selectWidget;
    QWidget *m_settingWidget;

    bool m_isFullScreen;
    QSplitter *m_hSplitter;
    QSplitter *m_vSplitter;
    QSplitter *m_hSplitterView;
    QByteArray m_hSplitterState;
    QByteArray m_vSplitterState;

    UPtr<Image> m_image;
    UPtr<Image> m_imageProxy;
    UPtr<Image> m_imageRamp;
    UPtr<Image> m_imageLattice;
    UPtr<ImagePipeline> m_pipeline;
    QSize m_proxySize;
};