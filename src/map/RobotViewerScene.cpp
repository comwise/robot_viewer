#include "RobotViewerScene.h"
#include "RobotViewerView.h"
#include "RobotViewerConfig.h"
#include "ItemMap.h"
#include "ItemAgv.h"
#include "ItemInitPos.h"
#include "ItemControlPoint.h"
#include "ItemRouter.h"
#include "ItemReflector.h"
#include "ManageMapItem.h"
#include "ManageAgvItem.h"
#include "ManageNodeItem.h"
#include "ManageEdgeItem.h"
#include "ManageSiteItem.h"
#include "ManageAreaItem.h"
#include "ManagePolygonItem.h"
#include "ManageLaserItem.h"
#include "ManageRouteItem.h"
#include <iostream>
#include <QMenu>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include "map/robot_map.h"

#define SAFE_DELETE(p) \
  if (p) {             \
    delete p;          \
  }                    \
  p = nullptr;

RobotViewerScene::RobotViewerScene(QObject *parent)
    : map_manage_(NULL)
    , agv_manage_(NULL)
    , context_menu_(NULL)
    , contextmenu_clicked_flag_(false)
    , is_trace_robot_(false)
    , srcNode_(NULL)
    , desNode_(NULL)
    , selectRect_(NULL)
    , current_area_(NULL)
    , agv_item_(NULL)
    , selectItem_(NULL)
    , area_press_(false)
    , flush_able_(false)
    , align_axisdis_(10)
{
    setParent(parent);
    init();
}

RobotViewerScene::~RobotViewerScene()
{
    deinit();
}

void RobotViewerScene::init()
{
    initMenu();
    initItem();
    initConfig();
    initOthers();
}

void RobotViewerScene::deinit()
{
  SAFE_DELETE(node_manage_);
  SAFE_DELETE(edge_manage_);
  SAFE_DELETE(route_manage_);
  SAFE_DELETE(site_manage_);
  SAFE_DELETE(area_manage_);
  SAFE_DELETE(poly_manage_);
  SAFE_DELETE(robot_map_);
  SAFE_DELETE(map_manage_);
  SAFE_DELETE(agv_manage_);
  SAFE_DELETE(laser_manage_);
}

void RobotViewerScene::initItem()
{
#if 0
    reference_item_ = new ItemReflector(tr(""));
    reference_item_->setDetect(3);
    reference_item_->setVisible(false);
    addItem(reference_item_);
    
    aim_item_ = new ItemReflector(tr(""));
    aim_item_->setDetect(4);
    aim_item_->setVisible(false);
    addItem(aim_item_);
#endif

    robot_map_ = new RobotMap();
    addItem(robot_map_);

    map_manage_ = new ManageMapItem;
    map_manage_->SetSence(this);

#if 0
    agv_manage_ = new ManageAgvItem(this);
    agv_manage_->SetSence(this);
    agv_manage_->addAgvItem();
#endif

    node_manage_ = new ManageNodeItem;
    node_manage_->SetSence(this);
    site_manage_ = new ManageSiteItem;
    site_manage_->SetSence(this);
    edge_manage_ = new ManageEdgeItem;
    edge_manage_->SetSence(this);
    edge_manage_->SetSiteManage(site_manage_);
    node_manage_->SetEdgeManage(edge_manage_);

    area_manage_ = new ManageAreaItem;
    area_manage_->SetSence(this);
    area_manage_->SetDefaultDesc(tr("区域"));
    poly_manage_ = new ManagePolygonItem();
    poly_manage_->SetSence(this);
    poly_manage_->SetDefaultDesc(tr("多边形"));
    laser_manage_ = new ManageLaserItem;
    laser_manage_->SetSecne(this);
    route_manage_ = new ManageRouteItem;
    route_manage_->SetSence(this);
    route_manage_->SetEdgeManage(edge_manage_);
    route_manage_->SetSiteManage(site_manage_);
}

void RobotViewerScene::initMenu()
{
    //地图编辑
    QAction *turn_alledgesdir2way_act_ = new QAction(tr("所有边变双向"), this);
    connect(turn_alledgesdir2way_act_, SIGNAL(triggered()), this, SLOT(slotTurnAllEdgesDir2Way()));

    QAction *turn_alledgesdirforward_act_ = new QAction(tr("所有边变正向"), this);
    connect(turn_alledgesdirforward_act_, SIGNAL(triggered()), this, SLOT(slotTurnAllEdgesDirForward()));

    QAction *turn_alledgesdirbackward_act_ = new QAction(tr("所有边变反向"), this);
    connect(turn_alledgesdirbackward_act_, SIGNAL(triggered()), this, SLOT(slotTurnAllEdgesDirBackward()));

    QAction *turn_alledgesbackabletrue_act_ = new QAction(tr("允许所有边倒车"), this);
    connect(turn_alledgesbackabletrue_act_, SIGNAL(triggered()), this, SLOT(slotTurnAllEdgesBackwardableTrue()));

    QAction *turn_alledgesbackablefalse_act_ = new QAction(tr("取消所有边倒车"), this);
    connect(turn_alledgesbackablefalse_act_, SIGNAL(triggered()), this, SLOT(slotTurnAllEdgesBackwardableFalse()));

    //显示
    QAction *hide_map_act = new QAction(tr("隐藏地图"), this);
    hide_map_act->setCheckable(true);
    connect(hide_map_act, SIGNAL(toggled(bool)), this, SLOT(slotShowMap(bool)));

    QAction *hide_laser_act = new QAction(tr("隐藏激光"), this);
    hide_laser_act->setCheckable(true);
    connect(hide_laser_act, SIGNAL(toggled(bool)), this, SLOT(slotShowLaser(bool)));

    QAction *track_act = new QAction(tr("跟踪机器人"), this);
    track_act->setCheckable(true);
    connect(track_act, SIGNAL(toggled(bool)), this, SLOT(slotTrackRobot(bool)));

    context_menu_ = new QMenu;
    context_menu_->addAction(turn_alledgesdir2way_act_);
    context_menu_->addAction(turn_alledgesdirforward_act_);
    context_menu_->addAction(turn_alledgesdirbackward_act_);
    context_menu_->addSeparator();
    context_menu_->addAction(turn_alledgesbackabletrue_act_);
    context_menu_->addAction(turn_alledgesbackablefalse_act_);
    context_menu_->addSeparator();
    context_menu_->addAction(hide_map_act);
    context_menu_->addAction(hide_laser_act);
    context_menu_->addSeparator();
    context_menu_->addAction(track_act);
}

