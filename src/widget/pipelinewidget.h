#pragma once

#include <QtWidgets/QListWidget>


class QScrollArea;
class DevWidget;
class ImageOperator;

class PipelineWidget : public QListWidget
{
  public:
    PipelineWidget(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void setDevWidget(DevWidget *w);
    void setOperatorDetailWidget(QScrollArea *w);

  private:
    void buildFromPipeline();
    void initTransformationWidget(ImageOperator &op);

    void updateSelection(QListWidgetItem * item);
    void disableSelection(int selectedRow);
    void removeSelection(int selectedRow);

  private:
    DevWidget *m_devWidget;
    QScrollArea *m_operatorDetailWidget;

    uint16_t m_currentIndex;
};