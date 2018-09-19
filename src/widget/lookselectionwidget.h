#pragma once

#include "../utils/event_source.h"

#include <QtWidgets/QWidget>


class LookWidget;
class LookViewWidget;
class QToolButton;

typedef EventDesc <
    FuncT<void(const QString &)>,
    FuncT<void()>> LVEvtDesc;

class LookSelectionWidget : public QWidget, public EventSource<LVEvtDesc>
{
  public:
    enum Evt { Select = 0, Reset };

  public:
    LookSelectionWidget(QWidget *parent = nullptr);

  public:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

  public:
    void setLookWidget(LookWidget *lw);

    LookViewWidget * viewWidget();

    void updateSelection();
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