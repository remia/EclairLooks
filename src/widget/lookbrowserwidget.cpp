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

    setModel(m_fileSystemModel);
    setSelectionMode(QAbstractItemView::SingleSelection);
    header()->setVisible(false);

    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &LookBrowserWidget::updateSelection);
}

void LookBrowserWidget::setBrowserRootPath(const QString & path)
{
    setRootIndex(m_fileSystemModel->setRootPath(path));
}

void LookBrowserWidget::filterList(const QString &filter)
{
    if (filter.isEmpty()) {
        m_fileSystemModel->setNameFilterDisables(true);
    }
    else {
        QStringList filters;
        filters << filter;

        m_fileSystemModel->setNameFilters(filters);
        m_fileSystemModel->setNameFilterDisables(false);
    }
}

void LookBrowserWidget::updateSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList indexes = selected.indexes();

    if (indexes.size() > 0) {
        QModelIndex index = indexes[0];
        QString path = index.data(QFileSystemModel::FilePathRole).toString();
        EmitEvent<Select>(path);
    }
}