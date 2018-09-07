#pragma once

#include "../utils/generic.h"
#include "../utils/event_source.h"

#include <QtWidgets/QTabWidget>


typedef EventDesc <
    FuncT<void(const QString &)>,
    FuncT<void()>> LVEvtDesc;

class LookWidget;

class LookViewTabWidget : public QTabWidget, public EventSource<LVEvtDesc>
{
  public:
    enum Evt { Select = 0, Reset };

  public:
    LookViewTabWidget(QWidget *parent = nullptr);

  public:
    QWidget *currentView();
    void setLookWidget(LookWidget *lw);

    void showFolder(const QString &path);
    void updateSelection(const QString &path);

  private:
    void tabChanged(int index);
    void tabClosed(int index);
    TupleT<bool, uint16_t> tabExists(const QString &name);

  private:
    LookWidget *m_lookWidget;
};