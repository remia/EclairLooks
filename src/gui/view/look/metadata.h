#pragma once

#include <OpenColorIO/OpenColorIO.h>
#include <QtWidgets/QWidget>


class QTreeWidget;
class QTreeWidgetItem;
class Metadata;

class LookMetadataWidget : public QWidget
{
  public:
    LookMetadataWidget(QWidget *parent = nullptr);

  public:
    void clearView();
    void updateView(const QString &path);

  private:
    void dumpMetadata(OCIO_NAMESPACE::Metadata &metadata, QTreeWidgetItem * parent = nullptr);

  private:
    QString m_filePath;
    QTreeWidget * m_metadataTree;

    OCIO_NAMESPACE::Metadata m_metadata;
};