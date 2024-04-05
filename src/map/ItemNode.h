#ifndef __COMWISE__ROBOT_VIEWER__ITEM_NODE__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_NODE__H__

#include <QList>
#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneHoverEvent;
QT_END_NAMESPACE
class ItemEdge;

class ItemNode : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_NODE };
    int type() const { return Type; }

public:
    ItemNode();
    virtual ~ItemNode();

    //id
    virtual unsigned int getID() const override;
    virtual void setID(unsigned int id) override;

    // node data
    NodeData &getNodeData();
    void setNodeData(const NodeData &node_data);

    // Bezie
    void addEdge(ItemEdge *edge);
    void removeEdge(ItemEdge *edge);
    QList<ItemEdge *> getEdges() const;

    double getPosx() const;
    double getPosy() const;
    void setPosData(const QPointF &pos);
    void setSpin(int val);
    void setTshaped(int val);

protected:
    // draw
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //event
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    //menu
    // void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    void drawCommonNode(QPainter *painter);

private:
    QList<ItemEdge *> edgeList_;
    NodeData node_data_;
    double shape_r;
    bool hover_enter_;
    bool align_;
    bool pressed_;
    double scale_factor_;
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_NODE__H__
