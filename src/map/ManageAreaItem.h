#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_AREA_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_AREA_ITEM__H__

#include "ManageItemBase.h"
#include "ItemArea.h"
#include "common/algorithm.h"

class ItemArea;
struct AreaData;
class RobotViewerScene;

class ManageAreaItem : public ManageItemBase<ItemArea>
{
public:
    ManageAreaItem();
    virtual ~ManageAreaItem() {}

    void SetSence(QGraphicsScene *sce);
    void SetDefaultDesc(const QString &desc);
    
    ItemArea *addAreaItem(const QPointF &top_pos, const QPointF &bottom_pos);
    ItemArea *addAreaItem(const AreaData &data);
    void addAreaItems(std::vector<AreaData> &data);
    void removeAreaItem(ItemArea *area);

    bool getAreaDatas(std::vector<AreaData> &data);
    
    ItemArea *findArea(int area_id);
    bool findArea(const QPointF &pos);

private:
    RobotViewerScene *scene_{nullptr};
    QString default_desc_;
};

#endif //__COMWISE__ROBOT_VIEWER__MANAGE_AREA_ITEM__H__