void RobotViewerScene::initConfig()
{
#if 0
    ConfigFile config("SecurityPatrol.ini");
    correct_length_ = atof(config.GetValue("correct_length","Defalut_Parameter","1107.7").c_str())/10.0;
    align_axisdis_  = atof(config.GetValue("align_axisdis","Defalut_Parameter","100").c_str())/10.0;
    int tim_bank    = atoi(config.GetValue("tim_bank","Defalut_Parameter","10").c_str());
    int sz_bank     = atoi(config.GetValue("sz_bank","Defalut_Parameter","10").c_str());
    qDebug() <<"default param: corr_len ="<<correct_length_<<", align ="<<align_axisdis_<<", tim ="<<tim_bank<<", sz ="<<sz_bank;
    if(edge_manage_) edge_manage_->setDefaultBank(tim_bank,sz_bank);

    double laser_dist    = atof(config.GetValue("laser_dist","Defalut_Parameter","0").c_str());
    double laser_heading = atof(config.GetValue("laser_heading","Defalut_Parameter","180").c_str());
    bool laser_forward   = atof(config.GetValue("laser_forward","Defalut_Parameter","1").c_str());
    qDebug()<<"laser param: dist ="<<laser_dist<<", heading ="<<laser_heading<<", forward ="<<laser_forward;
    double laser_forward_angle = laser_forward ? 0.0f : 180.0f;
    if (laser_heading<=-180 || laser_heading>=180)
        laser_heading = laser_forward_angle;
    if(laser_manage_) laser_manage_->SetConfig(laser_dist,laser_heading);

    int edge_id = atoi(config.GetValue("edge_id","Defalut_Parameter","10").c_str());
    int node_id = atoi(config.GetValue("node_id","Defalut_Parameter","10").c_str());
    int site_id = atoi(config.GetValue("site_id","Defalut_Parameter","10").c_str());
    qDebug() <<"id param: edge_id ="<<edge_id<<", node_id ="<<node_id<<", site_id ="<<site_id;
    if(node_manage_) node_manage_->SetBasicID(node_id);
    if(edge_manage_) edge_manage_->SetBasicID(edge_id);
    if(site_manage_) site_manage_->SetBasicID(site_id);

    double slow = atof(config.GetValue("global_slow","Defalut_Parameter","0.0").c_str());
    double stop = atof(config.GetValue("global_stop","Defalut_Parameter","0.0").c_str());
    qDebug() <<"sensor param: slow ="<< slow <<", stop ="<< stop;
    if(edge_manage_) edge_manage_->setDefaultSensor(slow,stop);
#endif
}

void RobotViewerScene::initOthers()
{
    //node to map_scene
    connect(this, &RobotViewerScene::sigMapImage,
          this, [=](const QImage &img) { emit sigMapImage(img); });
    connect(robot_map_, &RobotMap::sig_pub_init_pos,
        this, [=](const algo::RobotPose &data) { emit sigInitPos(data); });
    connect(robot_map_, &RobotMap::sig_pub_2d_goal,
        this, [=](const algo::RobotPose &data) { emit sigSetGoal(data); });
    connect(robot_map_, &RobotMap::sig_cursor_pos,
          this, [=](const QPointF &pos) { 
            QPointF map_pos = transform_scene_to_map(pos);
            emit sigMousePosChanged(map_pos);
    });

    click_timer_ = new QTimer(this);
    //connect(click_timer_, SIGNAL(timeout()), this, SLOT(mouseClick()));
}

void RobotViewerScene::setInsertType(int mode)
{
    deleteAlignItem();
    type_mode_ = mode;
    switch (type_mode_)
    {
    case ACTION_MOVE:
        setItemsMovable(true);
        break;
    default:
        setItemsMovable(false);
        break;
    }
}

int RobotViewerScene::getInsertType()
{
    return type_mode_;
}

void RobotViewerScene::scaleItem(double scale_factor)
{
    scale_history_ = scale_factor;
    if(site_manage_) site_manage_->SetItemVisible(scale_factor >= 0.1);
    if(area_manage_) area_manage_->SetItemVisible(scale_factor >= 0.1);

#if 0
    if (map_manage_)
        map_manage_->SetItemScale(scale_factor);
    if (agv_manage_)
        agv_manage_->SetItemScale(scale_factor);
    if (node_manage_)
        node_manage_->SetItemScale(scale_factor);
    if (edge_manage_)
        edge_manage_->SetItemScale(scale_factor);
    if (site_manage_)
        site_manage_->SetItemScale(scale_factor);
    if (area_manage_)
        area_manage_->SetItemScale(scale_factor);
    if (poly_manage_)
        poly_manage_->SetItemScale(scale_factor);
    if (laser_manage_)
        laser_manage_->SetItemScale(scale_factor);
    if (route_manage_)
        route_manage_->SetItemScale(scale_factor);
#endif
}

void RobotViewerScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    QGraphicsScene::contextMenuEvent(contextMenuEvent);
    if (!contextmenu_clicked_flag_) {
        if (context_menu_) {
            context_menu_->exec(contextMenuEvent->screenPos());
        }
    } else {
        contextmenu_clicked_flag_ = 0;
    }
}

void RobotViewerScene::slotContextMenuEvent(int clicked)
{
    contextmenu_clicked_flag_ = clicked;
}

