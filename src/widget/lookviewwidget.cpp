#include "lookviewwidget.h"
#include "lookwidget.h"
#include "imagewidget.h"
#include "../image.h"

#include <QtWidgets/QtWidgets>


// ----------------------------------------------------------------------------

LookViewWidget::LookViewWidget(QWidget *parent)
    : QListWidget(parent), m_lookWidget(nullptr),
      m_displayMode(DisplayMode::Normal), m_readOnly(true)
{
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void LookViewWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidgetItem * item = itemAt(event->pos());
    if (!item)
        setCurrentIndex(QModelIndex());

    QListWidget::mousePressEvent(event);
}

void LookViewWidget::keyPressEvent(QKeyEvent *event)
{
    if (currentRow() >= 0) {
        switch (event->key()) {
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

void LookViewWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem* item = currentItem();
    LookViewItemWidget * w = static_cast<LookViewItemWidget *>(itemWidget(item));

    QMimeData *mimeData = new QMimeData();
    mimeData->setUrls(QList<QUrl>() << QUrl::fromLocalFile(w->path()));

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(*w->image());
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}

void LookViewWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
    installEventFilter(m_lookWidget);
}

void LookViewWidget::setDisplayMode(DisplayMode m)
{
    m_displayMode = m;
}

void LookViewWidget::setReadOnly(bool ro)
{
    m_readOnly = ro;
}

uint16_t LookViewWidget::countLook() const
{
    return count();
}

QString LookViewWidget::currentLook() const
{
    QListWidgetItem * item = currentItem();
    if (item)
        return item->data(Qt::UserRole).toString();

    return "";
}

QStringList LookViewWidget::allLook() const
{
    QStringList res;
    for (uint16_t i = 0; i < count(); ++i) {
        res << item(i)->data(Qt::UserRole).toString();
    }

    return res;
}

int LookViewWidget::indexLook(const QString &path) const
{
    for (uint16_t i = 0; i < count(); ++i) {
        if (item(i)->data(Qt::UserRole).toString() == path)
            return i;
    }

    return -1;
}

void LookViewWidget::appendFolder(const QString &path)
{
    clear();

    QDir dir(path);
    dir.setNameFilters(m_lookWidget->supportedExtensions());
    for (auto & entry : dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
        QString entryPath = entry.absoluteFilePath();
        addLook(entryPath);
    }

    setCurrentRow(0);
}

void LookViewWidget::appendLook(const QString &path)
{
    addLook(path);
}

void LookViewWidget::removeSelection(int selectedRow)
{
    if (m_readOnly)
        return;

    takeItem(selectedRow);
}

void LookViewWidget::addLook(const QString &path)
{
    if (!m_lookWidget)
        return;

    if (allLook().contains(path)) {
        setCurrentRow(indexLook(path));
        return;
    }

    QListWidgetItem *item = new QListWidgetItem();

    if (m_displayMode != DisplayMode::Minimized) {
        LookViewItemWidget *widget = new LookViewItemWidget();
        widget->setPath(path);
        widget->setImage(QPixmap::fromImage(computeThumbnail(path)));
        widget->setup();

        item->setSizeHint(widget->sizeHint());
        item->setData(Qt::UserRole, path);
        addItem(item);
        setItemWidget(item, widget);
    }
    else {
        QFileInfo info(path);
        item->setText(info.fileName());
        item->setData(Qt::UserRole, path);
        addItem(item);
    }
}

QImage LookViewWidget::computeThumbnail(const QString &path)
{
    QImage res;
    if (auto [valid, img] = m_lookWidget->lookPreviewProxy(path); valid) {
        img = img.to_type(PixelType::Uint8);
        res = QImage(
            img.pixels(), img.width(), img.height(),
            img.width() * img.channels() * 1,
            QImage::Format_RGB888);
    }
    else {
        res = QImage(
            img.width(), img.height(),
            QImage::Format_RGB888);
        res.fill(Qt::red);
    }

    return res;
}

// ----------------------------------------------------------------------------

LookViewItemWidget::LookViewItemWidget(QWidget * parent)
: QWidget(parent)
{

}

QString LookViewItemWidget::path() const
{
    return m_path;
}

void LookViewItemWidget::setPath(const QString &path)
{
    m_path = path;
}

const QPixmap * LookViewItemWidget::image() const
{
    return &m_pixmap;
}

void LookViewItemWidget::setImage(const QPixmap & img)
{
    m_pixmap = img;
}

void LookViewItemWidget::setup()
{
    QFileInfo info(m_path);

    QHBoxLayout * hLayout = new QHBoxLayout(this);
    hLayout->setContentsMargins(4, 4, 4, 4);
    m_thumbnail = new QLabel();
    m_thumbnail->setPixmap(m_pixmap);
    hLayout->addWidget(m_thumbnail);

    QVBoxLayout * vLayout = new QVBoxLayout();
    vLayout->setContentsMargins(0, 0, 0, 0);
    m_name = new QLabel(info.fileName());
    vLayout->addWidget(m_name);
    m_date = new QLabel(QString("Last modified : %1").arg(info.lastModified().toString()));
    vLayout->addWidget(m_date);
    vLayout->addStretch(1);

    hLayout->addLayout(vLayout);
    hLayout->addStretch(1);
}
