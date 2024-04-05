#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_MAP_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_MAP_ITEM__H__

#include "ManageItemBase.h"
#include "ItemMap.h"

class RobotViewerScene;

class ManageMapItem : public ManageItemBase<ItemMap>
{
public:
    ManageMapItem();

    ItemMap *addMapItem(const MapData &item);
    bool removeMapItem();

    void SetSence(QGraphicsScene *sce);
    void SetItemVisible(bool is_visible);

private:
    RobotViewerScene *scene_{nullptr};
    QString default_desc;
    ItemMap *map_item_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__MANAGE_MAP_ITEM__H__