void RobotViewerScene::slotTurnAllEdgesDir2Way()
{
    if (nullptr == edge_manage_) {
        return;
    }
    foreach (ItemEdge *edge, edge_manage_->GetItems()) {
        if (edge) {
            edge->setDirection(BOTH_WAY);
        }
    }
    update();
}

void RobotViewerScene::slotTurnAllEdgesDirForward()
{
    if (nullptr == edge_manage_) {
        return;
    }
    foreach (ItemEdge *edge, edge_manage_->GetItems()) {
        if (edge) {
            edge->setDirection(POSITIVE_WAY);
        }
    }
    update();
}

void RobotViewerScene::slotTurnAllEdgesDirBackward()
{
    if (nullptr == edge_manage_)
        return;

    foreach (ItemEdge *edge, edge_manage_->GetItems()) {
        if (edge) {
            edge->setDirection(NEGTIVE_WAY);
        }
    }
    update();
}

void RobotViewerScene::slotTurnAllEdgesBackwardableTrue()
{
    if (nullptr == edge_manage_) {
        return;
    }
    foreach (ItemEdge *edge, edge_manage_->GetItems()) {
        if (edge) {
            edge->setBackwardable(REVERSE_ALLOWED);
        }
    }
    update();
}

void RobotViewerScene::slotTurnAllEdgesBackwardableFalse()
{
    if (nullptr == edge_manage_) {
        return;
    }
    foreach (ItemEdge *edge, edge_manage_->GetItems()) {
        if (edge) {
            edge->setBackwardable(FORWARD_ONLY);
        }
    }
    update();
}

void RobotViewerScene::slotClearSitesPri()
{
    if (nullptr == edge_manage_) {
        return;
    }

    QMessageBox::StandardButton btn = QMessageBox::information(0, 
        tr("清空优先级"), tr("是否清空所有任务点优先级？"), QMessageBox::Ok | QMessageBox::Cancel);

    if (btn != QMessageBox::Ok) {
        return;
    }
    foreach (ItemEdge *edge, edge_manage_->GetItems()) {
        edge->setEdgePriority(0);
        foreach (ItemSite *site, edge->getSiteFromEdge()) {
            site->setSitePriority(0);
        }
    }
    update();
}

void RobotViewerScene::slotTrackRobot(bool flag)
{
    is_trace_robot_ = flag;
}

void RobotViewerScene::slotShowMap(bool is_show)
{
#if 0
    if (map_manage_) {
        map_manage_->SetItemVisible(is_show);
    }
#else
    if (robot_map_ && !is_show) {
        robot_map_->paint_maps(QImage());
    }
#endif
}

void RobotViewerScene::slotShowLaser(bool is_show)
{
#if 0
    if (laser_manage_) {
        laser_manage_->SetLaserVisible(is_show);
    }
#else
    if (robot_map_ && !is_show) {
        robot_map_->paint_laser_scan(QPolygonF());
    }
#endif
}

void RobotViewerScene::slotZoom(bool zoom_out)
{
        if (robot_map_) {
        zoom_out? robot_map_->set_zoom_out() : robot_map_->set_zoom_in();
        }
}

void RobotViewerScene::slotGetPoly(QVector<PolygonData> &data)
{
    data.clear();
    if(poly_manage_) {
        poly_manage_->getPolygon(data);
    }
}

void RobotViewerScene::slotSetPoly(const QVector<PolygonData> &data)
{
    if(poly_manage_) {
        poly_manage_->setPolygon(data);
    }
}

void RobotViewerScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF mousepos = mouseEvent->scenePos();
    if (mouseEvent->button() != Qt::LeftButton) {
        return;
    }

    switch (type_mode_)
    {
    case ACTION_SELECT:
    {
        paste_mouse_press_ = true;
        paste_mouse_press_pos_ = mousepos;
        QRectF rectf(mousepos.x() - 1, mousepos.y() - 1, 2, 2); //选择的大小为2个像素
        QList<QGraphicsItem *> itemAll = items(rectf);
        if(edge_manage_) {
            auto items = edge_manage_->GetItems();
            edge_manage_->ShowEdgeControlPoint(items, false);
        }
        if (itemAll.size() < 1) {
            clearSelection();
            addSelectRectItem(QRectF(mousepos, QSizeF(0, 0)).normalized());
        } else {
            if(edge_manage_) {
                QList<ItemEdge *> eList = edge_manage_->FindItemsInItems(itemAll, ItemEdge::Type);
                edge_manage_->ShowEdgeControlPoint(eList, true);
            }
        }
        break;
    }

    case ACTION_MOVE:
    {
        if(nullptr == edge_manage_ || nullptr == site_manage_) {
            break;
        }
        QList<QGraphicsItem *> itemAll = items(mousepos);
        QList<ItemEdge *> beziers = edge_manage_->FindItemsInItems(itemAll, ItemEdge::Type);
        edge_manage_->ShowEdgeControlPoint(beziers, true);
        site_manage_->findSiteInItems(itemAll);
        if (!selectItem_ && node_manage_)
        {
            selectItem_ = node_manage_->findNodeInItems(itemAll);
        }
        break;
    }
    case ADD_NODE:
    {
        if(node_manage_) {
        node_manage_->GetAlignNodePos(mousepos, align_axisdis_);
        addNode(mousepos);
        }
        break;
    }

    case ADD_EDGE:
    {
        if(node_manage_) {
        srcNode_ = node_manage_->findNode(mousepos);
        addDrawLineItem(mousepos, mousepos);
        }
        break;
    }

    case ADD_AREA:
    {
        mouse_press_pos_ = mousepos;
        area_press_ = true;
        break;
    }

    case ADD_POLYGON:
    {
        if(!ploygon_press_) {
            points_.clear();
            py_item_ = nullptr;
            ploygon_press_ = true;
        }
        break;
    }

    case SET_LOCROBOT:
    {
        if (agv_manage_) {
            agv_manage_->SetLocationPress(mousepos);
        }
        break;
    }

    case SET_NAVTARGET:
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        if (route_manage_) {
            route_manage_->AddTarget(mousepos);
        }
        break;
    }

    default:
        break;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void RobotViewerScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF mouse_pos = mouseEvent->scenePos();
