#include "ItemAgv.h"
#include <QtGui>
#include <QImage>
#include "RobotViewerLog.h"
#include "RobotViewerData.h"

static const int kAgvSize = 16;

ItemAgv::ItemAgv(QGraphicsObject *parent)
 : ItemBase(GraphicsItemType::ITEM_AGVPOS, parent)
 , agv_image_(NULL)
 , agv_size_(kAgvSize/scale())
 , hover_enter_(false)
{
    setCacheMode(DeviceCoordinateCache);
    setZValue(50);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void ItemAgv::setAgvSize(qreal agvSize)
{
    agv_size_ = agvSize/scale();
}

QRectF ItemAgv::boundingRect() const
{
    QRectF rect(-2*agv_size_, -2*agv_size_ , 4*agv_size_, 4*agv_size_);
    return rect;
}

QPainterPath ItemAgv::shape() const
{
    QPainterPath path;
    path.addRect(-2*agv_size_, -2*agv_size_ , 4*agv_size_, 4*agv_size_);
    return path;
}

void ItemAgv::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    double shape_r = agv_size_;
    if (hover_enter_) {
        shape_r = 2*agv_size_;
    }

    QRadialGradient gradient(0, 0, 2*shape_r);
    gradient.setColorAt(1, QColor(Qt::green));
    gradient.setColorAt(0, QColor(Qt::green));
    painter->setPen(QPen(Qt::black, 0));
    painter->setBrush(gradient);
    painter->drawEllipse(-2*shape_r, -2*shape_r,  4*shape_r, 4*shape_r);
    painter->setBrush(QBrush(QColor(255,0,128,150)));
    painter->drawPolygon(QPolygonF()
        << QPointF(shape_r, 0)
        << QPointF(-0.6*shape_r,  1*shape_r)
        << QPointF(0, 0)
        << QPointF(-0.6*shape_r, -1*shape_r));

#if 0
    if (painter && agv_image_) {
        painter->setTransform(QTransform(QMatrix(1, 0, 0, 1, 0, 0.0)), true);
        QRectF rotateRect(-2*agv_size_, -agv_size_, 4*agv_size_, 2*agv_size_);
        painter->drawImage(rotateRect, *agv_image_);
    }
#endif
}

void ItemAgv::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
    hover_enter_ = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void ItemAgv::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
    hover_enter_ = false;
    QGraphicsItem::hoverLeaveEvent(event);
}
