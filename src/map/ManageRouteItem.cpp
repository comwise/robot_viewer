#include "ManageRouteItem.h"
#include "ManageEdgeItem.h"
#include "ManageSiteItem.h"
#include "ItemRouter.h"
#include "ItemEdge.h"
#include "ItemSite.h"
#include "RobotViewerScene.h"
#include "ItemNode.h"
#include "RobotViewerData.h"

ManageRouteItem::ManageRouteItem()
    : current_upl_(new UPL)
{
    change_enable_ = false;
}

void ManageRouteItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

void ManageRouteItem::SetEdgeManage(ManageEdgeItem *edge_manage)
{
    edge_manage_ = edge_manage;
}

void ManageRouteItem::SetSiteManage(ManageSiteItem *site_manage)
{
    site_manage_ = site_manage;
}

void ManageRouteItem::AddRouteItems(const std::vector<UPL> &upls)
{
    DelRouteItems();
    if (upls.empty()) {
        return;
    }
    current_route_ = AddRouteItem(upls[0]);
    route_upls_.clear();
    route_upls_.push_back(upls[0]);
    for (std::size_t i = 1; i < upls.size(); ++i)
    {
        route_upls_.push_back(upls[i]);
        AddRouteItem(upls[i]);
    }
    change_enable_ = true;
}

ItemRouter *ManageRouteItem::AddRouteItem(const UPL &upl)
{
    ItemEdge *bezier = edge_manage_->findEdge(upl.start_id, upl.end_id);
    if (NULL == bezier)
        return NULL;

    QPointF source_point;
    QPointF fst_control;
    QPointF sce_control;
    QPointF dest_control;
    if (bezier->sourceNode())
        source_point = bezier->sourceNode()->pos();
    fst_control = bezier->getFirstCtrlPtPos();
    sce_control = bezier->getSecCtrlPtPos();
    dest_control = bezier->destNode()->pos();
    ItemRouter *item = new ItemRouter(source_point, fst_control, sce_control, dest_control);
    item->setID(bezier->getID());
    AddItem(item);
    return item;
}

void ManageRouteItem::DelRouteItems()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ClearItems();
    current_route_ = NULL;
    change_enable_ = false;
}

void ManageRouteItem::ChangeRouteItem()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!change_enable_)
    {
        return;
    }
    UPL current_upl = *current_upl_;
    ItemEdge *bezier = edge_manage_->findEdge(current_upl.start_id, current_upl.end_id);
    if (NULL == bezier || current_route_ == NULL)
    {
        return;
    }
    if (current_route_ && bezier->getID() != current_route_->getID())
    {
        if (current_route_)
        {
            DelItem(current_route_);
            current_route_ = NULL;
            route_upls_.pop_front();
        }
        current_route_ = findRouteItem(bezier->getID());
        if (!current_route_)
        {
            // 找不到曲线,报错
        }
    }
    else
    {
        QPointF source_point;
        QPointF dest_control;
        std::pair<QPointF, QPointF> fst, sec;
        if (items_.size() == 1)
        {
            //最后一条边，不做处理;
            return;
        }
        /*if (current_upl.forward_)
        {
            int startid = current_upl.start_id;
            current_upl.start_id = current_upl.end_id;
            current_upl.end_id = startid;
            current_upl.percent = 1-current_upl.percent;
        }*/
        if (bezier->sourceNode()->getID() == route_upls_.front().start_id)
        {
            if (current_upl.start_id != route_upls_.front().start_id)
            {
                current_upl.percent = 1 - current_upl.percent;
            }
            source_point = bezier->getControlPoint(current_upl.percent, fst, sec);
            dest_control = bezier->destNode()->pos();
            current_route_->SetPoint(source_point, sec.first, sec.second, dest_control);
        }
        else
        {
            if (current_upl.start_id != route_upls_.front().start_id)
            {
                current_upl.percent = 1 - current_upl.percent;
            }
            source_point = bezier->getControlPoint(1 - current_upl.percent, fst, sec);
            dest_control = bezier->sourceNode()->pos();
            current_route_->SetPoint(source_point, fst.first, fst.second, dest_control);
        }
    }
}

ItemRouter *ManageRouteItem::findRouteItem(int id)
{
    foreach (ItemRouter *item, items_)
    {
        if (item->getID() == id)
        {
            return item;
        }
    }
    return NULL;
}

void ManageRouteItem::AddTarget(QPointF pos)
{
    QList<ItemSite *> sites = site_manage_->GetSelectItems();
    if (!sites.empty() && target_list_.back() == sites[0])
    {
        return;
    }
    if (sites.empty())
    {
        ItemSite *site = site_manage_->addSiteItem(pos);
        if (!site)
        {
            return;
        }
        add_site_list_.push_back(site);
        sites.push_back(site);
    }
    target_list_ << sites[0];
    pos = sites[0]->pos();
    QPen pen(QColor(255, 0, 0));
    pen.setWidth(2);
    QBrush brush(QColor(255, 255, 128));
    double r = 30;
    QGraphicsEllipseItem *targetItem = scene_->addEllipse(pos.x() - r, pos.y() - r, 2 * r, 2 * r, pen, brush);
    targetItem->setZValue(-2);
    target_item_list_.append(targetItem);
}

std::vector<UPL> ManageRouteItem::getTargetList()
{
    std::vector<UPL> dest_upls;
    foreach (ItemSite *item, target_list_)
    {
        UPL dest_upl;
        dest_upl.start_id = item->getSouceNodeID();
        dest_upl.end_id = item->getDestNodeID();
        dest_upl.forward = !item->getDirection();
        dest_upl.percent = item->getPercentOfEdge();
        dest_upls.push_back(dest_upl);
    }
    return dest_upls;
}

void ManageRouteItem::setCurrentUPL(const UPL &current_upl)
{
    std::lock_guard<std::mutex> lock(mutex_);
    *current_upl_ = current_upl;
}

void ManageRouteItem::DelTarget()
{
    foreach (ItemSite *item, add_site_list_)
    {
        site_manage_->removeSiteItem(item);
    }
    foreach (QGraphicsEllipseItem *item, target_item_list_)
    {
        scene_->removeItem(item);
        delete item;
        item = NULL;
    }
    target_item_list_.clear();
    target_list_.clear();
    add_site_list_.clear();
}
