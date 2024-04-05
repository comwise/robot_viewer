#ifndef __COMWISE__ROBOT_VIEWER__ITEM_EDGE__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_EDGE__H__

#include <QList>
#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneHoverEvent;
QT_END_NAMESPACE

class ItemNode;
class ItemSite;
class ItemControlPoint;

class ItemEdge : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_EDGE };
    int type() const { return Type; }
    
public:
    ItemEdge(ItemNode *source, ItemNode *dest);
    ~ItemEdge();

    virtual unsigned int getID() const override;
    virtual void setID(unsigned int id) override;

    QPainterPath getPath();
    QPainterPath shape() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawEdgeCommon(QPainter *painter);
    void drawEdgeArrow(QPainter *painter);

    EdgeData &getEdgeData();
    void setEdgeData(const EdgeData &edgedata);

    //设置、读取属性
    ItemNode *sourceNode() const;
    ItemNode *destNode() const;

    int getDirection();
    void setDirection(int direction);
    double getEdgeAngle();

    int getBackwardable();
    void setBackwardable(int backwardable);

    double getWeight();
    void setWeight(double weight);
    void intWeight();

    int getEdgePriority();
    void setEdgePriority(int pro);

    double getEdgeSpeed();
    void setEdgeSpeed(double speed);

    // control point
    void setControlPointFlag(QGraphicsItem::GraphicsItemFlag flag, bool enable);
    void setControlPointSelect(bool select);
    void selectControlPoint();

    void showControlPoint(void);
    void hideControlPoint(void);

    QPointF getFirstCtrlPtPos();
    void setFirstCtrlPtPos(QPointF pos);
    QPointF getSecCtrlPtPos();
    void setSecCtrlPtPos(QPointF pos);

    ItemControlPoint *getFirstControlPoint();
    ItemControlPoint *getSecControlPoint();

    QPointF getControlPoint(double percent, 
        std::pair<QPointF, QPointF> &first, std::pair<QPointF, QPointF> &second);
    void getControlPoint(double a, QPointF P0, QPointF P1, QPointF P2, QPointF P3, 
        QPointF Q0, QPointF Qa, QPointF Q3, QPointF &first, QPointF &second);
    
    //在线上增加任务点操作
    void addSiteToEdge(ItemSite *site);
    void removeSiteFromEdge(ItemSite *site);
    void removeSites();
    QList<ItemSite *> getSiteFromEdge() const;

    void adjust();
    void removeFromNode();
    double getNearestPosPercent(const QPointF &pos, QPointF &nearest_pos);
    QList<ItemSite *> sortSitesByPercent();
    void flushSitesPriority();

    void setDefaultBank(int tim_bank, int sz_bank);
    void setDefaultSensor(double slow, double stop);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    // QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    void calculateArrow(QPolygonF &polygon, QPointF &arrowendnear, QPointF &arrowend);
    QPointF initfirstCtrlPtPos(ItemNode *srcNode, ItemNode *desNode);
    QPointF initsecCtrlPtPos(ItemNode *srcNode, ItemNode *desNode);
    void initCtrlPt(QPointF fstPos, QPointF secPos);

private:
    QPointF sourcenode_pos_;
    QPointF destnode_pos_;
    QPointF fstctrlpt_pos_;
    QPointF secctrlpt_pos_;
    QPainterPath path_;

    EdgeData edge_data_;
    ItemNode *source_node_;
    ItemNode *dest_node_;
    ItemControlPoint *fst_ctrlpt_;
    ItemControlPoint *sec_ctrlpt_;
    QList<ItemSite *> sitelist_;
    bool hover_enter_;
    const static int STROKER_WIDTH_ = 3;
    double scale_factor_;
};

#endif //__COMWISE__ROBOT_VIEWER__ITEM_EDGE__H__
