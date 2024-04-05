#ifndef __COMWISE__ROBOT_VIEWER__ITEM_POLYGON__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_POLYGON__H__

#include <QGraphicsPolygonItem>
#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE
class RobotViewerScene;

class ItemPolygon : public QGraphicsPolygonItem
{
public:
    enum { Type = GraphicsItemType::ITEM_POLYGON };
    int type() const { return Type; }
    
public:
    explicit ItemPolygon(RobotViewerScene *sce);
    virtual ~ItemPolygon();

    int getID() const;
    void setID(int id);

    void setDesc(const QString &desc);
    QString getDesc() const;

    void setData(const PolygonData &data);
    PolygonData getData();

    void setStorage(int area_id, int storage_id);
    void getStorage(int &area_id, int &storage_id);

protected:
    //painter
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    //menu
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    RobotViewerScene *scene_{nullptr};
    int id_{0};
    PolygonData data_;
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_POLYGON__H__
