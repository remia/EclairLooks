#pragma once

#include "../utils/generic.h"
#include <QtWidgets/QWidget>
#include <QtCore/QByteArray>


class Image;
class ImagePipeline;
class MainWindow;
class LookBrowserWidget;
class LookViewTabWidget;
class LookDetailWidget;
class QLineEdit;
class QSplitter;

class LookWidget : public QWidget
{
  public:
    LookWidget(MainWindow *mw, QWidget *parent = nullptr);

  public:
    bool eventFilter(QObject *obj, QEvent *event) override;

  public:
    LookViewTabWidget * lookViewTabWidget();

    void toggleFullScreen();

    QString rootPath();
    QString tonemapPath();

    Image & fullImage();
    Image & proxyImage();

    TupleT<bool, Image &> lookPreview(const QString &lookPath);
    TupleT<bool, Image &> lookPreviewProxy(const QString &lookPath);
    TupleT<bool, Image &> lookPreviewRamp(const QString &lookPath);

  private:
    void setupPipeline();
    QWidget* setupUi();

    TupleT<bool, Image &> _lookPreview(const QString &lookPath, Image &img);

  private:
    MainWindow *m_mainWindow;

    LookBrowserWidget *m_browserWidget;
    QLineEdit *m_browserSearch;
    LookViewTabWidget *m_viewTabWidget;
    LookDetailWidget *m_detailWidget;

    bool m_isFullScreen;
    QSplitter *m_hSplitter;
    QSplitter *m_vSplitter;
    QByteArray m_hSplitterState;
    QByteArray m_vSplitterState;

    UPtr<Image> m_image;
    UPtr<Image> m_imageProxy;
    UPtr<Image> m_imageRamp;
    UPtr<ImagePipeline> m_pipeline;
    QSize m_proxySize;
};