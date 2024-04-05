#ifndef __COMWISE__ROBOT_VIEWER__ITEM_CONTROL_POINT__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_CONTROL_POINT__H__

#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;
QT_END_NAMESPACE
class ItemEdge;
class ItemNode;

class ItemControlPoint : public ItemBase
{
    Q_OBJECT
public:
    enum { Type = GraphicsItemType::ITEM_CONTROLPOINT };
    int type() const { return Type; }
    
public:
    ItemControlPoint(ItemEdge *edge, ItemNode *node);

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    double shape_r_;
    QString tip_;
    double adjust_;
    bool hover_enter_;
    ItemEdge *edge_;
    ItemNode *node_;
    double scale_factor_;
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_CONTROL_POINT__H__
