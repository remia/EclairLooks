#include "graphic.h"

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>

#include <utils/generic.h>
#include <utils/gl.h>
#include <utils/chrono.h>
#include <core/image.h>
#include <gui/common/common.h>


// ----------------------------------------------------------------------------

constexpr const float SceneRatio = 2.75f;

enum ZValue {
    ZBackground,
    ZGridInner,
    ZGridOuter,
    ZCurve,
    ZPoint,
    ZForeground
};

// ----------------------------------------------------------------------------

PointItem::PointItem(CurveItem *curve)
: QGraphicsEllipseItem(curve), m_curve(curve)
{
    setZValue(ZPoint);
    setRect(0, 0, m_size, m_size);
    setFlags(
        QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemClipsToShape);

    QTransform f;
    f.scale(1.f / SceneRatio, 1.f);
    f.translate(-m_size / 2.f, -m_size / 2.f);
    setTransform(f);
}

void PointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // if (CurveScene* s = static_cast<CurveScene*>(scene())) {
    //     s->selectedCurve(curve());
    //     qInfo() << "SelectedCurve by point";
    // }

    if (event->button() == Qt::LeftButton) {
        setBrush(QBrush(m_color, Qt::Dense3Pattern));
    }
    else if (event->button() == Qt::RightButton) {
        // Remove itself from the curve
        event->ignore();
        m_curve->removePoint(*this);
        return;
    }

    QGraphicsEllipseItem::mousePressEvent(event);
}

void PointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setBrush(QBrush(m_color, Qt::SolidPattern));
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}

void PointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // First move the point, then check and correct its position if needed
    QGraphicsEllipseItem::mouseMoveEvent(event);
    m_curve->updatePointPosition(*this);
}

void PointItem::setColor(const QColor &color)
{
    m_color = color;

    QPen pen(m_color);
    pen.setWidth(0);

    setPen(pen);
    setBrush(QBrush(m_color, Qt::SolidPattern));
}

// ----------------------------------------------------------------------------

CurveItem::CurveItem(CurveScene *scene, QGraphicsItem *parent)
: QGraphicsPathItem(parent), m_scene(scene)
{
    setZValue(ZCurve);
    setFlags(
        QGraphicsItem::ItemClipsToShape |
        QGraphicsItem::ItemIsSelectable);

    initCurve();
    updateCurve();
}

void CurveItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // m_scene->selectedCurve(this);

    // Add a control point
    if (QGuiApplication::keyboardModifiers() == Qt::NoModifier &&
        event->button() == Qt::LeftButton) {
        QPointF pos = event->scenePos();

        // Find where to insert the point (at which index)
        int index = m_points.size() - 1;
        while (m_points[index]->pos().x() > pos.x() && index > 0)
            index--;

        insertPoint(index + 1, {
            static_cast<float>(pos.x()),
            static_cast<float>(pos.y()),
            0.f, 0.f, 0.f});

        // Work is done, ignore the event so that curve item looses mouse focus
        event->ignore();
        // Post a new press event that will be catched by the newly added point
        // Qt should pick the point first because of construction order (if not
        // see setZValue() in CurvePointItem constructor...)
        QGraphicsSceneMouseEvent *mousePress = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
        mousePress->accept();
        mousePress->setButton(event->button());
        mousePress->setButtons(event->buttons());
        mousePress->setScreenPos(event->screenPos());
        mousePress->setScenePos(event->scenePos());
        mousePress->setModifiers(event->modifiers());
        mousePress->setWidget(event->widget());
        mousePress->setButtonDownPos(event->button(), event->buttonDownPos(event->button()));
        mousePress->setButtonDownScenePos(event->button(), event->buttonDownScenePos(event->button()));
        mousePress->setButtonDownScreenPos(event->button(), event->buttonDownScreenPos(event->button()));
        QCoreApplication::postEvent(scene(), mousePress);
    }

    QGraphicsPathItem::mousePressEvent(event);
}

void CurveItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPathItem::mouseReleaseEvent(event);
}

void CurveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Don't draw default dashed rectangle when curve is selected...
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, &myOption, widget);
}

QPainterPath CurveItem::shape() const
{
    // Allow selection with a margin of error
    return m_shape;
}

QRectF CurveItem::boundingRect() const
{
    return shape().boundingRect();
}

void CurveItem::setColor(const QColor &color)
{
    m_color = color;

    QPen pen(m_color);
    setPen(pen);
}

