#pragma once

#include <QtWidgets/QListWidget>


class TransformationListWidget : public QListWidget
{
  public:
    TransformationListWidget(QWidget *parent = nullptr);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};