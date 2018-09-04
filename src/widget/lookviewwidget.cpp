#include "lookviewwidget.h"
#include "lookwidget.h"
#include "imagewidget.h"
#include "../image.h"

#include <QtWidgets/QtWidgets>


LookViewTabWidget::LookViewTabWidget(QWidget *parent)
:   QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    QObject::connect(this, &QTabWidget::currentChanged, this, &LookViewTabWidget::tabChanged);
    QObject::connect(this, &QTabWidget::tabCloseRequested, this, &LookViewTabWidget::tabClosed);
}

void LookViewTabWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
}

void LookViewTabWidget::showPreview(const QString &path)
{
    QFileInfo fileInfo(path);
    QString dirPath;
    if (fileInfo.isDir())
        dirPath = fileInfo.absoluteFilePath();
    else
        dirPath = fileInfo.dir().absolutePath();

    QDir rootDir(m_lookWidget->rootPath());
    QString relPath = rootDir.relativeFilePath(dirPath);

    if (auto [exists, index] = tabExists(relPath); !exists) {
        LookViewWidget *lookViewWidget = new LookViewWidget();
        lookViewWidget->setLookWidget(m_lookWidget);
        lookViewWidget->setLookViewTabWidget(this);
        lookViewWidget->showPreview(dirPath);

        if (lookViewWidget->countLook() >= 1) {
            addTab(lookViewWidget, relPath);
            setCurrentIndex(count() - 1);
        }
        else {
            delete lookViewWidget;
        }
    }
    else {
        setCurrentIndex(index);
    }
}

void LookViewTabWidget::updateSelection(const QString &path)
{
    EmitEvent<Select>(path);
}

void LookViewTabWidget::tabChanged(int index)
{
    LookViewWidget *widget = static_cast<LookViewWidget*>(currentWidget());
    if (widget)
        widget->updateSelection();
}

void LookViewTabWidget::tabClosed(int index)
{
    removeTab(index);
}

TupleT<bool, uint16_t> LookViewTabWidget::tabExists(const QString &name)
{
    for (uint16_t i = 0; i < count(); ++i)
        if (tabText(i) == name)
            return { true, i };

    return { false, 0 };
}



LookViewWidget::LookViewWidget(QWidget *parent)
:   QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    m_lookList = new QListWidget();
    vLayout->addWidget(m_lookList);

    QObject::connect(m_lookList, &QListWidget::itemSelectionChanged, this, &LookViewWidget::updateSelection);
}

void LookViewWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
}

void LookViewWidget::setLookViewTabWidget(LookViewTabWidget *w)
{
    m_lookViewTabWidget = w;
}

uint16_t LookViewWidget::countLook() const
{
    return m_lookList->count();
}

void LookViewWidget::showPreview(const QString &path)
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