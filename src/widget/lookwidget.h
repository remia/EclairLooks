#pragma once

#include "../utils/generic.h"
#include <QtWidgets/QWidget>


class Image;
class ImagePipeline;
class MainWindow;
class LookBrowserWidget;
class LookViewTabWidget;
class LookDetailWidget;
class QLineEdit;

class LookWidget : public QWidget
{
  public:
    LookWidget(MainWindow *mw, QWidget *parent = nullptr);

  public:
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
    LookViewTabWidget *m_viewWidget;
    LookDetailWidget *m_detailWidget;

    UPtr<Image> m_image;
    UPtr<Image> m_imageProxy;
    UPtr<Image> m_imageRamp;
    UPtr<ImagePipeline> m_pipeline;
    QSize m_proxySize;
};