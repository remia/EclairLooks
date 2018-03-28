#include "transformationlistwidget.h"
#include "transform_ocio.h"
#include "mainwindow.h"
#include "imagewidget.h"

#include <QtCore/QDebug>
#include <QtCore/QMimeData>
#include <QtGui/QDragEnterEvent>


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
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            QString fileName = url.toLocalFile();
            qDebug() << "Dropped transform file:" << fileName << "\n";

            auto transform = std::make_unique<TransformOCIO>();
            transform->SetFileTransform(fileName.toStdString());
            if (transform->IsIdentity())
                return;

            // Direct Parent : DockWidget
            MainWindow * mw = (MainWindow *) parent()->parent();
            mw->pipeline().AddTransformation(std::move(transform));
            // Manual notify for now ...
            mw->viewer()->setImage(mw->pipeline().GetOutput());

            addItem(url.fileName());
            update();
        }
    } else
        QListWidget::dropEvent(e);
}