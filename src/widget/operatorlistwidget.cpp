#include "operatorlistwidget.h"
#include "devwidget.h"
#include "../operator/imageoperatorlist.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QtWidgets>


OperatorListWidget::OperatorListWidget(QWidget *parent)
    : QListWidget(parent), m_devWidget(nullptr)
{
    setSelectionMode(QAbstractItemView::NoSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
}

void OperatorListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem *child = itemAt(event->pos());
    if (!child)
        return;

    QMimeData *mimeData = new QMimeData();
    mimeData->setText(child->text());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void OperatorListWidget::setDevWidget(DevWidget *w)
{
    m_devWidget = w;

    clear();
    for (auto &name : w->operators()->Operators())
        addItem(new QListWidgetItem(QString::fromStdString(name)));
}
