#pragma once

#include "../utils/event_source.h"

#include <QtWidgets/QTreeView>


class LookWidget;
class LookBrowserModel;
class QSortFilterProxyModel;

typedef EventDesc <
    FuncT<void(const QString &)>> LBEvtDesc;

class LookBrowserWidget : public QTreeView, public EventSource<LBEvtDesc>
{
  public:
    enum Evt { Select = 0 };

  public:
    LookBrowserWidget(QWidget *parent = nullptr);

  public:
    void setLookWidget(LookWidget *lookWidget);

    void filterList(const QString &filter);
    void updateSelection(const QModelIndex &index);
    void updateRootPath(const QString &path);

  private:
    LookWidget *m_lookWidget;
    LookBrowserModel *m_fileSystemModel;
    QSortFilterProxyModel *m_sortFilterModel;
};