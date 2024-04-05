#ifndef __COMWISE__ROBOT_VIEWER__SCENE__H__
#define __COMWISE__ROBOT_VIEWER__SCENE__H__

#include <thread>
#include <mutex>
#include <QGraphicsScene>
#include <QList>
#include "RobotViewerData.h"
#include "common/msg.h"

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class ItemPolygon;
class QTimer;
class QSoundEffect;
class QMouseEvent;
class QMenu;
QT_END_NAMESPACE

class RobotMap;
class ItemMap;
class ItemAgv;
class ItemInitPos;
class ItemEdge;
class ItemControlPoint;
class ItemSite;
class ItemNode;
class ItemArea;
class ItemReflector;
class ManageMapItem;
class ManageAgvItem;
class ManageNodeItem;
class ManageEdgeItem;
class ManageSiteItem;
class ManageAreaItem;
class ManagePolygonItem;
class ManageLaserItem;
class ManageRouteItem;
class RobotViewerView;

//消息类
class GReflector;
class GLaserScan;
class Glaser3DBeam;
class UPL;
class occupancy_grid_t;
class StorageEvent;

class RobotViewerScene : public QGraphicsScene
{
    Q_OBJECT
public:
    RobotViewerScene(QObject *parent);
    ~RobotViewerScene();

    void setRobotStatus(int status);

    void scaleItem(double scalefactor);

    void selectAll();
    void deleteItems();

    void setInsertType(int mode);
    int getInsertType();

    std::vector<UPL> getTargetList();
    void setRouteUPL(std::vector<UPL> &upls);
    void setCurrentUPL(const UPL &upl);

    void clearRoute();
    void clearTarget();

    bool searchSite(int id);
    bool searchNode(int id);
    bool searchEdge(int id);

    void emitEdgeChanged(const EdgeData &edge_data, int change_type);
    void emitSiteChanged(const SiteData &site_data, int change_type);

    //manage item
    ManageNodeItem *GetNodeManage() { return node_manage_; }
    ManageEdgeItem *GetEdgeManage() { return edge_manage_; }
    ManageSiteItem *GetSiteManage() { return site_manage_; }
    ManageAreaItem *GetAreaManage() { return area_manage_; }
    ManagePolygonItem* GetPolygonManage() { return poly_manage_; }

    //加载地图
    void addCurrentSite();
    bool addCurrentNode();
    void addAgvItem();
    void addMapItem(const MapData &map_data);
    void addPointItems(const QList<QString> &points);
    void addNodeItems(std::vector<NodeData> &nodedatas);
    void addEdgeItems(std::vector<EdgeData> &edgedatas);
    void addSiteItems(std::vector<SiteData> &sitedatas);
    void addAreaItems(std::vector<AreaData> &areadates);

    void confirmPloygonItem();
    void undoPloygonItem();

    void removeMapItem();
    void removeAgvItem();
    void removeEdgeItem(ItemEdge *edge);
    void removeAreaItem();

    bool getNodeDatas(std::vector<NodeData> &nodedatas);
    bool getEdgeDatas(std::vector<EdgeData> &edgedatas);
    bool getSiteDatas(std::vector<SiteData> &sitedatas);
    bool getAreaDatas(std::vector<AreaData> &sitedatas);

    //定时更新
    void updateAll();
    void updateAgvPos();
    void updateLaser();
    void updateAimPos();
    void updateReferencePos();
    void updateRouteItem();
    void enableAimRefUpdate(bool able);

    //消息接受
    void setAgvPos(const OrientPos &agv_pos);
    void setReflector(const GReflector &reflector);
    void setLaserScan(const QPolygonF &laserscan);
    void setLaserScan3D(const Glaser3DBeam &laserscan);
    void setAimPos(const VecPosition &pos);
    void setReferencePos(const VecPosition &reflector);

signals:
    void sigMovePosChanged(QPointF);
    void sigMousePosChanged(QPointF);
    void sigRightMousePressed();
    
    void edgeChanged(const EdgeData &, int);
    void siteChanged(const SiteData &, int);
    
    void sigMoveCamera();
    void sigMapImage(const QImage &);

    void sigInitPos(const algo::RobotPose &);
    void sigSetGoal(const algo::RobotPose &);

    void storageChanged(const StorageEvent&);

public slots:
    void slotContextMenuEvent(int clicked);

    //msg
    void slotUpdateMaps(const QImage &);
    void slotUpdatePlannerPath(const QPolygonF &);
    void slotUpdateOccupancyGrid(const occupancy_grid_t &);

    //nav action
    void slotNavInitPos();
    void slotNavToGoal();
    void slotNavToMutilGoal();
    void slotNavToRest();
    void slotSetSelect();
    void slotMoveCamera();

