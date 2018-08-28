#pragma once

#include <QtWidgets/QWidget>


class ImagePipeline;
class ImageOperatorList;
class LookBrowserWidget;
class LookViewWidget;
class LookDetailWidget;
class QLineEdit;

class LookWidget : public QWidget
{
  public:
    LookWidget(ImagePipeline *pipeline, ImageOperatorList *list, QWidget *parent = nullptr);

  public:
    void filterLooks(const QString &filter);

  private:
    QWidget * setupUi();

    void initLookBrowser();
    void initLookView();
    void initLookDetail();

  private:
    ImagePipeline *m_pipeline;
    ImageOperatorList *m_operators;

    LookBrowserWidget *m_browserWidget;
    QLineEdit *m_browserSearch;
    LookViewWidget *m_viewWidget;
    LookDetailWidget *m_detailWidget;
};