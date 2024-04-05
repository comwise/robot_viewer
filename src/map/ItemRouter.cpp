#include "ItemRouter.h"
#include <QPainter>
#include <QPen>
#include <QRadialGradient>

const double g_route_size = 10;

ItemRouter::ItemRouter(const QPointF &source, const QPointF &fst,
        const QPointF &scend, const QPointF &dest)
    : ItemBase(GraphicsItemType::ITEM_LINE)
    , scale_factor_(1.0/0.3)
    , sourcenode_pos_(source)
    , fstctrlpt_pos_(fst)
    , secctrlpt_pos_(scend)
    , destnode_pos_(dest)
{
    setFlag(ItemIsSelectable, false);
    setAcceptHoverEvents(false);
    setZValue(-10);
}

void ItemRouter::SetPoint(
        const QPointF &source, const QPointF &fst,
        const QPointF &scend, const QPointF &dest_pos)
{
    sourcenode_pos_ = source;
    fstctrlpt_pos_ = fst;
    secctrlpt_pos_ = scend;
    destnode_pos_ = dest_pos;
}


QPainterPath ItemRouter::shape() const
{
    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);
    QPainterPathStroker stroker;
    stroker.setWidth(g_route_size / scale_factor_);
    return stroker.createStroke(path);
}

QRectF ItemRouter::boundingRect() const
{
    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);

    QPainterPathStroker stroker;
    stroker.setWidth(7.5 * g_route_size / scale_factor_);
    path = stroker.createStroke(path);
    return (path.controlPointRect());
}

void ItemRouter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    scale_factor_ = scale();
    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);

    QPainterPathStroker stroker;
    stroker.setWidth(g_route_size / scale_factor_);
    painter->setPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(QColor(Qt::red));
    path = stroker.createStroke(path);
    painter->drawPath(path);
}