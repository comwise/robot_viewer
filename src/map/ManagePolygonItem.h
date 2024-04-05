#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_POLYGON_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_POLYGON_ITEM__H__

#include "ManageItemBase.h"
#include "ItemPolygon.h"

class RobotViewerScene;

class ManagePolygonItem : public ManageItemBase<ItemPolygon>
{
public:
    ManagePolygonItem();
    virtual ~ManagePolygonItem() {}

    void SetSence(QGraphicsScene *sce);
    void SetDefaultDesc(const QString &desc);
    
    ItemPolygon *add(const QPolygonF &points);
    ItemPolygon *add(int id, const QPolygonF &points);
    void remove(ItemPolygon *item);
    void remove(int id);
    void removeAll();

    bool setPolygon(int id, const QPolygonF &points);
    bool setPolygon(const QVector<PolygonData> &data);
    bool getPolygon(QVector<QPolygonF> &points);
    bool getPolygon(QVector<PolygonData> &data);

    ItemPolygon *find(const QPointF &pos);
    ItemPolygon *find(int id);

private:
    RobotViewerScene *scene_{nullptr};
    QString default_desc_;
};

#endif //__COMWISE__ROBOT_VIEWER__MANAGE_POLYGON_ITEM__H__
