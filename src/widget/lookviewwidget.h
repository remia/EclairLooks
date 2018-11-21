#pragma once

#include <QtWidgets/QListWidget>


class LookWidget;
class LookViewItemWidget;
class QListWidget;
class QListWidgetItem;
class QImage;
class QPixmap;
class QStringList;
class QLabel;

class LookViewWidget : public QListWidget
{
  public:
    enum class DisplayMode { Normal, Minimized };

  public:
    LookViewWidget(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void startDrag(Qt::DropActions supportedActions) override;

  public:
    void setLookWidget(LookWidget *lw);
    void setDisplayMode(DisplayMode m);
    void setReadOnly(bool ro);

    uint16_t countLook() const;
    QString currentLook() const;
    QStringList allLook() const;
    int indexLook(const QString &path) const;

  public:
    void appendFolder(const QString &path);
    void appendLook(const QString &path);

    void removeSelection(int selectedRow);
    void updateSupportedExtensions(const QStringList extensions);

  private:
    void addLook(const QString &path);
    QImage computeThumbnail(const QString &path);

  private:
    LookWidget *m_lookWidget;
    DisplayMode m_displayMode;
    bool m_readOnly;
    QStringList m_SupportedExtensions;
};

class LookViewItemWidget : public QWidget
{
  public:
    LookViewItemWidget(QWidget *parent = nullptr);

  public:
    QString path() const;
    void setPath(const QString &path);

    const QPixmap * image() const;
    void setImage(const QPixmap & img);

    void setup();

  private:
    QLabel * m_thumbnail;
    QLabel * m_name;
    QLabel * m_date;

    QString m_path;
    QPixmap m_pixmap;
};