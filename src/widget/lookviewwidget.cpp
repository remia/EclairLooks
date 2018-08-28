#include "lookviewwidget.h"
#include "imagewidget.h"
#include "../imagepipeline.h"
#include "../operator/imageoperatorlist.h"

#include <QtWidgets/QtWidgets>


LookViewWidget::LookViewWidget(QWidget *parent)
:   QWidget(parent)
{
    m_pipeline = std::make_unique<ImagePipeline>();

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    m_lookList = new QListWidget();
    vLayout->addWidget(m_lookList);

    QObject::connect(m_lookList, &QListWidget::itemSelectionChanged, this, &LookViewWidget::updateSelection);
}

void LookViewWidget::setPipeline(ImagePipeline *pipeline)
{
    m_globalPipeline = pipeline;
}

void LookViewWidget::setOperators(ImageOperatorList *list)
{
    m_operators = list;
}

void LookViewWidget::showPreview(const QString &path)
{
    // NOTE : When the new Image API will be ready
    // Time wasted because with compute full resolution image for thumbnails
    // Need to add resize capabilities to Image class
    m_pipeline->SetInput(m_globalPipeline->GetInput());

    m_lookList->clear();

    QDir dir = QFileInfo(path).dir();
    for (auto & entry : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
        QString entryPath = entry.absoluteFilePath();
        if (QWidget *w = widgetFromLook(entryPath); w) {
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(w->sizeHint());
            item->setData(Qt::UserRole, entryPath);

            m_lookList->addItem(item);
            m_lookList->setItemWidget(item, w);
        }
    }
}

void LookViewWidget::updateSelection()
{
    QListWidgetItem *item = m_lookList->currentItem();
    if (item) {
        QString path = item->data(Qt::UserRole).toString();
        EmitEvent<Select>(path);
    }
}

QWidget *LookViewWidget::widgetFromLook(const QString &path) const
{
    if (ImageOperator *op = m_operators->CreateFromPath(path.toStdString())) {
        m_pipeline->Reset();
        m_pipeline->AddOperator(op);
        Image img = m_pipeline->GetOutput().to_type(PixelType::Uint8);
        QImage qImg = QImage(img.pixels(), img.width(), img.height(), img.width() * img.channels() * 1, QImage::Format_RGB888);
        qImg = qImg.scaled(256, 256, Qt::KeepAspectRatio);

        QWidget *widget = new QWidget();

        QHBoxLayout *hLayout = new QHBoxLayout(widget);
        QLabel *thumbnail = new QLabel();
        thumbnail->setPixmap(QPixmap::fromImage(qImg));
        hLayout->addWidget(thumbnail);

        QVBoxLayout *vLayout = new QVBoxLayout();
        // Name, Lattice size, Date, LUT Format Name (ocio)
        vLayout->addWidget(new QLabel(path));
        vLayout->addWidget(new QLabel("Metadata 2"));
        vLayout->addWidget(new QLabel("Metadata 3"));
        hLayout->addLayout(vLayout);

        hLayout->setSizeConstraint(QLayout::SetFixedSize);

        return widget;
    }

    return nullptr;
}