    void slotShowMap(bool);
    void slotShowLaser(bool);
    void slotTrackRobot(bool);
    void slotZoom(bool zoom_out);
    void slotGetPoly(QVector<PolygonData> &);
    void slotSetPoly(const QVector<PolygonData> &);

private slots:
    void slotTurnAllEdgesDir2Way();
    void slotTurnAllEdgesDirForward();
    void slotTurnAllEdgesDirBackward();
    void slotTurnAllEdgesBackwardableTrue();
    void slotTurnAllEdgesBackwardableFalse();
    void slotClearSitesPri();

protected:
    //鼠标事件
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

    //菜单事件
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);

private:
    void init();
    void deinit(); 

    void initConfig();
    void initMenu();
    void initItem();
    void initOthers();

    void setItemsMovable(bool able);
    void hideAllEdgeControlPoint();

    //画node和site
    void addNode(QPointF pos);
    void addSite(QPointF mousepos);
    QPointF getCorrectPos();
    void setNodeAlign(QPointF mousepos);
    void setSiteAlign(QPointF mousepos);
    void addAlignItem(QList<QLineF> lineList);
    void deleteAlignItem();

    //画选择区域
    void addSelectRectItem(const QRectF &rect);
    void changeSelectRectItem(const QRectF &rect);
    void deleteSelectRectItem();

    //画边的缓存线
    void addDrawLineItem(QPointF fstPos, QPointF secPos);
    void changeDrawLineItem(QPointF fstPos, QPointF secPos);
    void deleteDrawLineItem(void);

    //数据区
    void addNodeInEdge(ItemEdge *edge, ItemNode *node);

    // transform
    QPointF transform_scene_to_map(const QPointF &pos);
    QPointF transform_map_to_scene(const QPointF &pos);

private:
    int type_mode_;		   //鼠标操作类型
    double scale_history_{1.0}; //缩放比例
    bool is_trace_robot_{false};  //追踪机器人

    //右键菜单
    QMenu *context_menu_{nullptr};
    int contextmenu_clicked_flag_{0};

    //区域操作
    QPointF mouse_press_pos_;
    bool area_press_{false};
    ItemArea *current_area_{nullptr};

    //画多边形
    bool ploygon_press_{false};
    QVector<QPointF> points_;
    ItemPolygon* py_item_{nullptr};
    QTimer* click_timer_{nullptr};

    //画线
    bool paste_mouse_press_{false};
    QPointF paste_mouse_press_pos_;
    ItemNode *desNode_{nullptr};
    ItemNode *srcNode_{nullptr};
    QGraphicsLineItem *drawLine_{nullptr};

    //区域选择
    QPointF selectRectOrgPoint_;
    QGraphicsRectItem *selectRect_{nullptr};

    //对齐线
    QList<QGraphicsLineItem *> align_items_;
    QList<QGraphicsRectItem *> map_2d_items_;
    QGraphicsItem *selectItem_{nullptr}; //选择的item

    //AGV管理
    std::mutex agv_mutex_;
    OrientPos agv_pos_;
    ItemAgv *agv_item_{nullptr};

    bool flush_able_;
    double correct_length_;
    double align_axisdis_;

    //参考点
    std::mutex reference_mutex_;
    QPointF reference_pos_;
    ItemReflector *reference_item_{nullptr};

    //预瞄点
    std::mutex aim_mutex_;
    QPointF aim_pos_;
    ItemReflector *aim_item_{nullptr};

    //管理类
    ManageNodeItem *node_manage_{nullptr};
    ManageEdgeItem *edge_manage_{nullptr};
    ManageRouteItem *route_manage_{nullptr};
    ManageSiteItem *site_manage_{nullptr};
    ManageAreaItem *area_manage_{nullptr};
    ManagePolygonItem *poly_manage_{nullptr};
    ManageLaserItem *laser_manage_{nullptr};
    ManageMapItem *map_manage_{nullptr};
    ManageAgvItem *agv_manage_{nullptr};

    //special item
    RobotMap *robot_map_{nullptr};

    //map size
    int map_width_{0};
    int map_height_{0};
    //地图点(0, 0)坐标对应世界坐标系坐标
    float map_originX_{0};
    float map_originY_{0};
    QPointF map_center_point_; //地图中心点坐标
    QPointF scene_center_point_; //场景中心点坐标
    float map_resolution_{1}; //地图一个像素对应真实世界的距离

    //media
    QSoundEffect *media_player_{nullptr};
};

#endif // #define __COMWISE__ROBOT_VIEWER__SCENE__H__