#if 0
    QPointF map_pos = transform_scene_to_map(mouse_pos);
    emit sigMovePosChanged(map_pos);
#endif

    switch (type_mode_)
    {
    case ACTION_SELECT:
    {
        if (selectRect_) {
            QRectF rect(selectRectOrgPoint_, mouse_pos);
            changeSelectRectItem(rect.normalized()); //改变选择区域
        }
        break;
    }
    case ACTION_MOVE:
    {
        if (selectItem_ != NULL) {
            //节点自动对齐
            if (selectItem_->type() == ItemSite::Type)
                setSiteAlign(selectItem_->scenePos());
            if (selectItem_->type() == ItemNode::Type)
                setNodeAlign(selectItem_->scenePos());
        }
        setItemsMovable(true);
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    }
    case ADD_NODE:
        setNodeAlign(mouse_pos);
        break;
    case ADD_SITE:
        setSiteAlign(mouse_pos);
        break;
    case ADD_EDGE:
    {
        //如果没有起点
        if (nullptr == srcNode_  || nullptr == node_manage_) {
            break;
        }
        desNode_ = node_manage_->findNode(mouse_pos);
        //如果起点等于终点
        if (desNode_ == srcNode_) {
            break;
        }
        //如果没有终点
        if (desNode_ == NULL) {
            changeDrawLineItem(srcNode_->scenePos(), mouse_pos);
        } else {
            changeDrawLineItem(desNode_->scenePos(), desNode_->scenePos());
            if(edge_manage_) {
                ItemEdge *edge = edge_manage_->addEdgeItem(srcNode_, desNode_);
                (void)edge;
                srcNode_ = desNode_;
            }
        }
        break;
    }
    case ADD_AREA:
    {
        if (!area_press_) {
            break;
        }
        if (current_area_) {
            current_area_->SetBottomRight(mouse_pos);
            //qDebug() << "area(moving)" << mouse_press_pos_ << mouse_pos;
        } else {
            if(area_manage_)  {
                current_area_ = area_manage_->addAreaItem(mouse_press_pos_, mouse_pos);
                //qDebug() << "area(begin)" << mouse_press_pos_ << mouse_pos;
       
            }
        }
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    }
    case ADD_POLYGON:
    {
        if (!ploygon_press_) {
            break;
        }
        QPolygonF temp(points_);
        temp << mouse_pos;
        if(nullptr == py_item_)  {
            py_item_ = poly_manage_->add(points_);
        }
        if(py_item_) {
            py_item_->setPolygon(temp);
        }
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    }
    case SET_LOCROBOT:
    {
        if (agv_manage_) {
            agv_manage_->SetLocationMove(mouse_pos);
        }
        break;
    }
    default:
        break;
    }
}

void RobotViewerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF scene_pos = mouseEvent->scenePos();
    if (mouseEvent->button() != Qt::LeftButton) {
        if(mouseEvent->button() == Qt::RightButton) {
            if(type_mode_ == ADD_POLYGON) {
                points_ << scene_pos;
                confirmPloygonItem();
            }
        }
        if(edge_manage_) {
            auto items = edge_manage_->GetItems();
            edge_manage_->ShowEdgeControlPoint(items, false);
            deleteAlignItem();
            emit sigRightMousePressed();
        }
        return;
    }
    switch (type_mode_)
    {	
    case ACTION_SELECT:
    {
        if (selectRect_) {
            QList<QGraphicsItem *> itemAll = items(selectRect_->rect());
            foreach (QGraphicsItem *item, itemAll) {
                if(item) {
                    item->setSelected(true);
                }
            }
            if(edge_manage_) {
                QList<ItemEdge *> beziers = edge_manage_->FindItemsInItems(itemAll, ItemEdge::Type);
                edge_manage_->ShowEdgeControlPoint(beziers, true);
            }
            deleteSelectRectItem();
        }
        break;
    }
    case ACTION_MOVE:
    {
        if (selectItem_ != NULL)
        {
            deleteAlignItem(); //删除对齐
            selectItem_ = NULL;
        }
        break;
    }
    case ADD_NODE:
    {
        deleteAlignItem(); //删除对齐
        break;
    }
    case ADD_EDGE:
    {
        deleteDrawLineItem(); //删除绘制的线
        srcNode_ = NULL;
        desNode_ = NULL;
        break;
    }
    case ADD_SITE:
    {
        deleteAlignItem(); //删除对齐
        if(site_manage_) {
        site_manage_->GetAlignSitePos(scene_pos, align_axisdis_);
        addSite(scene_pos);
        }
        break;
    }
    case ADD_AREA:
    {
        //qDebug() << "area(release)" << mouse_press_pos_ << area_press_;
        if (current_area_) {
            current_area_->AreaUpdate();
            current_area_ = NULL;
        }
        area_press_ = false;
        break;
    }

    case ADD_POLYGON:
    {
        if (ploygon_press_) {
           points_ << scene_pos;
           if (py_item_) {
               py_item_->setPolygon(points_);
           }
        }
        break;
    }
    case SET_LOCROBOT:
    {
        if (agv_manage_) {
            agv_manage_->SetLocationRelease(scene_pos);
        }
        break;
    }

    default:
        break;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void RobotViewerScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF mousepos = mouseEvent->scenePos();
    (void)mousepos;
    switch (type_mode_)
    {
    case ACTION_SELECT:
    {
        if(!site_manage_ || !edge_manage_) {
            break;
        }
        QGraphicsScene::mouseDoubleClickEvent(mouseEvent); //什么都不选择的时候，才把双击的消息传递给item
        QList<ItemSite *> sites = site_manage_->GetSelectItems();
        QList<ItemEdge *> edges = edge_manage_->GetSelectItems();
        foreach (ItemSite *item, sites)
        {
            emit siteChanged(item->getSiteData(), 2);
        }
        foreach (ItemEdge *item, edges)
        {
            emit edgeChanged(item->getEdgeData(), 2);
        }
        break;
    }
    case ADD_POLYGON:
    {
        break;
    }
    default:
        break;
    }
}

