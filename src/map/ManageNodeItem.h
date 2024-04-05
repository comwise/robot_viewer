#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_NODE_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_NODE_ITEM__H__

#include "ManageItemBase.h"
#include "ItemNode.h"

class RobotViewerScene;
class ItemNode;
class ManageEdgeItem;
struct NodeData;

class ManageNodeItem : public ManageItemBase<ItemNode>
{
public:
    ManageNodeItem();

    void SetSence(QGraphicsScene *sce);

    void SetEdgeManage(ManageEdgeItem *edge_manage);

    ItemNode *addNodeItem(QPointF pos);
    ItemNode *addNodeItem(const NodeData &nodedate);
    void removeNodeItem(ItemNode *node);
    
    ItemNode *findNode(int node_id);
    ItemNode *findNode(QPointF pos);
    ItemNode *findNodeInItems(const QList<QGraphicsItem *> &items);

    QList<QLineF> GetNodeAlignLine(QPointF mousePos, double AxisDis);
    bool GetAlignNodePos(QPointF &mousePos, double AxisDis);
    bool getNodeDatas(std::vector<NodeData> &nodedatas);
    void addNodeItems(std::vector<NodeData> &nodedatas);

private:
    RobotViewerScene *scene_;
    ManageEdgeItem *edge_manage_;
};

#endif // __COMWISE__ROBOT_VIEWER__MANAGE_NODE_ITEM__H__