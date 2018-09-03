#pragma once

#include "../utils/generic.h"
#include "../utils/event_source.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>

#include <tuple>


class LookWidget;
class QListWidget;

typedef EventDesc <FuncT<void(const QString &)>> LVEvtDesc;

class LookViewTabWidget : public QTabWidget, public EventSource<LVEvtDesc>
{
  public:
    enum Evt { Select = 0 };

  public:
    LookViewTabWidget(QWidget *parent = nullptr);

  public:
    void setLookWidget(LookWidget *lw);

    void showPreview(const QString &path);
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
    void setLookWidget(LookWidget *lw);
    void setLookViewTabWidget(LookViewTabWidget *w);

    uint16_t countLook() const;

    void showPreview(const QString &path);
    void updateSelection();

  private:
    QWidget *widgetFromLook(const QString &path) const;

  private:
    LookWidget *m_lookWidget;
    LookViewTabWidget *m_lookViewTabWidget;
    QListWidget *m_lookList;
};