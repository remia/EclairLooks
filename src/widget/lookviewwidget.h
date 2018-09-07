#pragma once

#include "../utils/generic.h"
#include "../utils/event_source.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>

#include <tuple>


class LookWidget;
class LookViewWidget;
class QListWidget;

typedef EventDesc <
    FuncT<void(const QString &)>,
    FuncT<void()>> LVEvtDesc;

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

class LookViewWidget : public QWidget
{
  public:
    LookViewWidget(QWidget *parent = nullptr);

  public:
    QListWidget *lookListWidget();
    void setLookWidget(LookWidget *lw);
    void setLookViewTabWidget(LookViewTabWidget *w);

    uint16_t countLook() const;

    void showFolder(const QString &path);
    void updateSelection();

  private:
    QWidget *widgetFromLook(const QString &path) const;

  private:
    LookWidget *m_lookWidget;
    LookViewTabWidget *m_lookViewTabWidget;
    QListWidget *m_lookList;
};