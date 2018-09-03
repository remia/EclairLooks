#pragma once

#include <QtWidgets/QWidget>


class MainWindow;
class ImagePipeline;
class ImageOperatorList;
class ImageWidget;
class PipelineWidget;
class WaveformWidget;
class OperatorListWidget;
class QTabBar;

class DevWidget : public QWidget
{
  public:
    DevWidget(MainWindow *mw, QWidget *parent = nullptr);

  public:
    ImagePipeline *pipeline();
    ImageOperatorList *operators();

  private:
    QWidget * setupUi();

    void initPipelineView();
    void initOperatorsView();
    void initScopeView();

  private:
    MainWindow *m_mainWindow;

    ImageWidget *m_imageWidget;
    PipelineWidget *m_pipelineWidget;
    OperatorListWidget *m_operatorsWidget;

    QTabBar *m_scopeTab;
    WaveformWidget *m_scopeWidget;
};