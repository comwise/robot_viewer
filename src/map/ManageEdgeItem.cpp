#include "ManageEdgeItem.h"
#include "RobotViewerScene.h"
#include "ItemSite.h"
#include "ItemNode.h"
#include "ItemEdge.h"
#include "ItemSite.h"
#include "ManageNodeItem.h"
#include "ManageSiteItem.h"
#include "RobotViewerData.h"

ManageEdgeItem::ManageEdgeItem()
    : global_slow_(0.0)
    , global_stop_(0.0)
{

}

void ManageEdgeItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

void ManageEdgeItem::SetItemsFlag(QGraphicsItem::GraphicsItemFlag flag, bool enable)
{
    foreach (ItemEdge *bezier, items_) {
        if (bezier) {
            bezier->setControlPointFlag(flag, enable);
        }
    }
}

void ManageEdgeItem::SetItemsSelected(bool select)
{
    foreach (ItemEdge *bezier, items_) {
        if (bezier) {
            bezier->setControlPointSelect(select);
            bezier->setSelected(select);
        }
    }
}

ItemEdge *ManageEdgeItem::addEdgeItem(const EdgeData &edgedata)
{
    if (findEdge(edgedata.id)) {
        return nullptr;
    }
    if(!scene_) {
        return nullptr;
    }
    ItemNode *src = scene_->GetNodeManage()->findNode(edgedata.startid);
    ItemNode *des = scene_->GetNodeManage()->findNode(edgedata.endid);
    if (src == nullptr || des == nullptr) {
        return nullptr;
    }

    ItemEdge *edge = new ItemEdge(src, des);
    if(edge && scene_) {
        edge->setEdgeData(edgedata);
        edge->setFirstCtrlPtPos(QPointF(edgedata.ctrlpos1.x, edgedata.ctrlpos1.y));
        edge->setSecCtrlPtPos(QPointF(edgedata.ctrlpos2.x, edgedata.ctrlpos2.y));
        edge->setDefaultBank(default_tim_bank_, default_sz_bank_);
        edge->setDefaultSensor(global_slow_, global_stop_);
        edge->adjust();
        AddItem(edge);
        scene_->emitEdgeChanged(edge->getEdgeData(), 0);
    }
    return edge;
}

ItemEdge *ManageEdgeItem::addEdgeItem(EdgeData &edgedata)
{
    edgedata.id = GetUnusedId();
    const EdgeData &edge_data = edgedata;
    return addEdgeItem(edge_data);
}

ItemEdge *ManageEdgeItem::addEdgeItem(ItemNode *src, ItemNode *des)
{
    foreach (ItemEdge *bezier, src->getEdges()) {
        if (!bezier || des == bezier->sourceNode() || des == bezier->destNode())
            return nullptr;
    }
    ItemEdge *edge = new ItemEdge(src, des);
    if(edge && scene_) {
        edge->setID(GetUnusedId());
        edge->setDefaultBank(default_tim_bank_, default_sz_bank_);
        // edge->setDefaultSensor(global_slow_, global_stop_);
        edge->intWeight();
        AddItem(edge);
        scene_->emitEdgeChanged(edge->getEdgeData(), 0);
    }
    return edge;
}

void ManageEdgeItem::addEdgeItems(const std::vector<EdgeData> &edgedatas)
{
    for (std::size_t i = 0; i < edgedatas.size(); i++) {
        addEdgeItem(edgedatas[i]);
    }
}

void ManageEdgeItem::removeEdgeItem(ItemEdge *edge)
{
    if(nullptr == edge) {
        return;
    }
    edge->removeFromNode();
    EdgeData edge_data = edge->getEdgeData();
    QList<ItemSite *> sites = edge->getSiteFromEdge();
    foreach (ItemSite *item, sites) {
        if(site_manage_ && item) {
            site_manage_->removeSiteItem(item);
        }
    }
    DelItem(edge);
    if(scene_) {
        scene_->emitEdgeChanged(edge_data, 1);
    }
}

ItemEdge *ManageEdgeItem::findEdgeInItems(const QList<QGraphicsItem *> &items)
{
    QGraphicsItem *item = FindItemInItems(items, GraphicsItemType::ITEM_EDGE);
    return qgraphicsitem_cast<ItemEdge *>(item);
}

ItemEdge *ManageEdgeItem::findEdge(int edge_id)
{
    foreach (ItemEdge *item, items_) {
        if (item && item->getID() == edge_id) {
            return item;
        }
    }
    return nullptr;
}

ItemEdge *ManageEdgeItem::findEdge(int souce_id, int dest_id)
{
    foreach (ItemEdge *item, items_) {
        if (item && 
            item->sourceNode()->getID() == souce_id && 
            item->destNode()->getID() == dest_id) {
            return item;
        }
        if (item && 
            item->sourceNode()->getID() == dest_id &&
            item->destNode()->getID() == souce_id) {
            return item;
        }
    }
    return nullptr;
}

