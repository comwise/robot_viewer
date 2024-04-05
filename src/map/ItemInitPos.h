#ifndef  __COMWISE__ROBOT_VIEWER__ITEM_INIT_POS__H__
#define  __COMWISE__ROBOT_VIEWER__ITEM_INIT_POS__H__

#include "ItemBase.h"

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QImage;
class QGraphicsScene;

class ItemInitPos : public ItemBase
{
public:
	enum { Type = GraphicsItemType::ITEM_INITPOS };
    int type() const { return Type; }

public:
    ItemInitPos(QGraphicsScene *sce);
    ~ItemInitPos();

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void init();
    void uninit();

private:
    static const int kInitWidth = 500;
    static const int kInitHeight = 500;

    QImage *init_image_{nullptr};
    QGraphicsScene *scene_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_INIT_POS__H__