bool RobotViewerScene::getNodeDatas(std::vector<NodeData> &nodedatas)
{
    return node_manage_? node_manage_->getNodeDatas(nodedatas) : false;
}

bool RobotViewerScene::getEdgeDatas(std::vector<EdgeData> &edgedatas)
{
    return edge_manage_? edge_manage_->getEdgeDatas(edgedatas) : false;
}

bool RobotViewerScene::getSiteDatas(std::vector<SiteData> &sitedatas)
{
    return site_manage_? site_manage_->getSiteDatas(sitedatas) : false;
}

bool RobotViewerScene::getAreaDatas(std::vector<AreaData> &areadatas)
{
    return area_manage_? area_manage_->getAreaDatas(areadatas) : false;
}

void RobotViewerScene::addNodeItems(std::vector<NodeData> &nodedatas)
{
    if(node_manage_) node_manage_->addNodeItems(nodedatas);
}

void RobotViewerScene::addEdgeItems(std::vector<EdgeData> &edgedatas)
{
    if(edge_manage_) edge_manage_->addEdgeItems(edgedatas);
}

void RobotViewerScene::addSiteItems(std::vector<SiteData> &sitedatas)
{
    if(site_manage_) site_manage_->addSiteItems(sitedatas);
}

void RobotViewerScene::addAreaItems(std::vector<AreaData> &areadates)
{
    if(area_manage_) area_manage_->addAreaItems(areadates);
}

void RobotViewerScene::addNodeInEdge(ItemEdge *edge, ItemNode *node)
{
    if(nullptr == edge || nullptr == node || nullptr == edge_manage_) {
        return;
    }

    double percent = 0.0;
    QPointF near_pos;
    percent = edge->getNearestPosPercent(node->pos(), near_pos);
    node->setPos(near_pos);
    std::pair<ItemEdge *, ItemEdge *> edges = edge_manage_->SpitEdge(edge, node, percent);
    if (edges.first && edges.second) {
        edge_manage_->removeEdgeItem(edge);
    }
}

void RobotViewerScene::removeEdgeItem(ItemEdge *edge)
{
    if(edge_manage_) edge_manage_->removeEdgeItem(edge);
}

void RobotViewerScene::removeAreaItem()
{
    if(area_manage_) area_manage_->ClearItems();
}

void RobotViewerScene::deleteItems()
{
    QList<QGraphicsItem *> itemAll = selectedItems();
#ifdef DELETE_CRASH
    printf("11, size = %d\n", itemAll.size());
#endif
    for(auto &item : itemAll)
    {
        if(nullptr == item) {
            continue;
        }
#ifdef DELETE_CRASH
        printf("22, %d\n", item);
#endif
        int type = item->type();
#ifdef DELETE_CRASH
        printf("33, type = %d\n", type);
#endif
        switch (type)
        {
        case ItemNode::Type:
        {
            ItemNode *node = qgraphicsitem_cast<ItemNode *>(item);
            itemAll.removeOne(item);
            foreach (ItemEdge *edge, node->getEdges())
            {
                itemAll.removeOne(edge->getFirstControlPoint());
                itemAll.removeOne(edge->getSecControlPoint());
                foreach (ItemSite *site, edge->getSiteFromEdge()) {
                    itemAll.removeOne(site);
                }
                itemAll.removeOne(edge);
            }
            if(node_manage_ && node) {
                node_manage_->removeNodeItem(node);
            }
            break;
        }
        case ItemEdge::Type:
        {
            ItemEdge *edge = qgraphicsitem_cast<ItemEdge *>(item);
            if(nullptr == edge) {
                continue;
            }
            itemAll.removeOne(item);
            foreach (ItemSite *site, edge->getSiteFromEdge()) {
                itemAll.removeOne(site);
            }
            itemAll.removeOne(edge->getFirstControlPoint());
            itemAll.removeOne(edge->getSecControlPoint());
            if(edge_manage_ && edge) {
                edge_manage_->removeEdgeItem(edge);
            }
            break;
        }
        case ItemControlPoint::Type:
        {
            break;
        }
        case ItemSite::Type:
        {
            ItemSite *site = qgraphicsitem_cast<ItemSite *>(item);
            itemAll.removeOne(item);
            if(site_manage_ && site) {
                site_manage_->removeSiteItem(site);
            }
            break;
        }
        case ItemArea::Type:
        {
            ItemArea *area = qgraphicsitem_cast<ItemArea *>(item);
            foreach (CornerItem *corner_item, area->GetCornerItem()) {
                itemAll.removeOne(corner_item);
            }
            itemAll.removeOne(item);
            if(area_manage_) { 
                area_manage_->removeAreaItem(area);
            }
            break;
        }
        case ItemPolygon::Type:
        {
#if 0
            ItemPolygon *poly = qgraphicsitem_cast<ItemPolygon *>(item);
            StorageEvent event;
            if(poly) {
                event.data = poly->getData();
                event.data.feature = 1;
            }
            itemAll.removeOne(item);
            if(poly_manage_) { 
                poly_manage_->DelItem(poly);
            }
            emit storageChanged(event);
            break;
#endif
        }
        default:
            break;
        }
    }
#ifdef DELETE_CRASH
    printf("66\n");
#endif
}

void RobotViewerScene::selectAll()
{
    if(node_manage_) node_manage_->SetItemsSelected(true);
    if(edge_manage_) edge_manage_->SetItemsSelected(true);
    if(site_manage_) site_manage_->SetItemsSelected(true);
    if(area_manage_) area_manage_->SetItemsSelected(true);
    if(poly_manage_) poly_manage_->SetItemsSelected(true);
    if(robot_map_)   robot_map_->setSelected(true);
}

