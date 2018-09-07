#include "lookviewtabwidget.h"
#include "lookviewwidget.h"
#include "lookwidget.h"

#include <QtWidgets/QtWidgets>


LookViewTabWidget::LookViewTabWidget(QWidget *parent)
:   QTabWidget(parent)
{
    setTabsClosable(true);
    setMovable(true);

    QObject::connect(this, &QTabWidget::currentChanged, this, &LookViewTabWidget::tabChanged);
    QObject::connect(this, &QTabWidget::tabCloseRequested, this, &LookViewTabWidget::tabClosed);
}

QWidget *LookViewTabWidget::currentView()
{
    if (LookViewWidget *ptr = static_cast<LookViewWidget*>(currentWidget()))
        return ptr->lookListWidget();

    return nullptr;
}

void LookViewTabWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
}

void LookViewTabWidget::showFolder(const QString &path)
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
        lookViewWidget->showFolder(dirPath);

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
    else
        EmitEvent<Reset>();
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
