#pragma once

#include <QtWidgets/QWidget>

#include "../utils/generic.h"


class MainWindow;
class ImagePipeline;
class Image;
class ImageOperatorList;
class ImageWidget;
class PipelineWidget;
class WaveformWidget;
class NeutralWidget;
class CubeWidget;
class OperatorListWidget;
class QTabBar;
class QStackedWidget;

class DevWidget : public QWidget
{
  public:
    DevWidget(MainWindow *mw, QWidget *parent = nullptr);

  public:
    ImagePipeline *pipeline();
    ImageOperatorList *operators();

    QStackedWidget *operatorArea();

  private:
    QWidget * setupUi();

    void initPipelineView();
    void initOperatorsView();
    void initScopeView();

    void updateScope(const Image &img);

  private:
    MainWindow *m_mainWindow;

    ImageWidget *m_imageWidget;
    PipelineWidget *m_pipelineWidget;
    QStackedWidget *m_operatorWidget;
    OperatorListWidget *m_operatorsWidget;

    QStackedWidget *m_scopeStack;
    QTabBar *m_scopeTab;
    WaveformWidget *m_waveformWidget;
    NeutralWidget *m_neutralsWidget;
    CubeWidget *m_cubeWidget;

    UPtr<Image> m_imageRamp;
    UPtr<Image> m_imageLattice;
    UPtr<Image> m_imageCompute;
};