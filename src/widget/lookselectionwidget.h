#pragma once

#include <QtWidgets/QWidget>


class LookViewWidget;
class QToolButton;

class LookSelectionWidget : public QWidget
{
  public:
    LookSelectionWidget(QWidget *parent = nullptr);

  private:
    LookViewWidget *m_viewWidget;
    QToolButton *m_saveBtn;
    QToolButton *m_clearBtn;
    QToolButton *m_loadBtn;
};