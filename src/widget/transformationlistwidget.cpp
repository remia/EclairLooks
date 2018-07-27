#include "transformationlistwidget.h"
#include "../operator/ociofiletransform_operator.h"
#include "../operator/ociocolorspace_operator.h"
#include "../operator/ctl_operator.h"
#include "../mainwindow.h"
#include "imagewidget.h"
#include "transformationwidget.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>
#include <QtWidgets/QtWidgets>


TransformationListWidget::TransformationListWidget(QWidget *parent)
    : QListWidget(parent), m_pipeline(nullptr), m_operatorDetailWidget(nullptr)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);

    QObject::connect(
        this, &QListWidget::currentRowChanged,
        this, &TransformationListWidget::updateSelection);
}

void TransformationListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
    else
        QListWidget::dragEnterEvent(e);
}

void TransformationListWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            QFileInfo fileInfo(fileName);
            ImageOperator * image_op = nullptr;

            if (fileInfo.isDir()) {
                qDebug() << "Dropped ctl folder :" << fileName << "\n";
                auto t = m_pipeline->AddTransformation<CTLTransform>();
                initTransformationWidget(*t);
                t->SetBaseFolder(fileName.toStdString());
                image_op = t;
            }
            else if (fileName.endsWith(".ocio")) {
                qDebug() << "Dropped config file :" << fileName << "\n";
                auto t = m_pipeline->AddTransformation<OCIOColorSpace>();
                initTransformationWidget(*t);
                t->SetConfig(fileName.toStdString());
                image_op = t;
            }
            else if (fileName.endsWith(".3dl") || fileName.endsWith(".cube")){
                qDebug() << "Dropped transform file :" << fileName << "\n";
                auto t = m_pipeline->AddTransformation<OCIOFileTransform>();
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

QSize TransformationListWidget::sizeHint() const
{
    return QSize(160, 480);
}

void TransformationListWidget::setPipeline(ImagePipeline *pipeline)
{
    m_pipeline = pipeline;
}

void TransformationListWidget::setOperatorDetailWidget(QScrollArea *w)
{
    m_operatorDetailWidget = w;
}

void TransformationListWidget::buildFromPipeline()
{

}

void TransformationListWidget::initTransformationWidget(ImageOperator &op)
{
    QListWidgetItem * item = new QListWidgetItem(QString::fromStdString(op.OpName()));
    addItem(item);
}

void TransformationListWidget::updateSelection(int selectedRow)
{
    if (!m_operatorDetailWidget)
        return;

    QWidget * widget = TransformationWidget::FromOperator(m_pipeline->GetOperator(selectedRow));
    m_operatorDetailWidget->setWidget(widget);
}