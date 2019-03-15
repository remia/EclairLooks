#pragma once

#include <vector>

#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsEllipseItem>

#include <utils/generic.h>
#include "curve.h"


//
// This framework must be re used for the neutral curves plot..
//


class CurveScene;
class CurveItem;

// ----------------------------------------------------------------------------

class PointItem : public QGraphicsEllipseItem
{
  public:
    PointItem(CurveItem *curve);

  public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  public:
    void setColor(const QColor &color);

  private:
    CurveItem* m_curve;

    UPtr<QGraphicsLineItem> m_tangent;
    UPtr<QGraphicsEllipseItem> m_pointLeft;
    UPtr<QGraphicsEllipseItem> m_pointRight;

    float m_size = 0.03;
    QColor m_color;
};

// ----------------------------------------------------------------------------

class CurveItem : public QGraphicsPathItem
{
  public:
    CurveItem(CurveScene *scene, QGraphicsItem *parent = nullptr);

  public:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

  public:
    int insertPoint(int index, const Knots &p);
    void removePoint(PointItem &p);

    void updateCurve();
    void resetCurve();
    void updatePointPosition(PointItem &p);

    void setColor(const QColor &color);
    QPainterPath shape() const override;
    QRectF boundingRect() const override;

  private:
    void initCurve();

    int indexOf(const PointItem &p) const;

  private:
    CurveScene* m_scene;

    UPtrV<PointItem> m_points;
    QPainterPath m_shape;
    QColor m_color = QColor(200, 200, 200);
};

// ----------------------------------------------------------------------------

class CurveScene : public QGraphicsScene
{
  Q_OBJECT

  public:
    using CurveId = std::string;
    using CurveMap = std::map<CurveId, UPtr<CurveItem>>;

  public:
    CurveScene(QObject *parent = nullptr);

  public:
    // QGraphicsPixmapItem* background();
    QRectF viewRect() const;

  public:
    void addCurve(const CurveId &curve, const KnotsV &p, const QColor &color);
    void selectCurve(CurveItem *curve);
    void redrawScene(const QRectF &extend);
    void clearScene();

  signals:
    void selectedCurve(const CurveId &id);
    void updatedCurve(const CurveId &id, const KnotsV &p);

  private:
    CurveMap m_curves;
    // UPtr<QGraphicsPixmapItem> m_background;
    CurveItem *m_select = nullptr;

    // Must have a reference to the curve to have the real points position
    // Because the points will not be drawn at the actual position depending
    // on zoom and scale. These will need to be updated when individual points
    // are drag and dropped. Maybe each PointItem need to know its index in the
    // curve ? And be updated accordingly when points are added or removed.

    QRectF m_viewRect;
};

// ----------------------------------------------------------------------------

class CurveView : public QGraphicsView
{
  Q_OBJECT

  public:
    CurveView(QWidget *parent = nullptr);

  public:
    QSize sizeHint() const override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;

  public:
    QRectF viewRect() const;
    void setBackgroundColor(const QColor &color);
    void setBackgroundColorCb(const ColorCallback &cb);

  private:
    CurveScene* curveScene();
    void updateTransform();
    void computeBackground();

  private:
    QColor m_background_color = QColor(39, 39, 44);
    OptT<ColorCallback> m_background_color_cb = std::nullopt;

    uint16_t m_grid_size = 5;
    QColor m_grid_color = QColor(150, 150, 150);

    bool m_keepAspectRatio = true;

    QPointF m_position;
    QPointF m_clickPosition;
    QPointF m_moveDelta;
    QPointF m_ratio;
    QPointF m_zoom = QPointF(1.f, 1.f);
    QPointF m_zoomPos;

    // Selection mode
    // ALL (Gang)
    // IDX
};

// ----------------------------------------------------------------------------
