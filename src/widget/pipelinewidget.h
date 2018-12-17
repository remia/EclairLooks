#pragma once

#include <QtWidgets/QListWidget>


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

  private:
    void initTransformationWidget(ImageOperator &op);

    void updateSelection(QListWidgetItem * item);
    void disableSelection(int selectedRow);
    void removeSelection(int selectedRow);

  private:
    DevWidget *m_devWidget;
};