#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_EDGE_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_EDGE_ITEM__H__

#include "ManageItemBase.h"
#include "ItemEdge.h"

class ItemNode;
class ManageNodeItem;
class ManageSiteItem;
class RobotViewerScene;
struct EdgeData;

class ManageEdgeItem : public ManageItemBase<ItemEdge>
{
public:
    struct NearPos
    {
        QPointF pos_near_;
        double percent_near_;
        double dis_min_;
        NearPos() : percent_near_(0.0), dis_min_(0.0) {}
    };

    struct NearEdgeFromSite
    {
        QPointF pos_near_;
        double percent_near_;
        ItemEdge *edge_near_;
        double dis_min_;

        NearEdgeFromSite() : percent_near_(0.0), edge_near_(NULL), dis_min_(0.0) {}
    };

public:
    void SetSence(QGraphicsScene *sce);
    void SetItemsFlag(QGraphicsItem::GraphicsItemFlag flag, bool enable);
    void SetItemsSelected(bool select);

public:
    ManageEdgeItem();

    void SetNodeManage(ManageNodeItem *node_manage);
    void SetSiteManage(ManageSiteItem *site_manage);

    ItemEdge *addEdgeItem(const EdgeData &edgedate);
    ItemEdge *addEdgeItem(EdgeData &edgedate);
    ItemEdge *addEdgeItem(ItemNode *src, ItemNode *des);
    // ItemEdge* addEdgeItem(QPointF src,QPointF fst,QPointF sce,QPointF dest);

    void addEdgeItems(const std::vector<EdgeData> &edgedatas);
    void removeEdgeItem(ItemEdge *edge);

    std::pair<ItemEdge *, ItemEdge *> SpitEdge(
        ItemEdge *split_edge, ItemNode *split_node, double percent);

    ItemEdge *findEdgeInItems(const QList<QGraphicsItem *> &items);
    ItemEdge *findEdge(int edge_id);
    ItemEdge *findEdge(int souce_id, int dest_id); //根据两边节点号查边

    ItemEdge *GetNearestEdge(const QList<ItemEdge *> &edges, QPointF pos);
    NearEdgeFromSite GetNearestEdge(const QPointF &pos);
    bool getEdgeData(int edge_id, EdgeData &edge_data);
    bool getEdgeDatas(std::vector<EdgeData> &edgedatas);

    void ShowEdgeControlPoint(QList<ItemEdge *> &edges, bool show);
    void setDefaultBank(int tim_bank, int sz_bank);
    void setDefaultSensor(double slow, double stop);

private:
    RobotViewerScene *scene_;
    ManageSiteItem *site_manage_;
    int default_tim_bank_;
    int default_sz_bank_;
    double global_slow_;
    double global_stop_;
};

#endif // __COMWISE__ROBOT_VIEWER__MANAGE_EDGE_ITEM__H__