int CurveItem::insertPoint(int index, const Knots &p)
{
    UPtr<PointItem> item(new PointItem(this));
    item->setPos(p.x, p.y);
    item->setColor(m_color);

    m_points.insert(m_points.begin() + index, std::move(item));

    updateCurve();

    return index;
}

void CurveItem::removePoint(PointItem &p)
{
    int index = indexOf(p);
    if (index != 0 && index != m_points.size() - 1)
        m_points.erase(m_points.begin() + index);

    updateCurve();
}

void CurveItem::updateCurve()
{
    if (m_points.empty())
        return;

    // Curve curve = toCurve();
    QPainterPath path;

    // Get the range of where to draw
    QRectF r = m_scene->viewRect();
    qInfo() << r;
    // Only compute points between this range (1 per pixel)


    // Linear...
    // if (m_points.size() <= 2) {
        // path.moveTo(m_points[0]->x(), m_points[0]->y());
        // for (auto& point : m_points)
        //     path.lineTo(point->x(), point->y());
    // else {
    //     // Catmull-Rom
    //     int sceneWidth = scene()->views().first()->size().width();
    //     auto points = InterpolateCurve(curve, sceneWidth / 4);

    //     path.moveTo(m_points[0]->x(), m_points[0]->y());
    //     for (auto & point : points)
    //         path.lineTo(point.x, point.y);
    // }

    // Need to use actual curve points because the one in the scene will
    // be transformed related to the viewing scale / translate mode
    // KnotsV kv;
    // for (auto & p : m_points)
    //     kv.push_back( {p->x(), p->y()} );
    // Curve c;
    // c.knots = kv;

    // path.moveTo(0.f, 0.f);
    // for (int x = 0; x < width(); ++x) {
    //     float xn = x / (width() - 1.);
    //     xn = r.left() + xn * r.width();
    //     float yn = c.eval(xn);
    //     path.lineTo(xn, yn);
    // }

    QPen pen(m_color);
    pen.setWidth(0);
    setPath(path);
    setPen(pen);

    QPainterPathStroker s;
    s.setWidth(0.125);
    m_shape = s.createStroke(path);

    // if (CurveScene* s = static_cast<CurveScene*>(scene()))
    //     s->updatedCurve(curve);
}

void CurveItem::resetCurve()
{
    m_points.clear();
}

// void CurveItem::initCurve(Curve c)
// {
//     m_points.clear();

//     for (auto p : c.points) {
//         UPtr<PointItem> item(new PointItem(this));
//         item->setPos(p.x, p.y);
//         item->setColor(m_color);

//         m_points.push_back(std::move(item));
//     }

//     updateCurve();
// }

void CurveItem::updatePointPosition(PointItem &p)
{
    int index = indexOf(p);
    if (index < 0)
        return;

    // Check outside grid
    if (p.x() < 0.0)
        p.setPos(0, p.y());
    else if (p.x() > 1.0)
        p.setPos(1.0, p.y());

    if (p.y() < 0.0)
        p.setPos(p.x(), 0.0);
    else if (p.y() > 1.0)
        p.setPos(p.x(), 1.0);

    // First and last points should stick to closest axis
    if (index == 0) {
        if (p.x() > p.y())
            p.setPos(p.x(), 0);
        else
            p.setPos(0, p.y());
    }
    else if (index == m_points.size() - 1) {
        if (p.x() < p.y())
            p.setPos(p.x(), 1.0);
        else
            p.setPos(1.0, p.y());
    }
    // Strict ordering
    float interval_min = 0.001;
    if (index > 0) {
        if (p.x() <= m_points[index - 1]->x())
            p.setPos(m_points[index - 1]->x() + interval_min, p.y());
    }
    if (index < m_points.size() - 1) {
        if (p.x() >= m_points[index + 1]->x())
            p.setPos(m_points[index + 1]->x() - interval_min, p.y());
    }

    updateCurve();
}

// Curve CurveItem::toCurve() const
// {
//     Curve c;

//     for (auto & p : m_points) {
//         ControlPoint cp;
//         cp.x = p->x();
//         cp.y = p->y();
//         c.points.push_back(cp);
//     }

//     return c;
// }

void CurveItem::initCurve()
{
    m_points.clear();

    setPath(QPainterPath());

    QPen pen(m_color);
    pen.setWidth(0);
    setPen(pen);
}

