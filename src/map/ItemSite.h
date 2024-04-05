#ifndef  __COMWISE__ROBOT_VIEWER__ITEM_SITE__H__
#define  __COMWISE__ROBOT_VIEWER__ITEM_SITE__H__

#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QStyleOptionGraphicsItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QGraphicsSceneHoverEvent;
class QWidget;
class QMenu;
QT_END_NAMESPACE

class ItemEdge;
class RobotViewerScene;

class ItemSite : public ItemBase
{
    Q_OBJECT
public:
    enum { Type = GraphicsItemType::ITEM_SITE };
    int type() const { return Type; }

public:
    ItemSite(ItemEdge* parentedge, double percent, RobotViewerScene* scene);
    virtual ~ItemSite();

    virtual unsigned int getID();
    virtual void setID(unsigned int id);

    QRectF getBaseRect();

    SiteData getSiteData();
    void setSiteData(const SiteData & site_data);

    void setParentEdge(ItemEdge* parentedge);
    ItemEdge* getParentEdge();
    void resetParentEdge(ItemEdge *parentedge, double percent);

    void setPercentOfEdge(double percent);
    double getPercentOfEdge();

    void adjustWithEdgeMoved(QPointF newPos);
    void adjustWithEdge();
    void resetPos(double percent);
    void setPosData(QPointF posf);
    double getPosx();
    double getPosy();
    double getAngle();

    void removeSiteFromEdge();

    double getSitePriority();
    void setSitePriority(double pro);

    int getDirection();
    void setDirection(int d);

    int getPreciseStop();
    void setPreciseStop(int precise);

    int getFuncCharacter();
    void setFuncCharacter(int funccharacter);

    unsigned int getSouceNodeID();
    unsigned int getDestNodeID();

    int getWaitTime();
    void setWaitTime(int time);

    void setMapScene(RobotViewerScene* scene) { scene_ = scene; }

    int itemflag(GraphicsItemFlag flag);
    void comeBack();
    void setArrayTemp(bool);

signals:
    void mouseRightClicked(int);

private slots:
    void aboutAttribute();

protected:
    //draw
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void drawCommonNode(QPainter *painter);

    //event
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    //menu
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
    void createContextMenu();

private:
    RobotViewerScene* scene_;
    ItemEdge* parent_edge_;
    QMenu* contextMenu_;
    double shape_r;
    bool device_connected_;
    bool hover_enter_;
    bool is_array_temp_;
    SiteData site_data_;
    double scale_factor_;
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_SITE__H__
