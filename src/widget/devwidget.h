#pragma once

#include <QtWidgets/QWidget>


class QTabBar;
class ImagePipeline;
class ImageWidget;
class TransformationListWidget;
class WaveformWidget;

class DevWidget : public QWidget
{
  public:
    DevWidget(ImagePipeline *pipeline, QWidget *parent = nullptr);

  private:
    QWidget * setupUi();

    void initPipelineView();
    void initScopeView();

  private:
    ImagePipeline *m_pipeline;

    ImageWidget *m_imageWidget;
    TransformationListWidget *m_transformationsWidget;
    QTabBar *m_scopeTab;
    WaveformWidget *m_scopeWidget;
};