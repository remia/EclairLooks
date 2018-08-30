#include "lookbrowserwidget.h"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QFileSystemModel>


class LookBrowserModel : public QFileSystemModel
{
  public:
    LookBrowserModel(QObject *parent = nullptr) : QFileSystemModel(parent)
    {
        setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    }

  public:
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return 1;
    }
};

LookBrowserWidget::LookBrowserWidget(QWidget *parent)
:   QTreeView(parent)
{
    m_fileSystemModel = new LookBrowserModel(this);
    m_sortFilterModel = new QSortFilterProxyModel(this);

    m_sortFilterModel->setFilterRole(QFileSystemModel::FilePathRole);
    m_sortFilterModel->setRecursiveFilteringEnabled(true);
    m_sortFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_sortFilterModel->setSourceModel(m_fileSystemModel);

    setModel(m_sortFilterModel);
    setSelectionMode(QAbstractItemView::SingleSelection);
    header()->setVisible(false);

    QObject::connect(this, &LookBrowserWidget::doubleClicked, this, &LookBrowserWidget::updateSelection);
}

void LookBrowserWidget::setBrowserRootPath(const QString & path)
{
    m_rootPath = path;
    updateRootPath(m_rootPath);
}

void LookBrowserWidget::filterList(const QString &filter)
{
    expandAll();
    m_sortFilterModel->setFilterFixedString(filter);

    if (filter.isEmpty() || !rootIndex().isValid()) {
        updateRootPath(m_rootPath);
        collapseAll();
    }
}

void LookBrowserWidget::updateSelection(const QModelIndex &index)
{
    QString path = index.data(QFileSystemModel::FilePathRole).toString();
    EmitEvent<Select>(path);
}

void LookBrowserWidget::updateRootPath(const QString &path)
{
    QModelIndex idx = m_sortFilterModel->mapFromSource( m_fileSystemModel->setRootPath(path) );
    setRootIndex(idx);
}