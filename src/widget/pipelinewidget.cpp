#include "pipelinewidget.h"
#include "imagewidget.h"
#include "operatorwidget.h"
#include "devwidget.h"
#include "../imagepipeline.h"
#include "../operator/imageoperatorlist.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QtWidgets>


PipelineWidget::PipelineWidget(QWidget *parent)
    : QListWidget(parent), m_devWidget(nullptr), m_currentIndex(0)
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
    } else {
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
    std::vector<ImageOperator*> operatorToAdd;

    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();

            if (ImageOperator *op = m_devWidget->operators()->CreateFromPath(fileName.toStdString()))
                operatorToAdd.push_back(op);
            else
                qDebug() << "Dropped file not supported" << fileName << "\n";
        }
    }
    else if (e->mimeData()->hasText()) {
        QString text = e->mimeData()->text();
        if (ImageOperator *op = m_devWidget->operators()->CreateFromName(text.toStdString()))
            operatorToAdd.push_back(op);
        else
            qDebug() << "Dropped text not recognized" << text << "\n";
    }
    else
        QListWidget::dropEvent(e);

    for (auto op : operatorToAdd) {
       m_devWidget->pipeline()->AddOperator(op);
       initTransformationWidget(*op);
       setCurrentRow(count() - 1);
       updateSelection(currentItem());
    }
}

void PipelineWidget::setDevWidget(DevWidget *w)
{
    m_devWidget = w;
}

void PipelineWidget::setOperatorDetailWidget(QScrollArea *w)
{
    m_operatorDetailWidget = w;
}

void PipelineWidget::buildFromPipeline() {}

void PipelineWidget::initTransformationWidget(ImageOperator &op)
{
    QString name = QString("%1 [%2]").arg(
        QString::fromStdString(op.OpName()), QString::number(++m_currentIndex));
    addItem(new QListWidgetItem(name));
}

void PipelineWidget::updateSelection(QListWidgetItem *item)
{
    int selectedRow = row(item);

    if (selectedRow < 0 || selectedRow >= m_devWidget->pipeline()->OperatorCount())
        return;
    if (!m_operatorDetailWidget)
        return;

    OperatorWidget *widget = new OperatorWidget(&m_devWidget->pipeline()->GetOperator(selectedRow));
    m_operatorDetailWidget->setWidget(widget);
}

void PipelineWidget::disableSelection(int selectedRow)
{
    // NOTE : we should also track the operator state to choose new styles on enable /
    // disable From the CSS would be perfect, I think we have to subclass QListWidgetItem
    // and add new property that will be accessible from the CSS ?
    auto &op    = m_devWidget->pipeline()->GetOperator(selectedRow);
    auto param  = op.GetParameter<CheckBoxParameter>("Enabled");
    param.value = !param.value;
    op.SetParameter(param);
}

void PipelineWidget::removeSelection(int selectedRow)
{
    if (m_devWidget->pipeline()->DeleteOperator(selectedRow)) {
        takeItem(selectedRow);
        m_operatorDetailWidget->takeWidget();
    }
}