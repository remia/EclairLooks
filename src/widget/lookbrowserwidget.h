#pragma once

#include "../utils/event_source.h"

#include <QtWidgets/QTreeView>


class QFileSystemModel;

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
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    QFileSystemModel *m_fileSystemModel;
};