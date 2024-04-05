#ifndef  __COMWISE__ROBOT_VIEWER__ITEM_AGV_POS__H__
#define  __COMWISE__ROBOT_VIEWER__ITEM_AGV_POS__H__

#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QImage;
QT_END_NAMESPACE

class ItemAgv : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_AGVPOS };
    int type() const { return Type; }
    
public:
    ItemAgv(QGraphicsObject *parent = 0);

    void setAgvSize(qreal size);

protected:
    //painter
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //event
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QImage *agv_image_{nullptr};
    qreal agv_size_{0.0};
    bool hover_enter_{false};
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_AGV_POS__H__