#include "ManageNodeItem.h"
#include "ManageEdgeItem.h"
#include "ItemNode.h"
#include "ItemEdge.h"
#include "RobotViewerScene.h"

ManageNodeItem::ManageNodeItem()
{
}

void ManageNodeItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

ItemNode *ManageNodeItem::addNodeItem(QPointF pos)
{
    ItemNode *node = new ItemNode();
    if (node) {
        AddItem(node);
        node->setPos(pos);
        node->setID(ManageItemBase::GetUnusedId());
    }
    return node;
}

ItemNode *ManageNodeItem::addNodeItem(const NodeData &nodedata)
{
    if (findNode(nodedata.id)) {
        return NULL;
    }
    ItemNode *node = new ItemNode();
    if (node) {
        node->setNodeData(nodedata);
        AddItem(node);
        node->setPos(QPointF(nodedata.pos.x, nodedata.pos.y));
    }
    return node;
}

void ManageNodeItem::addNodeItems(std::vector<NodeData> &nodedatas)
{
    for (std::size_t i = 0; i < nodedatas.size(); i++) {
        addNodeItem(nodedatas[i]);
    }
}

void ManageNodeItem::removeNodeItem(ItemNode *node)
{
    if (nullptr == node || nullptr == edge_manage_) {
        return;
    }
    QList<ItemEdge *> edges = node->getEdges();
    foreach (ItemEdge *edge, edges) {
        edge_manage_->removeEdgeItem(edge);
    }
    DelItem(node);
}

ItemNode *ManageNodeItem::findNode(int node_id)
{
    foreach (ItemNode *item, items_) {
        if (item && item->getID() == node_id) {
            return item;
        }
    }
    return NULL;
}

ItemNode *ManageNodeItem::findNode(QPointF pos)
{
    ItemNode *node = NULL;
    if (scene_) {
        QList<QGraphicsItem *> itemALL = scene_->items(pos);
        node = findNodeInItems(itemALL);
    }
    return node;
}

bool ManageNodeItem::getNodeDatas(std::vector<NodeData> &nodedatas)
{
    foreach (ItemNode *nodeitem, items_) {
        if(nodeitem) {
            NodeData nodedata = nodeitem->getNodeData();
            nodedatas.push_back(nodedata);
        }
    }
    return true;
}

ItemNode *ManageNodeItem::findNodeInItems(const QList<QGraphicsItem *> &items)
{
    QGraphicsItem *item = ManageItemBase::FindItemInItems(items, GraphicsItemType::ITEM_NODE);
    return qgraphicsitem_cast<ItemNode *>(item);
}

void ManageNodeItem::SetEdgeManage(ManageEdgeItem *edge_manage)
{
    edge_manage_ = edge_manage;
}

QList<QLineF> ManageNodeItem::GetNodeAlignLine(QPointF mousePos, double AxisDis)
{
    QList<QLineF> lineList;
    QList<ItemNode *> nList = GetItems();
    for (int i = 0; i < nList.size(); i++) {
        if ((fabs(nList[i]->x() - mousePos.x()) < AxisDis)) {
            QLineF line(nList[i]->x(), nList[i]->y(), nList[i]->x(),
                        mousePos.y());
            lineList << line;
            continue;
        }
        if ((fabs(nList[i]->y() - mousePos.y()) < AxisDis)) {
            QLineF line(nList[i]->x(), nList[i]->y(), mousePos.x(),
                        nList[i]->y());
            lineList << line;
            continue;
        }
    }
    return lineList;
}

bool ManageNodeItem::GetAlignNodePos(QPointF &mousePos, double AxisDis)
{
    QList<ItemNode *> nList = GetItems();
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
