#include "pipeline.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QtWidgets>

#include <core/imagepipeline.h>
#include <operator/imageoperatorlist.h>
#include <gui/common/imageviewer.h>
#include "widget.h"
#include "operator.h"


PipelineWidget::PipelineWidget(QWidget *parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);

    QObject::connect(this, &QListWidget::itemClicked, this, &PipelineWidget::updateSelection);
}

void PipelineWidget::keyPressEvent(QKeyEvent *event)
{
    if (currentRow() >= 0) {
        switch (event->key()) {
            case Qt::Key_D:
                disableSelection(currentRow());
                break;
            case Qt::Key_Backspace:
                removeSelection(currentRow());
                break;
            default:
                QListWidget::keyPressEvent(event);
        }
    }
    else {
        QListWidget::keyPressEvent(event);
    }
}

void PipelineWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls() || e->mimeData()->hasText())
        e->acceptProposedAction();
    else
        QListWidget::dragEnterEvent(e);
}

void PipelineWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls())
            addFromFile(url.toLocalFile().toStdString());
    }
    else if (e->mimeData()->hasText()) {
        addFromName(e->mimeData()->text().toStdString());
    }
    else
        QListWidget::dropEvent(e);
}

void PipelineWidget::setDevWidget(DevWidget *w)
{
    m_devWidget = w;
}

void PipelineWidget::addFromFile(const std::string &path)
{
    if (ImageOperator *op = m_devWidget->operators()->CreateFromPath(path))
        addOperator(*op);
    else
        qDebug() << "Dropped file not supported" << QString::fromStdString(path) << "\n";
}

void PipelineWidget::addFromName(const std::string &name)
{
    if (ImageOperator *op = m_devWidget->operators()->CreateFromName(name))
        addOperator(*op);
    else
        qDebug() << "Dropped text not recognized" << QString::fromStdString(name) << "\n";
}

void PipelineWidget::addOperator(ImageOperator &op)
{
    QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(op.OpLabel()));
    item->setToolTip(QString::fromStdString(op.OpDesc()));
    addItem(item);

    op.Subscribe<ImageOperator::Update>([item, &op](){
        item->setText(QString::fromStdString(op.OpLabel()));
        item->setToolTip(QString::fromStdString(op.OpDesc()));
    });

    m_devWidget->pipeline()->AddOperator(&op);
    m_devWidget->operatorArea()->insertWidget(count() - 1, new OperatorWidget(&op));

    setCurrentRow(count() - 1);
    updateSelection(currentItem());
}

void PipelineWidget::updateSelection(QListWidgetItem *item)
{
    int selectedRow = row(item);

    if (selectedRow < 0 || selectedRow >= m_devWidget->pipeline()->OperatorCount())
        return;
    if (!m_devWidget->operatorArea())
        return;
    if (!m_devWidget->operatorArea()->widget(selectedRow))
        return;

    m_devWidget->operatorArea()->setCurrentIndex(selectedRow);
}

void PipelineWidget::disableSelection(int selectedRow)
{
    // NOTE : we should also track the operator state to choose new styles on enable /
    // disable From the CSS would be perfect, I think we have to subclass QListWidgetItem
    // and add new property that will be accessible from the CSS ?
    auto &op = m_devWidget->pipeline()->GetOperator(selectedRow);
    auto param  = op.GetParameter<CheckBoxParameter>("Enabled");
    param->setValue(!param->value());
}

void PipelineWidget::removeSelection(int selectedRow)
{
    if (selectedRow <= m_devWidget->pipeline()->OperatorCount()) {
        takeItem(selectedRow);
        QWidget *widget = m_devWidget->operatorArea()->widget(selectedRow);
        m_devWidget->operatorArea()->removeWidget(widget);
        delete widget;

        m_devWidget->pipeline()->DeleteOperator(selectedRow);
    }
}