void RobotViewerScene::setItemsMovable(bool flag)
{
    if(node_manage_) node_manage_->SetItemsFlag(QGraphicsItem::ItemIsMovable, flag);
    if(edge_manage_) edge_manage_->SetItemsFlag(QGraphicsItem::ItemIsMovable, flag);
    if(site_manage_) site_manage_->SetItemsFlag(QGraphicsItem::ItemIsMovable, flag);
    if(area_manage_) area_manage_->SetItemsFlag(QGraphicsItem::ItemIsMovable, flag);
    if(robot_map_)   robot_map_->setFlag(QGraphicsItem::ItemIsMovable, flag);
}

void RobotViewerScene::addSelectRectItem(const QRectF &rect)
{
    selectRect_ = new QGraphicsRectItem(rect);
    selectRect_->setPen(QPen(Qt::black, 5.0 / scale_history_, Qt::DashLine));
    selectRectOrgPoint_ = rect.topLeft();
    addItem(selectRect_);
}

void RobotViewerScene::changeSelectRectItem(const QRectF &rect)
{
    if(selectRect_) {
        selectRect_->setRect(rect);
    }
}

void RobotViewerScene::deleteSelectRectItem()
{
    if (selectRect_) {
        removeItem(selectRect_);
        delete selectRect_;
        selectRect_ = NULL;
        selectRectOrgPoint_ = QPointF(0, 0);
    }
}

void RobotViewerScene::addDrawLineItem(QPointF fstPos, QPointF secPos)
{
    drawLine_ = new QGraphicsLineItem(QLineF(fstPos, secPos));
    addItem(drawLine_);
}

void RobotViewerScene::changeDrawLineItem(QPointF fstPos, QPointF secPos)
{
    QLineF newLine(fstPos, secPos);
    if(drawLine_) {
        drawLine_->setLine(newLine);
    }
}

void RobotViewerScene::deleteDrawLineItem(void)
{
    if (drawLine_) {
        removeItem(drawLine_);
        delete drawLine_;
    }
    drawLine_ = nullptr;
}

void RobotViewerScene::undoPloygonItem()
{
    if(!points_.empty()) {
        points_.pop_back();
        if(py_item_) {
            py_item_->setPolygon(points_);
#if 0
            StorageEvent e;
            PolygonData data = py_item_->getData();
            if(points_.empty()) {
                data.feature = 2;
            }
            e.data = data;
            emit storageChanged(e);
#endif
        }
    }
}

void RobotViewerScene::confirmPloygonItem()
{
    ploygon_press_ = false;
    if(py_item_) {
#if 0
        StorageEvent e;
        e.data = py_item_->getData();
        emit storageChanged(e);
#endif
    }
}

void RobotViewerScene::setAgvPos(const OrientPos &pt)
{
#ifdef ENABLE_TF
    QPointF pos = transform_map_to_scene(QPointF(pt.x, pt.y));
#else
    QPointF pos(pt.x, pt.y);
#endif
    if (robot_map_) {
        robot_map_->paint_robot_pos(algo::RobotPose(pos.x(), pos.y(), pt.angle));
      }

    {
        std::lock_guard<std::mutex> lock(agv_mutex_);
        agv_pos_.x = pos.x();
        agv_pos_.y = pos.y();
        agv_pos_.angle = pt.angle * 180 / 3.1416f;
    }
#if 0
    if(laser_manage_) {
        laser_manage_->setAgvPos(agv_pos_);
    }
#endif
}

void RobotViewerScene::updateAll()
{
    updateAgvPos();
#if 0
    updateLaser();
    updateAimPos();
    updateReferencePos();
    updateRouteItem();
#endif
}

void RobotViewerScene::updateAgvPos()
{
    OrientPos pos;
    {
        std::lock_guard<std::mutex> lock(agv_mutex_);
        pos = agv_pos_;
    }

#if 0
    if (agv_item_ == NULL) {
        agv_item_ = new ItemAgv();
        this->addItem(agv_item_);
    }
    agv_item_->setPos(QPointF(pos.x, pos.x));
    agv_item_->setRotation(pos.angle);
    agv_item_->setAgvAngle(pos.angle);
#endif
#if 0
    if (agv_manage_ && !agv_manage_->IsLocPressed()) {
        QPointF pt(pos.x, pos.y);
        agv_manage_->UpdateRobotPos(pt, pos.angle);
    }
#endif

    if (is_trace_robot_) {
        foreach (QGraphicsView *view, views()) {
            if (view){
                view->centerOn(robot_map_);
                //view->centerOn(QPointF(pos.x, pos.y));
            }
        }
    }
}

void RobotViewerScene::updateLaser()
{
    if(laser_manage_) laser_manage_->UpdateItems();
}

void RobotViewerScene::setReflector(const GReflector &reflector)
{
    if(laser_manage_) laser_manage_->setReflector(reflector);
}

void RobotViewerScene::setLaserScan(const QPolygonF &data)
{
    if (robot_map_) {
#ifdef ENABLE_TF
    QPolygonF scene_data;
    for(int i = 0; i < data.size(); i++) {
      QPointF pt = transform_map_to_scene(QPointF(data[i].x(), data[i].y()));
      scene_data.append(pt);
    }
#else
    QPolygonF scene_data(data);
#endif
    robot_map_->paint_laser_scan(scene_data);
    //if(laser_manage_)
    //	laser_manage_->setLaserScan(scene_data);
  }
}

void RobotViewerScene::setLaserScan3D(const Glaser3DBeam &laserscan)
{
    if (laser_manage_) {
        laser_manage_->setLaserScan3D(laserscan);
    }
}

std::vector<UPL> RobotViewerScene::getTargetList()
{
    std::vector<UPL> upl_lst;
    return route_manage_? route_manage_->getTargetList() : upl_lst;
}

void RobotViewerScene::setRouteUPL(std::vector<UPL> &upls)
{
    if(route_manage_) {
        route_manage_->AddRouteItems(upls);
    }
}

void RobotViewerScene::clearRoute()
{
    if(route_manage_) {
        route_manage_->DelRouteItems();
    }
}

