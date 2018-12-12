#include "lookbrowserwidget.h"
#include "lookwidget.h"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QFileSystemModel>


class LookBrowserModel : public QFileSystemModel
{
  public:
    LookBrowserModel(QObject *parent = nullptr) : QFileSystemModel(parent)
    {
        setFilter(QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);
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

void LookBrowserWidget::setLookWidget(LookWidget *lookWidget)
{
    m_lookWidget = lookWidget;
    m_fileSystemModel->setNameFilters(m_lookWidget->GetSupportedExtensions());
    m_fileSystemModel->setNameFilterDisables(false);
    updateRootPath(m_lookWidget->rootPath());
}

void LookBrowserWidget::filterList(const QString &filter)
{
    expandAll();
    m_sortFilterModel->setFilterFixedString(filter);

    if (filter.isEmpty() || !rootIndex().isValid()) {
        updateRootPath(m_lookWidget->rootPath());
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