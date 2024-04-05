#include "ManageMapItem.h"
#include "RobotViewerScene.h"

ManageMapItem::ManageMapItem()
    : scene_(NULL), map_item_(NULL)
{
}

void ManageMapItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

void ManageMapItem::SetItemVisible(bool is_visible)
{
    if (map_item_) {
        map_item_->setVisible(is_visible);
    }
}

ItemMap *ManageMapItem::addMapItem(const MapData &map_data)
{
    map_item_ = new ItemMap(map_data, scene_);
    map_item_->setID(ManageItemBase::GetUnusedId());
    AddItem(map_item_);
    return map_item_;
}

bool ManageMapItem::removeMapItem()
{
    return DelItem(map_item_);
}