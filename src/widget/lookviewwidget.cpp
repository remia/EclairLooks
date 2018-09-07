#include "lookviewwidget.h"
#include "lookviewtabwidget.h"
#include "lookwidget.h"
#include "imagewidget.h"
#include "../image.h"

#include <QtWidgets/QtWidgets>


LookViewWidget::LookViewWidget(QWidget *parent)
:   QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    m_lookList = new QListWidget();
    vLayout->addWidget(m_lookList);

    QObject::connect(m_lookList, &QListWidget::itemSelectionChanged, this, &LookViewWidget::updateSelection);
}

QListWidget *LookViewWidget::lookListWidget()
{
    return m_lookList;
}

void LookViewWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
    m_lookList->installEventFilter(m_lookWidget);
}

void LookViewWidget::setLookViewTabWidget(LookViewTabWidget *w)
{
    m_lookViewTabWidget = w;
}

uint16_t LookViewWidget::countLook() const
{
    return m_lookList->count();
}

void LookViewWidget::showFolder(const QString &path)
{
    m_lookList->clear();

    QDir dir(path);
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

    m_lookList->setCurrentRow(0);
}

void LookViewWidget::updateSelection()
{
    QListWidgetItem *item = m_lookList->currentItem();
    if (item) {
        QString path = item->data(Qt::UserRole).toString();
        m_lookViewTabWidget->updateSelection(path);
    }
}

QWidget *LookViewWidget::widgetFromLook(const QString &path) const
{
    if (auto [valid, img] = m_lookWidget->lookPreviewProxy(path); valid) {
        img = img.to_type(PixelType::Uint8);
        QImage qImg = QImage(
            img.pixels(), img.width(), img.height(),
            img.width() * img.channels() * 1,
            QImage::Format_RGB888);

        QWidget *widget = new QWidget();

        QHBoxLayout *hLayout = new QHBoxLayout(widget);
        QLabel *thumbnail = new QLabel();
        thumbnail->setPixmap(QPixmap::fromImage(qImg));
        hLayout->addWidget(thumbnail);

        QFileInfo info(path);

        QVBoxLayout *vLayout = new QVBoxLayout();
        vLayout->addWidget(new QLabel(info.fileName()));
        vLayout->addWidget(new QLabel(QString("Last modified : %1").arg(info.lastModified().toString())));
        vLayout->addStretch(1);
        hLayout->addLayout(vLayout);

        hLayout->setSizeConstraint(QLayout::SetFixedSize);

        return widget;
    }

    return nullptr;
}