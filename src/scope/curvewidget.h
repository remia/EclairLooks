#pragma once

#include "../utils/generic.h"
#include "../image.h"

#include <vector>
#include <map>

#include <QtWidgets/QGraphicsView>


class Image;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsItem;
class QGraphicsLineItem;
class QGraphicsTextItem;
class QGraphicsPathItem;
class QVector3D;

struct CurveItems {
    Image image;
    QGraphicsPathItem *curve[3];
    QGraphicsLineItem *cursorHLine[3];
    QGraphicsLineItem *cursorVLine;
    QGraphicsTextItem *cursorText;
};

class CurveWidget : public QGraphicsView
{
  public:
    CurveWidget(QWidget *parent = nullptr);

  public:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

  public:
    void clearView();

    void drawCurve(uint8_t id, const Image &img);
    void clearCurve(uint8_t id);

  private:
    CurveItems initCurve(uint8_t id, const Image &img);

    void drawGrid();
    void drawCurve(const CurveItems &items);
    void drawCursor(uint16_t x, uint16_t y);

  private:
    QGraphicsScene *m_scene;
    std::map<uint8_t, CurveItems> m_curves;
};