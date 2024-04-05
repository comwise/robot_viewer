#include "ManageSiteItem.h"
#include "ItemEdge.h"
#include "ManageEdgeItem.h"
#include "RobotViewerScene.h"
#include "ItemSite.h"

ManageSiteItem::ManageSiteItem()
{
}

void ManageSiteItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

ItemSite *ManageSiteItem::addSiteItem(const SiteData &sitedata)
{
    if (findSite(sitedata.id)) {
        return nullptr;
    }
    if(nullptr == scene_ || !scene_->GetEdgeManage()) {
        return nullptr;
    }
    ItemEdge *parentedge = scene_->GetEdgeManage()->findEdge(sitedata.parentedgeid);
    if (parentedge == nullptr) {
        return nullptr;
    }
    ItemSite *siteitem = new ItemSite(parentedge, sitedata.percent, scene_);
    if(nullptr == siteitem) {
        return nullptr;
    }
    siteitem->setSiteData(sitedata);
    siteitem->setPos(QPointF(sitedata.pos.x, sitedata.pos.y));
    AddItem(siteitem);
    scene_->emitSiteChanged(siteitem->getSiteData(), 0);
    return siteitem;
}

ItemSite *ManageSiteItem::addSiteItem(ItemEdge *edge, QPointF pos)
{
    if (edge == nullptr)
        return nullptr;

    QPointF nearpos;
    double percent = edge->getNearestPosPercent(pos, nearpos);
    return addSiteItem(edge, percent);
}

ItemSite *ManageSiteItem::addSiteItem(ItemEdge *edge, double percent)
{
    if (edge == nullptr) {
        return nullptr;
    }
    ItemSite *site = new ItemSite(edge, percent, scene_);
    if(site && scene_) {
        QPainterPath path = edge->getPath();
        site->setPos(path.pointAtPercent(percent));
        site->setID(GetUnusedId());
        AddItem(site);
        scene_->emitSiteChanged(site->getSiteData(), 0);
    }
    return site;
}

ItemSite *ManageSiteItem::addSiteItem(QPointF pos)
{
    if (nullptr == scene_) {
        return nullptr;
    }
    ManageEdgeItem *edge_manage = scene_->GetEdgeManage();
    if(nullptr == edge_manage) {
        return nullptr;
    }
    
    QList<QGraphicsItem *> item_all = scene_->items(pos);
    ItemEdge *bezier = edge_manage->findEdgeInItems(item_all);
    if (bezier) {
        return addSiteItem(bezier, pos);
    }
    return nullptr;
}

void ManageSiteItem::addSiteItems(std::vector<SiteData> &sitedatas)
{
    for (std::size_t i = 0; i < sitedatas.size(); i++) {
        addSiteItem(sitedatas[i]);
    }
}

void ManageSiteItem::removeSiteItem(ItemSite *site)
{
    if(site && scene_) {
        site->removeSiteFromEdge();
        scene_->emitSiteChanged(site->getSiteData(), 1);
        DelItem(site);
    }
}

ItemSite *ManageSiteItem::findSiteInItems(const QList<QGraphicsItem *> &items)
{
    QGraphicsItem *item = FindItemInItems(items, GraphicsItemType::ITEM_SITE);
    return qgraphicsitem_cast<ItemSite *>(item);
}

ItemSite *ManageSiteItem::findSite(int site_id)
{
    foreach (ItemSite *item, items_) {
        if (item && item->getID() == site_id) {
            return item;
        }
    }
    return nullptr;
}

bool ManageSiteItem::getSiteDatas(std::vector<SiteData> &sitedatas)
{
    foreach (ItemSite *siteitem, items_) {
        if(siteitem) {
            SiteData sitedata = siteitem->getSiteData();
            sitedatas.push_back(sitedata);
        }
    }
    return true;
}

QList<QLineF> ManageSiteItem::GetSiteAlignLine(QPointF mousePos, double AxisDis)
{
    QList<QLineF> lineList;
    QList<ItemSite *> nList = GetItems();
    for (int i = 0; i < nList.size(); i++) {
        if ((fabs(nList[i]->x() - mousePos.x()) < AxisDis)) {
            QLineF line(nList[i]->x(), nList[i]->y(), 
                        nList[i]->x(), mousePos.y());
            lineList << line;
            continue;
        }
        if ((fabs(nList[i]->y() - mousePos.y()) < AxisDis)) {
            QLineF line(nList[i]->x(), nList[i]->y(), 
                        mousePos.x(), nList[i]->y());
            lineList << line;
            continue;
        }
    }
    return lineList;
}

bool ManageSiteItem::GetAlignSitePos(QPointF &mousePos, double AxisDis)
{
    QList<ItemSite *> nList = GetItems();
    for (int i = 0; i < nList.size(); i++) {
        if ((fabs(nList[i]->x() - mousePos.x()) < AxisDis)) {
            mousePos.setX(nList[i]->x());
            return true;
        }
        if ((fabs(nList[i]->y() - mousePos.y()) < AxisDis)) {
            mousePos.setY(nList[i]->y());
            return true;
        }
    }
    return false;
}
