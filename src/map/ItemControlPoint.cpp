#include "ItemControlPoint.h"
#include "ItemNode.h"
#include "ItemEdge.h"
#include <QPainter>
#include <QPen>
#include <QRadialGradient>

static const double g_control_point_size = 35;

ItemControlPoint::ItemControlPoint(ItemEdge *edge, ItemNode *node)
    : ItemBase(GraphicsItemType::ITEM_CONTROLPOINT)
    , scale_factor_(1.0 / 0.3)
    , edge_(edge)
    , node_(node)
    , shape_r_(g_control_point_size)
    , adjust_(2)
    , hover_enter_(false)
{
    setFlag(ItemIsMovable, true);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    this->setAcceptedMouseButtons(Qt::LeftButton);
    this->setAcceptHoverEvents(true);
    setZValue(10);

    setParentItem(edge);
}

QRectF ItemControlPoint::boundingRect() const
{
    double shape_r = 1.5 * g_control_point_size / scale_factor_;
    return QRectF(-shape_r - adjust_, -shape_r - adjust_, 2 * (shape_r + adjust_), 2 * (shape_r + adjust_));
}

QPainterPath ItemControlPoint::shape() const
{
    double shape_r = g_control_point_size / scale_factor_;
    if (hover_enter_) {
        shape_r = 1.5 * g_control_point_size / scale_factor_;
    }

    QPainterPath path;
    path.addRect(-shape_r, -shape_r, 2 * shape_r, 2 * shape_r);
    return path;
}

void ItemControlPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    scale_factor_ = scale();
    double shape_r = g_control_point_size / scale_factor_;
    if (hover_enter_) {
        shape_r = 1.5 * g_control_point_size / scale_factor_;
    }

    painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
    QRadialGradient gradient(-3, -3, 2 * shape_r);

    gradient.setColorAt(0, Qt::cyan);
    gradient.setColorAt(1, Qt::blue);
    painter->setBrush(gradient);
    if (isSelected()) {
        painter->setPen(QPen(Qt::red, 0.5));
        gradient.setColorAt(1, Qt::cyan);
        gradient.setColorAt(0, Qt::blue);
        painter->setBrush(gradient);
    } else {
        painter->setPen(QPen(Qt::black, 0));
    }

    painter->drawRect(-shape_r, -shape_r, 2 * shape_r, 2 * shape_r);
    painter->drawText(-shape_r, -shape_r, tip_);
}

void ItemControlPoint::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    ItemNode *othernode_;
    ItemControlPoint *otherctrl_;

    if (node_ == edge_->sourceNode()) {
        othernode_ = edge_->destNode();
    } else if (node_ == edge_->destNode()) {
        othernode_ = edge_->sourceNode();
    }

    if (this == edge_->getFirstControlPoint()) {
        otherctrl_ = edge_->getSecControlPoint();
    } else {
        otherctrl_ = edge_->getFirstControlPoint();
    }

#if 0
    VecPosition pos(node_->scenePos().x(), node_->scenePos().y());
    VecPosition otherpos(othernode_->scenePos().x(), othernode_->scenePos().y());
    VecPosition movpos(event->scenePos().x(),event->scenePos().y());
    VecPosition vec1 = movpos-pos;
    VecPosition vec2 = otherpos-pos;
    double angle = vec1.normalizeAngle(vec1.getDirection() -vec2.getDirection());

    VecPosition otherctrlpos(otherctrl_->scenePos().x(), otherctrl_->scenePos().y());
    VecPosition vec3 = otherctrlpos-otherpos;

    double a = vec3.getY() * (pos.getX() -otherpos.getX()) - vec3.getX() * (pos.getY() -otherpos.getY());
    double b = vec3.getY() * (movpos.getX() -otherpos.getX()) - vec3.getX() * (movpos.getY() -otherpos.getY());


    if (a * b < 0) {
        adjust_= g_control_point_size/scale_factor_;
        tip_ = tr("警告");
    } else if (angle>90 || angle<-90){
        adjust_= g_control_point_size/scale_factor_;
        tip_ = tr("警告");
        //QMessageBox::warning(0,tr(""),tr(""),QMessageBox::Ok);
    } else{
        adjust_= 2/scale_factor_;
        tip_ = "";
    }
#endif
    QGraphicsItem::mouseMoveEvent(event);
}

QVariant ItemControlPoint::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if ((change == QGraphicsItem::ItemPositionChange) || 
        (change == QGraphicsItem::ItemPositionHasChanged)) {
        edge_->adjust();
        edge_->intWeight();
    }

    return QGraphicsItem::itemChange(change, value);
}

void ItemControlPoint::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = true;
    update();
}

void ItemControlPoint::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = false;
    update();
}
