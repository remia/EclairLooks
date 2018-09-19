#pragma once

#include <QtWidgets/QWidget>


class LookWidget;
class LookViewWidget;
class QToolButton;

class LookSelectionWidget : public QWidget
{
  public:
    LookSelectionWidget(QWidget *parent = nullptr);

  public:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

  public:
    void setLookWidget(LookWidget *lw);

    LookViewWidget * viewWidget();

    void clearSelection();
    void saveSelection();
    void loadSelection();

  private:
    LookWidget *m_lookWidget;
    LookViewWidget *m_viewWidget;

    QToolButton *m_saveBtn;
    QToolButton *m_clearBtn;
    QToolButton *m_loadBtn;
};