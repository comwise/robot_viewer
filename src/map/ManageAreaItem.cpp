#include "ManageAreaItem.h"
#include "ItemArea.h"
#include "RobotViewerScene.h"
#include <QDebug>

ManageAreaItem::ManageAreaItem()
{

}

void ManageAreaItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

void ManageAreaItem::SetDefaultDesc(const QString &desc)
{
    default_desc_ = desc;
}

ItemArea *ManageAreaItem::addAreaItem(const QPointF &top_pos, const QPointF &bottom_pos)
{
    QRectF rc(top_pos, bottom_pos);
    if (rc.width() < 2 || rc.height() < 2) {
        return nullptr;
    }
    ItemArea *area = new ItemArea(top_pos, bottom_pos, scene_);
    if (area) {
        int id = ManageItemBase::GetUnusedId();
        area->setID(id);
        area->setDesc(default_desc_ + QString::number(id));
        AddItem(area);
    }
    return area;
}

ItemArea *ManageAreaItem::addAreaItem(const AreaData &data)
{
    QPointF top_left(data.rect_data.corner_x, data.rect_data.corner_y);
    QPointF bottom_right(data.rect_data.height, data.rect_data.width);
    ItemArea *area = new ItemArea(top_left, top_left + bottom_right, scene_);
    if (area) {
        area->SetAreaData(data);
        AddItem(area);
    }
    return area;
}

void ManageAreaItem::addAreaItems(std::vector<AreaData> &areadates)
{
    for (std::size_t i = 0; i < areadates.size(); i++) {
        addAreaItem(areadates[i]);
    }
}

void ManageAreaItem::removeAreaItem(ItemArea *area)
{
    DelItem(area);
}

bool ManageAreaItem::getAreaDatas(std::vector<AreaData> &areadates)
{
    foreach (ItemArea *item, items_) {
        if(item) {
            AreaData data = item->GetAreaData();
            areadates.push_back(data);
        }
    }
    return true;
}

ItemArea *ManageAreaItem::findArea(int area_id)
{
    foreach (ItemArea *item, items_) {
        if (item && item->getID() == area_id) {
            return item;
        }
    }
    return nullptr;
}

bool ManageAreaItem::findArea(const QPointF &pos)
{
    foreach (ItemArea *item, items_) {
        if(item && item->contains(pos)) {
            return true;
        }
    }
    return false;
}


