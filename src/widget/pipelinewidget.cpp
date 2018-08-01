#include "pipelinewidget.h"
#include "../imagepipeline.h"
#include "../operator/ociofiletransform_operator.h"
#include "../operator/ociocolorspace_operator.h"
#include "../operator/ctl_operator.h"
#include "imagewidget.h"
#include "operatorwidget.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QtWidgets>


PipelineWidget::PipelineWidget(QWidget *parent)
    : QListWidget(parent), m_pipeline(nullptr), m_operatorDetailWidget(nullptr)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);

    QObject::connect(
        this, &QListWidget::itemClicked,
        this, &PipelineWidget::updateSelection);
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
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
    else
        QListWidget::dragEnterEvent(e);
}

void PipelineWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            QFileInfo fileInfo(fileName);
            ImageOperator * image_op = nullptr;

            if (fileInfo.isDir()) {
                qDebug() << "Dropped ctl folder :" << fileName << "\n";
                auto t = m_pipeline->AddOperator<CTLTransform>();
                initTransformationWidget(*t);
                t->SetBaseFolder(fileName.toStdString());
                image_op = t;
            }
            else if (fileName.endsWith(".ocio")) {
                qDebug() << "Dropped config file :" << fileName << "\n";
                auto t = m_pipeline->AddOperator<OCIOColorSpace>();
                initTransformationWidget(*t);
                t->SetConfig(fileName.toStdString());
                image_op = t;
            }
            else if (fileName.endsWith(".3dl") || fileName.endsWith(".cube")){
                qDebug() << "Dropped transform file :" << fileName << "\n";
                auto t = m_pipeline->AddOperator<OCIOFileTransform>();
                initTransformationWidget(*t);
                t->SetFileTransform(fileName.toStdString());
                image_op = t;
            }
            else {
                qDebug() << "Dropped file not supported" << fileName << "\n";
                return;
            }

            update();
        }
    } else
        QListWidget::dropEvent(e);
}

QSize PipelineWidget::sizeHint() const
{
    return QSize(160, 480);
}

void PipelineWidget::setPipeline(ImagePipeline *pipeline)
{
    m_pipeline = pipeline;
}

void PipelineWidget::setOperatorDetailWidget(QScrollArea *w)
{
    m_operatorDetailWidget = w;
}

void PipelineWidget::buildFromPipeline()
{

}

void PipelineWidget::initTransformationWidget(ImageOperator &op)
{
    QListWidgetItem * item = new QListWidgetItem(QString::fromStdString(op.OpName()));
    addItem(item);
}

void PipelineWidget::updateSelection(QListWidgetItem * item)
{
    int selectedRow = row(item);

    if (selectedRow < 0 || selectedRow >= m_pipeline->OperatorCount())
        return;
    if (!m_operatorDetailWidget)
        return;

    QWidget * widget = OperatorWidget::FromOperator(m_pipeline->GetOperator(selectedRow));
    m_operatorDetailWidget->setWidget(widget);
}

void PipelineWidget::disableSelection(int selectedRow)
{
    // NOTE : we should also track the operator state to choose new styles on enable / disable
    // From the CSS would be perfect, I think we have to subclass QListWidgetItem and add new property
    // that will be accessible from the CSS ?
    auto & op = m_pipeline->GetOperator(selectedRow);
    auto param = op.GetParameter<CheckBoxParameter>("Enabled");
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