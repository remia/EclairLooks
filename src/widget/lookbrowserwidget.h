#pragma once

#include "../utils/event_source.h"

#include <QtWidgets/QTreeView>


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
    void setBrowserRootPath(const QString &path);

    void filterList(const QString &filter);
    void updateSelection(const QModelIndex &index);
    void updateRootPath(const QString &path);

  private:
    QString m_rootPath;

    LookBrowserModel *m_fileSystemModel;
    QSortFilterProxyModel *m_sortFilterModel;
};