#ifndef __COMWISE__ROBOT_VIEWER__ITEM_REFLECTOR__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_REFLECTOR__H__

#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

class ItemReflector : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_REFLECTOR };
    int type() const { return Type; }

public:
    explicit ItemReflector(const QString &txt);
    virtual ~ItemReflector() { }

    void setDetect(int detect);

protected:    
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

private:
    QString desc_;
    double shape_r_{25.0};
    int detect_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_REFLECTOR__H__
