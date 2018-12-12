#include "lookselectionwidget.h"
#include "lookviewwidget.h"
#include "lookwidget.h"
#include "../operator/imageoperator.h"

#include <QtWidgets/QtWidgets>
#include <QFile>


LookSelectionWidget::LookSelectionWidget(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(4, 4, 4, 4);

    m_viewWidget = new LookViewWidget();
    m_viewWidget->setDragEnabled(false);
    m_viewWidget->setReadOnly(false);
    m_viewWidget->setDisplayMode(LookViewWidget::DisplayMode::Minimized);
    vLayout->addWidget(m_viewWidget);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);

    m_clearBtn = new QToolButton();
    m_clearBtn->setText("Clear");
    m_clearBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hLayout->addWidget(m_clearBtn);

    m_saveBtn = new QToolButton();
    m_saveBtn->setText("Save");
    m_saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hLayout->addWidget(m_saveBtn);

    m_loadBtn = new QToolButton();
    m_loadBtn->setText("Load");
    m_loadBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hLayout->addWidget(m_loadBtn);

    vLayout->addLayout(hLayout);

    QObject::connect(m_viewWidget, &QListWidget::itemSelectionChanged, this, &LookSelectionWidget::updateSelection);
    QObject::connect(m_clearBtn, &QToolButton::clicked, this, &LookSelectionWidget::clearSelection);
    QObject::connect(m_saveBtn, &QToolButton::clicked, this, &LookSelectionWidget::saveSelection);
    QObject::connect(m_loadBtn, &QToolButton::clicked, this, &LookSelectionWidget::loadSelection);
}

void LookSelectionWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void LookSelectionWidget::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        foreach (const QUrl &url, e->mimeData()->urls()) {
            m_viewWidget->appendLook(url.toLocalFile());
        }
    }
}

void LookSelectionWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
    m_viewWidget->setLookWidget(m_lookWidget);
}

LookViewWidget * LookSelectionWidget::viewWidget()
{
    return m_viewWidget;
}

void LookSelectionWidget::updateSelection()
{
    QList<QListWidgetItem *> items = m_viewWidget->selectedItems();
    if (!items.isEmpty()) {
        QString path = items[0]->data(Qt::UserRole).toString();
        EmitEvent<Select>(path);
    }
    else {
        EmitEvent<Reset>();
    }
}

void LookSelectionWidget::clearSelection()
{
    m_viewWidget->clear();
}

void LookSelectionWidget::saveSelection()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save Look Selection"),
        "eclair-look-selection.txt",
        tr("Text file (*.txt)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QTextStream s(&file);

    for (uint16_t i = 0; i < m_viewWidget->count(); ++i) {
        QListWidgetItem * item = m_viewWidget->item(i);
        QString path = item->data(Qt::UserRole).toString();

        QDir rootDir(m_lookWidget->lookBasePath());
        QString relPath = rootDir.relativeFilePath(path);
        s << relPath << endl;
    }
}

void LookSelectionWidget::loadSelection()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Load Look Selection"),
        "",
        tr("Text file (*.txt)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QTextStream s(&file);
    while (!s.atEnd()) {
        QString line = s.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QDir rootDir(m_lookWidget->lookBasePath());
        QDir lookPath = rootDir.filePath(line);
        m_viewWidget->appendLook(lookPath.absolutePath());
    }
}