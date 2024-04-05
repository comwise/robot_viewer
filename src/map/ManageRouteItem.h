#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_ROUTE_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_ROUTE_ITEM__H__

#include <vector>
#include <thread>
#include <mutex>
#include "ManageItemBase.h"

class ItemRouter;
class RobotViewerScene;
class ManageEdgeItem;
class ManageSiteItem;
class UPL;
class RobotViewerNavigation;
class ItemSite;

class ManageRouteItem : public ManageItemBase<ItemRouter>
{
public:
    void SetSence(QGraphicsScene *sce);

public:
    ManageRouteItem();
    void SetEdgeManage(ManageEdgeItem *edge_manage);
    void SetSiteManage(ManageSiteItem *site_manage);
    void AddTarget(QPointF pos);
    std::vector<UPL> getTargetList();
    void DelTarget();

    void AddRouteItems(const std::vector<UPL> &upls);
    ItemRouter *AddRouteItem(const UPL &upl);
    void DelRouteItems();
    void setCurrentUPL(const UPL &current_upl);
    void ChangeRouteItem();
    ItemRouter *findRouteItem(int id);

private:
    ManageEdgeItem *edge_manage_{nullptr};
    ManageSiteItem *site_manage_{nullptr};
    QList<ItemSite *> target_list_;
    QList<ItemSite *> add_site_list_;
    QList<QGraphicsEllipseItem *> target_item_list_;
    ItemRouter *current_route_{nullptr};
    std::list<UPL> route_upls_;
    RobotViewerScene *scene_{nullptr};
    UPL *current_upl_{nullptr};
    bool change_enable_{false};
    std::mutex mutex_;
};

#endif // __COMWISE__ROBOT_VIEWER__MANAGE_ROUTE_ITEM__H__