int CurveItem::indexOf(const PointItem &p) const
{
    uint16_t index = 0;
    for (auto& ptr : m_points) {
        if (ptr.get() == &p)
            return index;
        index++;
    }

    return -1;
}

// ----------------------------------------------------------------------------

CurveScene::CurveScene(QObject *parent)
: QGraphicsScene(parent)
{
    // Populate scene
    // m_grid.reset(new GridItem(this));

    // m_background.reset(new QGraphicsPixmapItem);
    // m_background->setPos(0.0, 0.0);
    // m_background->setOpacity(0.75);
    // m_background->setZValue(ZBackground);
    // addItem(m_background.get());

    // Connecting
    // QObject::connect(this, &CurveScene::updatedCurve, [this](const Curve &c){
    //     updatedCurve(m_selectedCurve, c);
    // });

    // QObject::connect(this, &CurveScene::selectedCurve, [this](CurveItem *item){
    //     qInfo() << "SelectedCurve : " << item;
    //     // TODO : don't send if in gang mode
    //     for (int i = 0; i < m_curves.size(); ++i) {
    //         if (item == m_curves[i].get()) {
    //             selectedCurve(i + 1);
    //             qInfo() << "SelectedCurve : " << i;
    //         }
    //     }
    // });
}

// QGraphicsPixmapItem* CurveScene::background()
// {
//     return m_background.get();
// }

QRectF CurveScene::viewRect() const
{
    return m_viewRect;
}

void CurveScene::addCurve(const CurveId &curve, const KnotsV &p, const QColor &color)
{
    UPtr<CurveItem> item(new CurveItem(this));
    item->setColor(color);

    uint16_t index = 0;
    for (auto point : p)
        item->insertPoint(index++, point);

    addItem(item.get());
    m_curves[curve] = std::move(item);
}


void CurveScene::selectCurve(CurveItem *curve)
{
    m_select = curve;

    for (auto &[param, item] : m_curves) {
        // item->setVisible(false);
        item->setEnabled(false);
    }

    if (curve) {
        for (auto &[param, item] : m_curves) {
            item->setVisible(true);
            item->setZValue(item.get() == curve ? ZForeground : ZCurve);
            // TODO : Should stay enabled but when all curves are identity
            // and stacked, if we leave them enabled, the stacking Z order
            // don't prevent to accidentally select the wrong one.
            item->setEnabled(item.get() == curve);
        }
    }
}

void CurveScene::clearScene()
{
    m_curves.clear();
    // m_grid.reset();
}

void CurveScene::redrawScene(const QRectF &extend)
{
    m_viewRect = extend;
    for (auto &[param, item] : m_curves) {
        item->updateCurve();
    }
}

// ----------------------------------------------------------------------------

CurveView::CurveView(QWidget *parent)
: QGraphicsView(parent)
{
    // setBackgroundColor(m_background_color);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewport(new QOpenGLWidget());
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setSceneRect(QRectF(0.0, 0.0, 1.0, 1.0));
}

QSize CurveView::sizeHint() const
{
    return QSize(1280, 720);
}

void CurveView::resizeEvent(QResizeEvent *event)
{
    float srcRatio = SceneRatio;
    float dstRatio = 1.0f * event->size().width() / event->size().height();
    float wMargin = 1.;
    float hMargin = 1.;
    srcRatio = dstRatio;

    if (m_keepAspectRatio) {
        if (dstRatio > srcRatio)
            m_ratio = QPointF(event->size().height() * srcRatio, hMargin * -event->size().height());
        else
            m_ratio = QPointF(wMargin * event->size().width(), -event->size().width() / srcRatio);

        updateTransform();
    }
    else {
        // fitInView(scene()->sceneRect());
    }

    computeBackground();

    // qInfo() << "SceneRect : " << sceneRect();
    // qInfo() << "ViewExtend : " << mapToScene(rect());
}

void CurveView::wheelEvent(QWheelEvent *event)
{
    float delta = 0.f;

    QPoint numPixels = event->pixelDelta();
    if (!numPixels.isNull())
        delta = numPixels.y() / 60.0;
    else
        delta = event->angleDelta().y() / 60.0;

    delta = std::clamp(delta, -0.2f, 0.2f);
    m_zoom += QPointF(delta, delta);
    m_zoom = QPointF(
        std::clamp(m_zoom.x(), 0.1, 25.),
        std::clamp(m_zoom.y(), 0.1, 25.));

    m_zoomPos = QPointF(
        event->posF().x() / width(),
        event->posF().y() / height());

    updateTransform();

    QGraphicsView::wheelEvent(event);
}

