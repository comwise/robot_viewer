#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_SITE_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_SITE_ITEM__H__

#include "ManageItemBase.h"
#include "ItemSite.h"

class ItemSite;
class ItemEdge;
class RobotViewerScene;
struct SiteData;

class ManageSiteItem : public ManageItemBase<ItemSite>
{
public:
    ManageSiteItem();

    void SetSence(QGraphicsScene *sce);
    
    ItemSite *addSiteItem(const SiteData &sitedate);
    ItemSite *addSiteItem(ItemEdge *edge, double percent);
    ItemSite *addSiteItem(ItemEdge *edge, QPointF pos);
    ItemSite *addSiteItem(QPointF pos);
    void addSiteItems(std::vector<SiteData> &sitedatas);

    void removeSiteItem(ItemSite *site);

    ItemSite *findSiteInItems(const QList<QGraphicsItem *> &items);
    ItemSite *findSite(int site_id);
    
    bool getSiteDatas(std::vector<SiteData> &sitedatas);
    QList<QLineF> GetSiteAlignLine(QPointF mousePos, double AxisDis);
    bool GetAlignSitePos(QPointF &mousePos, double AxisDis);

private:
    RobotViewerScene *scene_{nullptr};
};

#endif //__COMWISE__ROBOT_VIEWER__MANAGE_SITE_ITEM__H__