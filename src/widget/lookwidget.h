#pragma once

#include <QtWidgets/QWidget>


class ImagePipeline;
class ImageOperatorList;
class LookBrowserWidget;
class LookViewTabWidget;
class LookDetailWidget;
class QLineEdit;

class LookWidget : public QWidget
{
  public:
    LookWidget(ImagePipeline *pipeline, ImageOperatorList *list, QWidget *parent = nullptr);

  public:
    void setLookPath(const std::string &path);

  private:
    QWidget* setupUi();

    void initLookBrowser();
    void initLookView();
    void initLookDetail();

  private:
    QString m_lookPath;
    ImagePipeline *m_pipeline;
    ImageOperatorList *m_operators;

    LookBrowserWidget *m_browserWidget;
    QLineEdit *m_browserSearch;
    LookViewTabWidget *m_viewWidget;
    LookDetailWidget *m_detailWidget;
};