void CurveView::mousePressEvent(QMouseEvent *event)
{
    if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        setMouseTracking(true);
        m_clickPosition = QPointF(
            event->localPos().x() / width(),
            event->localPos().y() / height());
    }

    QGraphicsView::mousePressEvent(event);
}

void CurveView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_zoom = QPointF(1.f, 1.f);
        m_position = QPointF();
        m_moveDelta = QPointF();
    }

    viewport()->update();
}

void CurveView::mouseMoveEvent(QMouseEvent *event)
{
    // qInfo() << "MouseMovePos X" << event->localPos().x() / width();

    if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        m_moveDelta = QPointF(
            event->localPos().x() / width(),
            event->localPos().y() / height()) - m_clickPosition;

        // Pan sensibility depends on zoom
        m_moveDelta.rx() /= m_zoom.x();
        m_moveDelta.ry() /= m_zoom.y();

        viewport()->update();
        curveScene()->redrawScene(viewRect());
    }

    QGraphicsView::mouseMoveEvent(event);
}

void CurveView::mouseReleaseEvent(QMouseEvent *event)
{
    setMouseTracking(false);
    m_position += m_moveDelta;
    m_moveDelta = QPointF(0.f, 0.f);

    QGraphicsView::mouseReleaseEvent(event);
}

void CurveView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);

    QPoint size = QPoint(painter->window().width(), painter->window().height());
    QPointF position = m_position + m_moveDelta;

    //
    // Draw background
    //

    painter->fillRect(rect, m_background_color);

    //
    // Grid setup
    //

    // Draw in least to most significant lines, each overwriting the previous one
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    // A future feature could be to dynamically update these scales step as
    // the zoom factor is ajusted, it will make new lines pop in or out
    // to fill the spaces appropriately.
    VecT<float> scales = { 0.01, 0.1, 0.5 };
    VecT<float> opacity = { 0.40, 0.60, 1. };
    VecT<QColor> colors = { QColor(50, 50, 50), QColor(80, 255, 80), QColor(255, 150, 150) };

    QPen pen;
    pen.setColor(m_grid_color);
    pen.setWidth(0);
    painter->setPen(pen);

    //
    // Legend setup
    //

    QFont font = painter->font();
    font.setPointSize(9);
    painter->setFont(font);
    QFontMetrics metric(painter->font());

    int scaleLegend = scales.size() - 2;
    auto drawLegend = [&](float v, int x, int y){
        // Drawing text on normalized coordinates don't seems to work, maybe
        // font engine must directly draw on screen on expect font size in pixels
        // only and not in arbitrary scale
        float opacity = painter->opacity();
        painter->setOpacity(1.0);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter->setWorldMatrixEnabled(false);
        painter->setViewTransformEnabled(false);

            QString str = QString::number(v, 'f', 2);
            QRect bbox = metric.boundingRect(str);
            painter->drawText(QPointF(x - bbox.width() / 2, y - bbox.height() / 2), str);

        painter->setWorldMatrixEnabled(true);
        painter->setViewTransformEnabled(true);
        painter->setCompositionMode(QPainter::CompositionMode_Source);
        painter->setOpacity(opacity);
    };

    //
    // Drawing...
    //

    for (int s = 0; s < scales.size(); ++s) {
        float range = 1. / m_zoom.x();
        float step = scales[s];
        // We draw one more for case when the offset is negative
        int count = ceil(1. * range / step) + 1;

        float x1 = -position.x() + -m_zoomPos.x() / m_zoom.x() + m_zoomPos.x();
        float y1 = -position.y() + -m_zoomPos.y() / m_zoom.y() + m_zoomPos.y();

        float off;
        float xa = modf(x1 / scales[s], &off) * scales[s];
        float ya = modf(y1 / scales[s], &off) * scales[s];

        // qInfo() << "_____________________________________________";
        // qInfo() << "SCALE" << s;
        // if (s >= 1) {
        //     qInfo() << "\tOffset B [" << xa << ya << "]";
        //     qInfo() << "\tRange X [" << x1 << x1 + range << "] Y [" << y1 << y1 + range << "]";
        //     qInfo() << "\tRange" << range << "\tStep" << step << "\tCount" << count;
        // }

        // For each scale, we compute the number of lines to be drawn
        // Each coordinate starts from absolute position in space, then
        // translation and scale factors are applied.

        painter->setOpacity(opacity[s]);

        for (int i = 0; i < count; ++i) {
            float x = x1 - xa + i * step;
            x = x + position.x();
            x = (x - m_zoomPos.x()) * m_zoom.x() + m_zoomPos.x();
            painter->drawLine(QLineF(x, 0.0, x, 1.0));

            float y = y1 - ya + i * step;
            y = y + position.y();
            y = (y - m_zoomPos.y()) * m_zoom.y() + m_zoomPos.y();
            painter->drawLine(QLineF(0.0, 1. - y, 1.0, 1. - y));

            // if (s >= 1) qInfo() << "\tIteration" << i << "\tX" << x << "\tY" << y;

            if (s == scaleLegend) {
                drawLegend(x1 + x * range, x * size.x(), size.y() - 2);
                drawLegend(1 - (y1 + y * range), 15, y * size.y());
            }
        }
    }

    //
    // Draw Axis
    //

    // Check if range is inside x [0..1] or y [0..1], then compute offset to axis
    // and draw it !
    // painter->setOpacity(1.0);
    // pen.setColor(QColor("red"));
    // painter->setPen(pen);

    // painter->drawLine(QLineF(0.0, 0.0, 2.0, 0.0));
    // painter->drawLine(QLineF(0.0, 0.0, 0.0, 2.0));
    // pen.setColor(m_grid_color);
    // painter->setPen(pen);

    //
    // Debug print
    //

    // pen.setColor(QColor("white"));
    // painter->setPen(pen);

    // painter->drawText(35, 15, QString("Translation : %1,%2").arg(position.x()).arg(position.y()));
    // painter->drawText(35, 35, QString("Zoom : %1,%2").arg(m_zoom.x()).arg(m_zoom.y()));
    // painter->drawText(35, 55, QString("ZoomPos : %1,%2").arg(m_zoomPos.x()).arg(m_zoomPos.y()));
    // painter->drawText(35, 75, QString("Offset : %1").arg(xa));
}