void RobotViewerScene::setCurrentUPL(const UPL &upl)
{
    if(route_manage_) {
        route_manage_->setCurrentUPL(upl);
    }
}

void RobotViewerScene::clearTarget()
{
    if(route_manage_) {
        route_manage_->DelTarget();
    }
}

void RobotViewerScene::setAimPos(const VecPosition &pos)
{
    std::lock_guard<std::mutex> lock(aim_mutex_);
    aim_pos_.setX(pos.getX() * 100.0f);
    aim_pos_.setY(pos.getY() * 100.0f);
}

void RobotViewerScene::setReferencePos(const VecPosition &pos)
{
    std::lock_guard<std::mutex> lock(reference_mutex_);
    reference_pos_.setX(pos.getX() * 100.0f);
    reference_pos_.setY(pos.getY() * 100.0f);
}

void RobotViewerScene::updateAimPos()
{
    std::lock_guard<std::mutex> lock(aim_mutex_);
    if (flush_able_) {
        aim_item_->setVisible(true);
        aim_item_->setPos(aim_pos_);
    } else {
        aim_item_->setVisible(false);
    }
}

void RobotViewerScene::updateReferencePos()
{
    std::lock_guard<std::mutex> lock(reference_mutex_);
    if (flush_able_) {
        reference_item_->setVisible(true);
        reference_item_->setPos(reference_pos_);
    } else {
        reference_item_->setVisible(false);
    }
}

void RobotViewerScene::updateRouteItem()
{
    if (route_manage_) {
        route_manage_->ChangeRouteItem();
    }
}

void RobotViewerScene::enableAimRefUpdate(bool able)
{
    flush_able_ = able;
}

void RobotViewerScene::addCurrentSite()
{
    QPointF sitepos = getCorrectPos();
    addSite(sitepos);
}


void RobotViewerScene::addMapItem(const MapData &map_data)
{
    if (map_manage_) {
        map_manage_->addMapItem(map_data);
    }
}
void RobotViewerScene::removeMapItem()
{
    if (map_manage_) {
        map_manage_->removeMapItem();
    }
}

void RobotViewerScene::addAgvItem()
{
    if (agv_manage_) {
        agv_manage_->addAgvItem();
    }
}

void RobotViewerScene::removeAgvItem()
{
    if (agv_manage_) {
        agv_manage_->removeAgvItem();
    }
}

bool RobotViewerScene::addCurrentNode()
{
    QPointF nodepos = getCorrectPos();
    if (nullptr == node_manage_ || node_manage_->findNode(nodepos)) {
        return false;
    }
    addNode(nodepos);
    return true;
}

void RobotViewerScene::addNode(QPointF mousepos)
{
    if(!node_manage_ || !edge_manage_) {
        return;
    }

    ItemNode *node = node_manage_->addNodeItem(mousepos);
    QList<QGraphicsItem *> items_ = this->items(mousepos);
    ItemEdge *edge = edge_manage_->findEdgeInItems(items_);
    if (edge) {
        QMessageBox::StandardButton btn = QMessageBox::information(0, 
            tr("分割边"), tr("是否使用该节点分割选定的边？"), QMessageBox::Ok | QMessageBox::Cancel);
        if (btn == QMessageBox::Ok) {
            addNodeInEdge(edge, node);
        }
    }
}

void RobotViewerScene::addSite(QPointF mousepos)
{
    if(site_manage_) site_manage_->addSiteItem(mousepos);
}

QPointF RobotViewerScene::getCorrectPos()
{
    OrientPos agv_pos;
    {
        std::lock_guard<std::mutex> lock(agv_mutex_);
        agv_pos = agv_pos_;
    }
#if 0
    VecPosition robotpos = agv_pos.pos;
    VecPosition dpos(correct_length_, 0);
    dpos.rotate(agv_pos.angle);
    VecPosition newpos = robotpos + dpos;
    QPointF pos=QPointF(newpos.getX(),newpos.getY());
    ManageEdgeItem::NearEdgeFromSite near = edge_manage_->GetNearestEdge(pos);
    return near.pos_near_;
#else
    return QPointF();
#endif
}

void RobotViewerScene::addAlignItem(QList<QLineF> lineList)
{
    for (int i = 0; i < lineList.size(); i++) {
        QGraphicsLineItem *lineItem = new QGraphicsLineItem(lineList[i]);
        lineItem->setPen(QPen(Qt::black, 1, Qt::DashLine, Qt::RoundCap,
                              Qt::RoundJoin));
        addItem(lineItem);
        align_items_ << lineItem;
    }
}

void RobotViewerScene::deleteAlignItem()
{
    for (int i = 0; i < align_items_.size(); i++) {
        removeItem(align_items_[i]);
        delete (align_items_[i]);
        align_items_[i] = NULL;
    }
    align_items_.clear();
}

void RobotViewerScene::setNodeAlign(QPointF mousepos)
{
    deleteAlignItem();
    if(node_manage_) {
        QList<QLineF> line_list = node_manage_->GetNodeAlignLine(mousepos, align_axisdis_);
        addAlignItem(line_list);
    }
}

void RobotViewerScene::setSiteAlign(QPointF mousepos)
{
    deleteAlignItem();
    if(site_manage_) {
        QList<QLineF> line_list = site_manage_->GetSiteAlignLine(mousepos, align_axisdis_);
        addAlignItem(line_list);
    }
}

bool RobotViewerScene::searchSite(int id)
{
    if(nullptr == site_manage_) {
        return false;
    }
    ItemSite *site = site_manage_->findSite(id);
    if (nullptr == site) {
        return false;
    }
    QPointF sitepos = site->scenePos();
    site_manage_->SetItemsSelected(false);
    site->setSelected(true);
    QGraphicsView *view = views()[0];
    view->centerOn(site);
    view->fitInView(sitepos.x() - 100, sitepos.y() - 100, 200, 200, Qt::KeepAspectRatio);
    ((RobotViewerView *)(view))->updateSceneScale();

    return true;
}

