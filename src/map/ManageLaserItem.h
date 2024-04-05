#ifndef __COMWISE__ROBOT_VIEWER__MANGAE_LASER_ITEMS__H__
#define __COMWISE__ROBOT_VIEWER__MANGAE_LASER_ITEMS__H__

#include <thread>
#include <mutex>
#include <QList>
#include "ItemBase.h"
#include "ManageItemBase.h"

class RobotViewerScene;
class GReflector;
class GLaserScan;
class ItemReflector;
class Glaser3DBeam;
class QGraphicsScene;
class QGraphicsRectItem;

class ManageLaserItem : public ManageItemBase<ItemBase>
{
public:
    ManageLaserItem();

    void SetConfig(double laser_dist, double laser_head);
    void SetSecne(QGraphicsScene *scene);
    void setAgvPos(const OrientPos &agv_pos);
    void setReflector(const GReflector &reflector);
    void setLaserScan(const QPolygonF &laser_scan);
    void setLaserScan3D(const Glaser3DBeam &laser);
    void SetLaserVisible(bool flag);

    void UpdateItems();

private:
    void DrawLaserScan(const QPolygonF &curr_scan);
    void DrawLaserScan3D(const Glaser3DBeam &curr_scan);
    void DrawReflector(const GReflector &reflector);
    void ClearReflector();

private:
    double laser_heading_;
    double laser_dist_;

    std::mutex agv_pos_mutex_;
    OrientPos scene_robotpos_;

    std::mutex reflector_mutex_;
    GReflector *reflector_;
    bool reflector_change_;

    std::mutex laser_scan_mutex_;
    QPolygonF laser_scan_;
    bool laser_scan_change_;

    std::mutex laser_scan_3d_mutex_;
    Glaser3DBeam *laser_scan_3d_;
    bool laser_scan_3d_change_;

    QGraphicsScene *scene_;
    QList<QGraphicsRectItem *> laserscan_item_list_;
    QList<ItemReflector *> reflector_item_list_;

    bool is_visible_;
};

#endif //__COMWISE__ROBOT_VIEWER__MANGAE_LASER_ITEMS__H__
