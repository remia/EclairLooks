#include "metadata.h"

#include <fstream>

#include <QtWidgets/QtWidgets>

#include <context.h>
#include <operator/ocio/filetransform.h>


LookMetadataWidget::LookMetadataWidget(QWidget *parent)
    : QWidget(parent), m_metadata("Empty")
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(4, 4, 4, 4);

    m_metadataTree = new QTreeWidget();
    m_metadataTree->setColumnCount(2);
    m_metadataTree->setHeaderLabels(QStringList() << "Name" << "Value");
    vLayout->addWidget(m_metadataTree);
}

void LookMetadataWidget::clearView()
{
    m_metadataTree->clear();
}

void LookMetadataWidget::updateView(const QString &path)
{
    m_filePath = path;
    m_metadataTree->clear();

    OCIOFileTransform fileOp;
    fileOp.SetFileTransform(m_filePath.toStdString());

    m_metadata = fileOp.GetFileMetadata();
    if (m_metadata.isEmpty())
        return;

    dumpMetadata(m_metadata);

}

void LookMetadataWidget::dumpMetadata(OCIO_NAMESPACE::Metadata &metadata, QTreeWidgetItem * parent)
{
    QTreeWidgetItem *treeItem = nullptr;

    if (parent == nullptr) {
        treeItem = m_metadataTree->invisibleRootItem();
    }
    else {
        QString name = QString::fromStdString(metadata.getName());
        treeItem = new QTreeWidgetItem(QStringList() << name);
        parent->addChild(treeItem);
    }

    treeItem->setExpanded(true);

    for (auto &item : metadata.getItems()) {
        if (item.isLeaf()) {
            QTreeWidgetItem *childItem = new QTreeWidgetItem(QStringList()
                << QString::fromStdString(item.getName())
                << "");

            QLineEdit *itemLe = new QLineEdit(QString::fromStdString(item.getValue()));
            treeItem->addChild(childItem);
            m_metadataTree->setItemWidget(childItem, 1, itemLe);

            QObject::connect(itemLe, QOverload<>::of(&QLineEdit::returnPressed),
                [&, le = itemLe]() {
                    item = le->text().toStdString();
                    
                    // TODO: update metadata...

                    OCIO_NAMESPACE::FileConverterRcPtr convert = OCIO_NAMESPACE::FileConverter::Create();
                    OCIO_NAMESPACE::ConstConfigRcPtr config = OCIO_NAMESPACE::Config::Create();
                    convert->setConfig(config);
                    convert->setFormat("Academy/ASC Common LUT Format");
                    convert->setInputFile(m_filePath.toStdString().c_str());
                    convert->setMetadata(m_metadata);

                    std::ofstream os(m_filePath.toStdString() + ".ecl.clf");
                    convert->convert(os);
            });
        }
        else {
            dumpMetadata(item, treeItem);
        }
    }
}