bool RobotViewerScene::searchNode(int id)
{
    if(!node_manage_) {
        return false;
    }
    ItemNode *node = node_manage_->findNode(id);
    if (NULL == node) {
        return false;
    }
    QPointF nodepos = node->scenePos();
    node_manage_->SetItemsSelected(false);
    node->setSelected(true);
    setFocusItem(node);
    QGraphicsView *view = views()[0];
    view->centerOn(node);
    view->fitInView(nodepos.x() - 100, nodepos.y() - 100, 200, 200, Qt::KeepAspectRatio);
    ((RobotViewerView *)(view))->updateSceneScale();

    return true;
}

bool RobotViewerScene::searchEdge(int id)
{
    if(nullptr == edge_manage_) {
        return false;
    }
    ItemEdge *edge = edge_manage_->findEdge(id);
    if (nullptr == edge) {
        return false;
    }
    QPointF nodepos = edge->sourceNode()->scenePos();
    QPointF destpos = edge->destNode()->scenePos();
    nodepos.setX((nodepos.x() + destpos.x()) / 2.0);
    nodepos.setY((nodepos.y() + destpos.y()) / 2.0);
    edge_manage_->SetItemsSelected(false);
    edge->setSelected(true);
    setFocusItem(edge);
    QGraphicsView *view = views()[0];
    view->centerOn(edge);
    view->fitInView(nodepos.x() - 500, nodepos.y() - 500, 1000, 1000, Qt::KeepAspectRatio);
    ((RobotViewerView *)(view))->updateSceneScale();
    
    return true;
}

void RobotViewerScene::emitEdgeChanged(const EdgeData &edge_data, int change_type)
{
    emit edgeChanged(edge_data, change_type);
}

void RobotViewerScene::emitSiteChanged(const SiteData &site_data, int change_type)
{
    emit siteChanged(site_data, change_type);
}

void RobotViewerScene::addPointItems(const QList<QString> &points)
{
    for (int i = 0; i < map_2d_items_.size(); ++i)
    {
        this->removeItem(map_2d_items_[i]);
    }
    map_2d_items_.clear();
    for (int i = 0; i < points.size(); ++i)
    {
        QString line = points[i];
        QStringList str_list = line.split(" ");
        if (str_list.size() >= 2)
        {
            QPointF pos(str_list[0].toFloat() / 10, str_list[1].toFloat() / 10);
            QGraphicsRectItem *rect = new QGraphicsRectItem;
            rect->setRect(0, 0, 1, 1);
            rect->setBrush(QBrush(Qt::black));
            rect->setPos(pos);
            rect->setEnabled(false);
            rect->setZValue(-100);
            this->addItem(rect);
            map_2d_items_ << rect;
        }
    }
}

void RobotViewerScene::setRobotStatus(int status)
{
    if (robot_map_) {
        robot_map_->set_robot_status(status);
    }
}

void RobotViewerScene::slotNavInitPos()
{
    if (robot_map_) {
        robot_map_->slot_set_init_pos();
    }
}

void RobotViewerScene::slotNavToGoal()
{
    if (robot_map_) {
        robot_map_->slot_set_nav_goal();
    }
}

void RobotViewerScene::slotNavToMutilGoal()
{
    if (robot_map_) {
        robot_map_->slot_set_nav_goal();
    }
}

void RobotViewerScene::slotNavToRest()
{
    if (robot_map_) {
        robot_map_->slot_set_move_camera();
      }
}

void RobotViewerScene::slotSetSelect()
{
    if (robot_map_) {
        robot_map_->slot_set_move_camera();
      }
}

void RobotViewerScene::slotMoveCamera()
{
    if (robot_map_) {
        robot_map_->slot_set_move_camera();
    }
}

void RobotViewerScene::slotUpdateMaps(const QImage &data)
{
  if (robot_map_) {
    robot_map_->paint_maps(data);
  }
}

void RobotViewerScene::slotUpdatePlannerPath(const QPolygonF &data)
{
  if (robot_map_) {
#ifdef ENABLE_TF
    QPolygonF scene_data;
    for(int i = 0; i < data.size(); i++) {
      QPointF pt = transform_map_to_scene(QPointF(data[i].x(), data[i].y()));
      scene_data.append(pt);
    }
#else
    QPolygonF scene_data(data);
#endif
    robot_map_->paint_planner_path(scene_data);
  }
}

void RobotViewerScene::slotUpdateOccupancyGrid(const occupancy_grid_t &data)
{
  map_width_ = data.width;
  map_height_ = data.height;
  map_resolution_ = data.resolution;

  map_originX_ = data.origin.position.x;
  map_originY_ = data.origin.position.y;

  //地图中心点坐标
  map_center_point_.setX(map_originX_ + map_resolution_ * map_width_ * 0.5);
  map_center_point_.setY(map_originY_ + map_resolution_ * map_height_ * 0.5);

  //场景中心点坐标
  scene_center_point_.setX(map_width_ / 2.0);
  scene_center_point_.setY(map_height_ / 2.0);
  
#if 0
  QGraphicsView *view = views()[0];
  view->fitInView(scene_center_point_.x(), scene_center_point_.y(), map_width_, map_height_, Qt::KeepAspectRatio);
#endif
}

QPointF RobotViewerScene::transform_scene_to_map(const QPointF &pos)
{
  QPointF robot_pos;
  robot_pos.setX((pos.x() - scene_center_point_.x()) * map_resolution_ +
               map_center_point_.x());
  robot_pos.setY(-1 * (pos.y() - scene_center_point_.y()) * map_resolution_ +
               map_center_point_.y());
  return robot_pos;
}

QPointF RobotViewerScene::transform_map_to_scene(const QPointF &pos)
{
  QPointF robot_pos;
  robot_pos.setX((pos.x() - map_center_point_.x()) / map_resolution_ +
               scene_center_point_.x());
  robot_pos.setY(-1 * (pos.y() - map_center_point_.y()) / map_resolution_ +
               scene_center_point_.y());
  return robot_pos;
}