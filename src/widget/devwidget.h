#pragma once

#include <QtWidgets/QWidget>


class QTabBar;
class ImagePipeline;
class ImageOperatorList;
class ImageWidget;
class PipelineWidget;
class WaveformWidget;
class OperatorListWidget;

class DevWidget : public QWidget
{
  public:
    DevWidget(ImagePipeline *pipeline, ImageOperatorList *list, QWidget *parent = nullptr);

  private:
    QWidget * setupUi();

    void initPipelineView();
    void initOperatorsView();
    void initScopeView();

  private:
    ImagePipeline *m_pipeline;
    ImageOperatorList *m_operators;

    ImageWidget *m_imageWidget;
    PipelineWidget *m_pipelineWidget;
    OperatorListWidget *m_operatorsWidget;

    QTabBar *m_scopeTab;
    WaveformWidget *m_scopeWidget;
};