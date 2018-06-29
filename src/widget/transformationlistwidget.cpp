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
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    viewport()->setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);
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
    // Direct Parent : DockWidget
    MainWindow * mw = (MainWindow *) parent()->parent();

    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            QFileInfo fileInfo(fileName);
            ImageOperator * image_op = nullptr;

            if (fileInfo.isDir()) {
                qDebug() << "Dropped ctl folder :" << fileName << "\n";
                auto t = mw->pipeline().AddTransformation<CTLTransform>();
                initTransformationWidget(*t);
                t->SetBaseFolder(fileName.toStdString());
                image_op = t;
            }
            else if (fileName.endsWith(".ocio")) {
                qDebug() << "Dropped config file :" << fileName << "\n";
                auto t = mw->pipeline().AddTransformation<OCIOColorSpace>();
                initTransformationWidget(*t);
                t->SetConfig(fileName.toStdString());
                image_op = t;
            }
            else if (fileName.endsWith(".3dl") || fileName.endsWith(".cube")){
                qDebug() << "Dropped transform file :" << fileName << "\n";
                auto t = mw->pipeline().AddTransformation<OCIOFileTransform>();
                initTransformationWidget(*t);
                t->SetFileTransform(fileName.toStdString());
                image_op = t;
            }
            else {
                qDebug() << "Dropped file not supported" << fileName << "\n";
                return;
            }


            mw->pipeline().Compute();
            update();
        }
    } else
        QListWidget::dropEvent(e);
}

void TransformationListWidget::initTransformationWidget(ImageOperator &op)
{
    QListWidgetItem * item = new QListWidgetItem();
    QWidget * widget = TransformationWidget::FromOperator(op);
    item->setSizeHint(widget->sizeHint());

    addItem(item);
    setItemWidget(item, widget);
}