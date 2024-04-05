#ifndef __COMWISE__ROBOT_VIEWER__ITEM_ROUTER__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_ROUTER__H__

#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

class ItemRouter : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_LINE };
    int type() const { return Type; }

public:
    explicit ItemRouter(const QPointF &source, const QPointF &fst,
            const QPointF &scend, const QPointF &dest);
    virtual ~ItemRouter() { }

    void SetPoint(const QPointF &source, const QPointF &fst, 
            const QPointF &scend, const QPointF &dest_pos);

protected:
    QPainterPath shape() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPointF sourcenode_pos_;
    QPointF fstctrlpt_pos_;
    QPointF secctrlpt_pos_;
    QPointF destnode_pos_;
    double scale_factor_{1.0};
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_ROUTER__H__
