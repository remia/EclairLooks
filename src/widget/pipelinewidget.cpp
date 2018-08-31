#include "pipelinewidget.h"
#include "../imagepipeline.h"
#include "../operator/imageoperatorlist.h"
#include "imagewidget.h"
#include "operatorwidget.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QtWidgets>


PipelineWidget::PipelineWidget(QWidget *parent)
    : QListWidget(parent), m_pipeline(nullptr), m_operators(nullptr),
      m_operatorDetailWidget(nullptr), m_currentIndex(0)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);

    QObject::connect(this, &QListWidget::itemClicked, this,
                     &PipelineWidget::updateSelection);
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
    if (!m_operators)
        return;

    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();

            if (ImageOperator *op = m_operators->CreateFromPath(fileName.toStdString())) {
                m_pipeline->AddOperator(op);
                initTransformationWidget(*op);
                update();
            }
            else {
                qDebug() << "Dropped file not supported" << fileName << "\n";
                return;
            }

        }
    }
    else if (e->mimeData()->hasText()) {
        QString text = e->mimeData()->text();
        if (ImageOperator *op = m_operators->CreateFromName(text.toStdString())) {
            m_pipeline->AddOperator(op);
            initTransformationWidget(*op);
            update();
        }
        else {
            qDebug() << "Dropped text not recognized" << text << "\n";
            return;
        }
    }
    else
        QListWidget::dropEvent(e);
}

void PipelineWidget::setPipeline(ImagePipeline *pipeline) { m_pipeline = pipeline; }

void PipelineWidget::setOperators(ImageOperatorList *list) { m_operators = list; }

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

    if (selectedRow < 0 || selectedRow >= m_pipeline->OperatorCount())
        return;
    if (!m_operatorDetailWidget)
        return;

    OperatorWidget *widget = new OperatorWidget(&m_pipeline->GetOperator(selectedRow));
    m_operatorDetailWidget->setWidget(widget);
}

void PipelineWidget::disableSelection(int selectedRow)
{
    // NOTE : we should also track the operator state to choose new styles on enable /
    // disable From the CSS would be perfect, I think we have to subclass QListWidgetItem
    // and add new property that will be accessible from the CSS ?
    auto &op    = m_pipeline->GetOperator(selectedRow);
    auto param  = op.GetParameter<CheckBoxParameter>("Enabled");
    param.value = !param.value;
    op.SetParameter(param);
}

void PipelineWidget::removeSelection(int selectedRow)
{
    if (m_pipeline->DeleteOperator(selectedRow)) {
        takeItem(selectedRow);
        m_operatorDetailWidget->takeWidget();
    }
}