#pragma once

#include <QtWidgets/QListWidget>


class DevWidget;

class OperatorListWidget : public QListWidget
{
  public:
    OperatorListWidget(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;

  public:
    void setDevWidget(DevWidget *w);

  private:
    DevWidget *m_devWidget;
};