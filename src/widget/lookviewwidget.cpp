#include "lookviewwidget.h"
#include "imagewidget.h"
#include "../imagepipeline.h"
#include "../operator/imageoperatorlist.h"

#include <QtWidgets/QtWidgets>


LookViewTabWidget::LookViewTabWidget(QWidget *parent)
:   QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    QObject::connect(this, &QTabWidget::currentChanged, this, &LookViewTabWidget::tabChanged);
}

void LookViewTabWidget::showPreview(const QString &path)
{
    QFileInfo fileInfo(path);
    QString dirPath;
    if (fileInfo.isDir())
        dirPath = fileInfo.absoluteFilePath();
    else
        dirPath = fileInfo.dir().absolutePath();

    QDir rootDir(m_rootPath);
    QString relPath = rootDir.relativeFilePath(dirPath);

    if (auto [exists, index] = tabExists(relPath); !exists) {
        LookViewWidget *lookViewWidget = new LookViewWidget();
        lookViewWidget->setPipeline(m_globalPipeline);
        lookViewWidget->setOperators(m_operators);
        lookViewWidget->setLookViewTabWidget(this);
        lookViewWidget->showPreview(dirPath);
        addTab(lookViewWidget, relPath);
        setCurrentIndex(count() - 1);
    }
    else {
        setCurrentIndex(index);
    }
}

void LookViewTabWidget::setPipeline(ImagePipeline *pipeline)
{
    m_globalPipeline = pipeline;
}

void LookViewTabWidget::setOperators(ImageOperatorList *list)
{
    m_operators = list;
}

void LookViewTabWidget::setBrowserRootPath(const QString &path)
{
    m_rootPath = path;
}

void LookViewTabWidget::updateSelection(const QString &path)
{
    EmitEvent<Select>(path);
}

void LookViewTabWidget::tabChanged(int index)
{
    LookViewWidget *widget = static_cast<LookViewWidget*>(currentWidget());
    widget->updateSelection();
}

std::tuple<bool, uint16_t> LookViewTabWidget::tabExists(const QString &name)
{
    for (uint16_t i = 0; i < count(); ++i)
        if (tabText(i) == name)
            return { true, i };

    return { false, 0 };
}



LookViewWidget::LookViewWidget(QWidget *parent)
:   QWidget(parent), m_thumbSize(256, 256)
{
    m_pipeline = std::make_unique<ImagePipeline>();

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);
    m_lookList = new QListWidget();
    vLayout->addWidget(m_lookList);

    QObject::connect(m_lookList, &QListWidget::itemSelectionChanged, this, &LookViewWidget::updateSelection);
}

void LookViewWidget::setLookViewTabWidget(LookViewTabWidget *w)
{
    m_lookViewTabWidget = w;
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
    m_pipeline->SetInput(m_globalPipeline->GetInput().resize(m_thumbSize.width(), m_thumbSize.height()));
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
    if (ImageOperator *op = m_operators->CreateFromPath(path.toStdString())) {
        m_pipeline->Reset();
        m_pipeline->AddOperator(op);
        Image img = m_pipeline->GetOutput().to_type(PixelType::Uint8);
        QImage qImg = QImage(img.pixels(), img.width(), img.height(), img.width() * img.channels() * 1, QImage::Format_RGB888);

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