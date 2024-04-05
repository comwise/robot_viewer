#ifndef __COMWISE__ROBOT_VIEWER__ITEM_TRAFFIC_AREA__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_TRAFFIC_AREA__H__

#include <QGraphicsRectItem>
#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneHoverEvent;
QT_END_NAMESPACE
class ItemArea;
class RobotViewerScene;

class CornerItem : public QGraphicsRectItem
{
public:
    enum { Type = GraphicsItemType::ITEM_ARAECORNER };
    int type() const { return Type; }
    
    enum CornerType {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT
    };

public:
    CornerItem(ItemArea *area_item);

    void SetType(int type) { type_ = type; }
    int GetType() { return type_; }

    void SetPos(const QPointF &point);

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    ItemArea *area_item_{nullptr};
    int type_{BOTTOM_RIGHT};
    bool set_pos_enable_{false};
};

class ItemArea : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_ARAE };
    int type() const { return Type; }
    
public:
    ItemArea(QPointF top_left, QPointF bottom_right, RobotViewerScene *sce);
    ItemArea(const AreaData &area_data, RobotViewerScene *sce);
    ~ItemArea();

    virtual void setID(unsigned int id) override;
    virtual unsigned int getID() const override;

    void SetAreaData(const AreaData &area_data);
    void SetAreaData(const QPointF &top_left, const QPointF& bottom_right);
    AreaData GetAreaData();
    void AreaUpdate();

    void setDesc(const QString &desc);

    void CornerItemShow(bool show);
    void CornerChange(CornerItem *item);
    void DrawCorner(QRectF &rect);

    void SetTopLeft(QPointF pos);
    void SetBottomRight(QPointF pos);
    
    std::vector<CornerItem*> GetCornerItem();

protected:
    //draw
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //event
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    void ItemInit();

private:
    std::pair<CornerItem *, CornerItem *> catercorner_1_items_;
    std::pair<CornerItem *, CornerItem *> catercorner_2_items_;

    CornerItem *top_left_item_{nullptr};
    CornerItem *top_right_item_{nullptr};
    CornerItem *bottom_left_item_{nullptr};
    CornerItem *bottom_right_item_{nullptr};

    QPointF top_left_;
    QPointF bottom_right_;
    AreaData area_data_;

    RobotViewerScene *scene_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_TRAFFIC_AREA__H__