bool ManageEdgeItem::getEdgeDatas(std::vector<EdgeData> &edgedatas)
{
    foreach (ItemEdge *edgeitem, items_) {
        if (edgeitem) {
            EdgeData edgedata = edgeitem->getEdgeData();
            edgedatas.push_back(edgedata);
        }
    }
    return true;
}

ItemEdge *ManageEdgeItem::GetNearestEdge(const QList<ItemEdge *> &edges, QPointF pos)
{
    ItemEdge *near_edge = nullptr;
    double min_dis(999999);
    foreach (ItemEdge *edgeitem, edges) {
        if (edgeitem) {
            QPointF near_pos;
            edgeitem->getNearestPosPercent(pos, near_pos);
            QLineF line(pos, near_pos);
            if (line.length() < min_dis) {
                min_dis = line.length();
                near_edge = edgeitem;
            }
        }
    }
    return near_edge;
}

ManageEdgeItem::NearEdgeFromSite ManageEdgeItem::GetNearestEdge(const QPointF &pos)
{
    // NearEdgeFromSite near;
#if 0
    double min_dis(999999999);
    foreach(ItemEdge *edgeitem, items_) {
        QPointF near_pos;
        double percent = edgeitem->getNearestPosPercent(pos, near_pos);
        QLineF line(pos, near_pos);
        if (line.length() < min_dis) {
            min_dis = line.length();
            near.edge_near_ = edgeitem;
            near.dis_min_ = min_dis;
            near.percent_near_ = percent;
            near.pos_near_ = near_pos;
        }
    }
#endif
    return NearEdgeFromSite();
}

void ManageEdgeItem::ShowEdgeControlPoint(QList<ItemEdge *> &edges, bool show)
{
    foreach (ItemEdge *edgeitem, edges) {
        if (nullptr == edgeitem) {
            continue;
        }
        if (show) {
            edgeitem->showControlPoint();
        } else {
            edgeitem->hideControlPoint();
        }
    }
}

std::pair<ItemEdge *, ItemEdge *> ManageEdgeItem::SpitEdge(
    ItemEdge *split_edge, ItemNode *split_node, double percent)
{
    if (nullptr == split_edge || nullptr == split_node) {
        std::pair<ItemEdge *, ItemEdge *>(nullptr, nullptr);
    }

    std::pair<QPointF, QPointF> first;
    std::pair<QPointF, QPointF> second;
    split_edge->getControlPoint(percent, first, second);
    EdgeData first_data = split_edge->getEdgeData();
    first_data.endid = split_node->getID();
    first_data.ctrlpos2.x = first.first.x();
    first_data.ctrlpos2.y = first.first.y();
    first_data.ctrlpos1.x = first.second.x();
    first_data.ctrlpos1.y = first.second.y();
    ItemEdge *first_edge = addEdgeItem(first_data);
    EdgeData second_data = split_edge->getEdgeData();
    second_data.startid = split_node->getID();
    second_data.ctrlpos1.x = second.first.x();
    second_data.ctrlpos1.y = second.first.y();
    second_data.ctrlpos2.x = second.second.x();
    second_data.ctrlpos2.y = second.second.y();
    ItemEdge *second_edge = addEdgeItem(second_data);
    if (!first_edge || !second_edge) {
        return std::pair<ItemEdge *, ItemEdge *>(nullptr, nullptr);
    }
    foreach (ItemSite *item, split_edge->getSiteFromEdge()) {
        if (nullptr == item) {
            continue;
        }
        if (item->getPercentOfEdge() < percent) {
            item->setParentEdge(first_edge);
        } else {
            item->setParentEdge(second_edge);
        }
    }
    split_edge->removeSites();
    return std::pair<ItemEdge *, ItemEdge *>(first_edge, second_edge);
}

void ManageEdgeItem::SetSiteManage(ManageSiteItem *site_manage)
{
    site_manage_ = site_manage;
}

void ManageEdgeItem::setDefaultBank(int tim_bank, int sz_bank)
{
    if (tim_bank < -1 || tim_bank > 15) {
        default_tim_bank_ = 0;
    } else {
        default_tim_bank_ = tim_bank;
    }

    if (sz_bank < -1 || sz_bank > 15) {
        default_sz_bank_ = 0;
    } else {
        default_sz_bank_ = sz_bank;
    }
}

bool ManageEdgeItem::getEdgeData(int edge_id, EdgeData &edge_data)
{
    ItemEdge *bezier = findEdge(edge_id);
    if (nullptr == bezier) {
        return false;
    }
    edge_data = bezier->getEdgeData();
    return true;
}

void ManageEdgeItem::setDefaultSensor(double slow, double stop)
{
    global_slow_ = slow;
    global_stop_ = stop;
}