CurveScene* CurveView::curveScene()
{
    return static_cast<CurveScene*>(scene());
}

QRectF CurveView::viewRect() const
{
    QPointF position = m_position + m_moveDelta;

    float range = 1. / m_zoom.x();
    float x1 = -position.x() + -m_zoomPos.x() / m_zoom.x() + m_zoomPos.x();
    float y1 = -position.y() + -m_zoomPos.y() / m_zoom.y() + m_zoomPos.y();

    return QRectF(QPointF(x1, y1), QSizeF(range, range));
}

void CurveView::setBackgroundColor(const QColor &color)
{
    m_background_color = color;
    setBackgroundBrush(QBrush(m_background_color));
}

void CurveView::setBackgroundColorCb(const ColorCallback &cb)
{
    m_background_color_cb = cb;
    computeBackground();
}

// void CurveView::updateCurve(int id, const Curve &curve)
// {
//     m_curves[id]->initCurve(curve);
// }

void CurveView::updateTransform()
{
    resetTransform();

    QTransform sceneTx;
    sceneTx.scale(m_ratio.x(), m_ratio.y());
    // sceneTx.scale(m_zoom, m_zoom);
    // sceneTx.translate(1000, 1000);
        // m_position.x() + m_moveDelta.x(),
        // m_position.y() + m_moveDelta.y());

    // if (m_zoom == 1.) {
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    // }
    // else
    //     setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setTransform(sceneTx);
    // qInfo() << "Scrollbar H : " << horizontalScrollBar()->value();
    // qInfo() << "Scrollbar V : " << verticalScrollBar()->value();
    // qInfo() << "Matrix : " << matrix();
    // qInfo() << "Transform : " << transform();
    // qInfo() << "SceneRect : " << sceneRect();
    // qInfo() << "ResizeAnchor : " << resizeAnchor();
}

void CurveView::computeBackground()
{
    if (!m_background_color_cb)
        return;

    QImage img(width(), height(), QImage::Format_RGB32);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            float u = x / (width() - 1.f);
            float v = std::abs(y / (height() - 1.f) - 0.5) * 2.f;
            ColorRGBA8 c = (*m_background_color_cb)(u, v);

            img.setPixel(x, y, toQColor(c).rgb());
        }
    }

    // curveScene()->background()->setPixmap(QPixmap::fromImage(img));
    // curveScene()->background()->setTransform(
    //     QTransform().scale(1. / img.width(), 1. / img.height()));
}
