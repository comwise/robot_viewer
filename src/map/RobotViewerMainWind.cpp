#include "RobotViewerMainWind.h"
#include "common/count_time.hpp"
#include <QTextCodec>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QSystemTrayIcon>
#include <QDebug>
#include "RobotViewerConfig.h"
#include "ItemPolygon.h"
#include "ItemMap.h"
#include "DialogFind.h"
#include "DialogUploadFile.h"
#include "DockEdge.h"
#include "DockSite.h"
#include "DockSimulator.h"
#include "RobotViewerScene.h"
#include "RobotViewerView.h"
#include "RobotViewerStick.h"
#include "RobotViewerNavigation.h"
#include "common/custom_widget.h"
#include "camera/camera_widget.h"

namespace {
static const std::string VERSION_MAJOY = "0";
static const std::string VERSION_MINOR = "1";
static const std::string VERSION_REVISION = "0";
static const std::string VERSION_BUILD = "0";
}

#define SAFE_DELETE(p) \
  if(p) { delete p; } \
  p = nullptr;

RobotViewerMainWind::RobotViewerMainWind(const std::string &file)
    : config_file_(file.c_str())
    , lbl_mousepos_(NULL)
    , lbl_robotpos_(NULL)
    , lbl_battary_(NULL)
    , is_map_draw_(false)
    , battary_volt_(0.0)
    , control_stick_(new RobotViewerStick)
    , is_stick_press_(true)
    , operate_type_(ACTION_SELECT)
{
    init();
}

RobotViewerMainWind::~RobotViewerMainWind()
{
    deinit();
}

void RobotViewerMainWind::init()
{
    initMembers();
    initForm();
    initSlots();
}

void RobotViewerMainWind::deinit()
{
    is_close_ = true;
    suspend_type_ = true;

    SAFE_DELETE(nav_task_)

    // camera dock
    SAFE_DELETE(camera_action_)
    SAFE_DELETE(camera_wgt_)
    SAFE_DELETE(camera_dock_)

    // edge dock
    SAFE_DELETE(edge_scan_action_)
    SAFE_DELETE(edge_tablewidget_)
    SAFE_DELETE(edge_dock_)

    // site dock
    SAFE_DELETE(site_scan_action_)
    SAFE_DELETE(site_tablewidget_)
    SAFE_DELETE(site_dock_)

    //map editor
    SAFE_DELETE(file_update_map_action_)
    SAFE_DELETE(file_edit_map_action_)

    //simulate dock
    SAFE_DELETE(simulator_action_)
    SAFE_DELETE(simulator_widge_)
    SAFE_DELETE(simulator_dock_)
}

void RobotViewerMainWind::initForm()
{
    createTitle();      //标题栏
    createActions();    //创建动作
    //createMenus();      //创建菜单窗口
    createToolBars();   //创建工具栏
    createCentralWnd(); //创建主窗体
    createDockWindows();//创建停靠窗口
    createStatusBar();  //创建状态栏
}

void RobotViewerMainWind::initMembers()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

    qRegisterMetaType<QVector<UPL>>("QVector<UPL>");

    is_close_ = false;
    suspend_type_ = true;

    map_2d_ = new RobotViewerMapConfig;
    find_dialog_ = new DialogFind(this);
    nav_task_ = new RobotViewerNavigation;

    scene_ = new RobotViewerScene(this);
    view_ = new RobotViewerView(this);
    view_->setScene(scene_);
    view_->updateSceneScale();
}

void RobotViewerMainWind::initSlots()
{
    //mouse
    connect(scene_, &RobotViewerScene::sigMousePosChanged, 
        this, &RobotViewerMainWind::slotMousePos);
    connect(scene_, &RobotViewerScene::sigRightMousePressed, 
        this, &RobotViewerMainWind::slotMouseRightPress);

    connect(view_, &RobotViewerView::sigMovePosChanged,
        this, &RobotViewerMainWind::slotMovePos);
    connect(view_, &RobotViewerView::sigScaleChanged,
        this, &RobotViewerMainWind::slotScale);


    // map
    connect(scene_, &RobotViewerScene::sigInitPos,
        this, [=](const algo::RobotPose &data) { emit sigInitPos(data); });
    connect(scene_, &RobotViewerScene::sigSetGoal,
        this, [=](const algo::RobotPose &data) { emit sigSetGoal(data); });

    //nav
    connect(nav_task_, SIGNAL(TaskFinish(bool)),
            this, SLOT(slotNavigationTaskFinish(bool)));
    connect(nav_task_, SIGNAL(TaskError(QVector<UPL>, int)),
            this, SLOT(slotNavigationError(QVector<UPL>, int)));

    // polu
    auto tree_clicked_cb = [&] () {
#if 0
        slotFileOpenImage(event.camera.path);
        if(scene_) {
            scene_->slotSetPoly(event.data);
        }
        if(poly_prop_) {
            StorageEvent e;
            e.camera = event.camera;
            poly_prop_->setData(e);
        }
#endif
    };

    auto poly_clicked_cb = [=]() {
        QList<QGraphicsItem *> lst;
        if(scene_) {
            lst = scene_->selectedItems();
        }
        if(lst.size() != 1) {
            return;
        }
        ItemPolygon *item = dynamic_cast<ItemPolygon *>(lst.front());
        if(item && poly_prop_) {
            //poly_prop_->setData(item->getData());
        }
    };

    auto storage_changed_cb = [=](const StorageEvent &event) {
#if 0
        QList<QGraphicsItem *> lst;
        if(scene_) {
            lst = scene_->selectedItems();
        }
        if(lst.size() == 1) {
#if 0
            ItemPolygon *item = dynamic_cast<ItemPolygon *>(lst.front());
            if(item) {
                if(event.data.feature == 1) {
                    item->setData(PolygonData());
                } else {
                    item->setData(event.data);
                }
            }
#endif
        }

        if(poly_prop_) {
            if(event.data.feature == 1) {
                poly_prop_->setData(PolygonData());
            } else if(!event.data.data.empty()){
                poly_prop_->setData(event.data);
            }
        }
        if(poly_widget_) {
            QString id = poly_widget_->get_id();
            PolyCamera cam(event.camera);
            cam.ip = id;
            cam.name = id;
            poly_widget_->save_config(id, cam);
            poly_widget_->save_config(id, event.data);
        }
#endif
    };

#if 0
    connect(poly_widget_, &StorageManage::storageClicked, this, tree_clicked_cb);

    connect(scene_, &RobotViewerScene::selectionChanged, this, poly_clicked_cb);

    connect(poly_prop_, &PolyProp::storageChanged, this, storage_changed_cb);
    connect(scene_, &RobotViewerScene::storageChanged, this, storage_changed_cb);
#endif

    startTimer(200);
}

void RobotViewerMainWind::createTitle()
{
    setWindowIcon(QIcon(":/images/robot.ico"));
    setWindowTitle(tr("机器人仿真软件"));
    QCoreApplication::setOrganizationName("ComWise");
    QCoreApplication::setOrganizationDomain("comwise.com");
    QCoreApplication::setApplicationName("RobotViewer");
}

void RobotViewerMainWind::createActions()
{
    //文件
    file_open_map_action_ = new QAction(QIcon(QString::fromUtf8(":/images/loadmap.png")), tr("打开地图"), this);
    file_open_map_action_->setShortcut(tr("Ctrl+Alt+O"));
    connect(file_open_map_action_, SIGNAL(triggered()), this, SLOT(slotFileOpenMap()));

    file_edit_map_action_ = new QAction(QIcon(QString::fromUtf8(":/images/map_edit.png")), tr("编辑地图"), this);
    file_edit_map_action_->setShortcut(QKeySequence(tr("Ctrl+J")));
    connect(file_edit_map_action_, SIGNAL(triggered()), this, SLOT(slotFileEditMap()));

    file_update_map_action_ = new QAction(QIcon(QString::fromUtf8(":/images/map_file.png")), tr("更新地图"), this);
    file_update_map_action_->setShortcut(QKeySequence(tr("Ctrl+G")));
    connect(file_update_map_action_, SIGNAL(triggered()), this, SLOT(slotFileUpdateMap()));

    file_new_action_ = new QAction(QIcon(QString::fromUtf8(":/images/new.png")), tr("新建文件"), this);
    file_new_action_->setShortcut(tr("Ctrl+N"));
    file_new_action_->setEnabled(false);
    connect(file_new_action_, SIGNAL(triggered()), this, SLOT(slotFileNew()));

    file_open_action_ = new QAction(QIcon(QString::fromUtf8(":/images/open.png")), tr("打开配置文件"), this);
    file_open_action_->setShortcut(tr("Ctrl+O"));
    connect(file_open_action_, SIGNAL(triggered()), this, SLOT(slotFileOpen()));

    file_save_action_ = new QAction(QIcon(QString::fromUtf8(":/images/save.png")), tr("保存文件"), this);
    file_save_action_->setShortcut(tr("Ctrl+S"));
    connect(file_save_action_, SIGNAL(triggered()), this, SLOT(slotFileSave()));

    file_save_as_action_ = new QAction(QIcon(QString::fromUtf8(":/images/saveas.png")), tr("另存为"), this);
    file_save_as_action_->setShortcut(tr("Ctrl+Shift+S"));
    connect(file_save_as_action_, SIGNAL(triggered()), this, SLOT(slotFileSaveAs()));

    file_merger_action_ = new QAction(QIcon(QString::fromUtf8(":/images/merge.png")), tr("合并文件"), this);
    file_merger_action_->setShortcut(tr("Ctrl+M"));
    connect(file_merger_action_, SIGNAL(triggered()), this, SLOT(slotFileMerge()));

    file_upload_config_action_ = new QAction(QIcon(QString::fromUtf8(":/images/upload.png")), tr("上传配置文件"), this);
    file_upload_config_action_->setShortcut(tr("Ctrl+U"));
    connect(file_upload_config_action_, SIGNAL(triggered()), this, SLOT(slotFileUpload()));

    file_download_config_action_ = new QAction(QIcon(QString::fromUtf8(":/images/download.png")), tr("下载配置文件"), this);
    file_download_config_action_->setShortcut(tr("Ctrl+D"));
    connect(file_download_config_action_, SIGNAL(triggered()), this, SLOT(slotFileDownload()));

    file_exit_action_ = new QAction(QIcon(QString::fromUtf8(":/images/close.png")), tr("退出"), this);
    file_exit_action_->setShortcut(tr("Ctrl+Q"));
    connect(file_exit_action_, SIGNAL(triggered()), this, SLOT(slotFileExit()));

    //编辑
    view_select_action_ = new QAction(QIcon(QString::fromUtf8(":/images/pointer.png")), tr("选择"), this);
    view_select_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S));
    view_select_action_->setCheckable(true);
    view_select_action_->setChecked(true);
    connect(view_select_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateSelect(bool)));

    view_move_action_ = new QAction(QIcon(QString::fromUtf8(":/images/move.png")), tr("移动视图"), this);
    view_move_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_L));
    view_move_action_->setCheckable(true);
    connect(view_move_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateMove(bool)));

    view_drag_action_ = new QAction(QIcon(QString::fromUtf8(":/images/cursor_openhand.png")), tr("拖拽视图"), this);
    view_drag_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_M));
    view_drag_action_->setCheckable(true);
    connect(view_drag_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateHand(bool)));

    view_zoomin_action_ = new QAction(QIcon(QString::fromUtf8(":/images/zoom_in.png")), tr("缩小视图"), this);
    connect(view_zoomin_action_, SIGNAL(triggered()), this, SLOT(slotOperateZoomIn()));

    view_zoomout_action_ = new QAction(QIcon(QString::fromUtf8(":/images/zoom_out.png")), tr("放大视图"), this);
    connect(view_zoomout_action_, SIGNAL(triggered()), this, SLOT(slotOperateZoomOut()));

    operation_node_action_ = new QAction(QIcon(QString::fromUtf8(":/images/node.png")), tr("增加节点"), this);
    operation_node_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_N));
    operation_node_action_->setCheckable(true);
    connect(operation_node_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateNode(bool)));

    operation_edge_action_ = new QAction(QIcon(QString::fromUtf8(":/images/curve.png")), tr("增加边"), this);
    operation_edge_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_E));
    operation_edge_action_->setCheckable(true);
    connect(operation_edge_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateEdge(bool)));

    operation_site_action_ = new QAction(QIcon(QString::fromUtf8(":/images/site.png")), tr("增加任务点"), this);
    operation_site_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_I));
    operation_site_action_->setCheckable(true);
    connect(operation_site_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateSite(bool)));

    operation_loc_site_action_ = new QAction(QIcon(QString::fromUtf8(":/images/locsite.png")), tr("标定任务点"), this);
    operation_loc_site_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_D));
    connect(operation_loc_site_action_, SIGNAL(triggered()), this, SLOT(slotOperateLocSite()));

    operation_loc_node_action_ = new QAction(QIcon(QString::fromUtf8(":/images/locnode.png")), tr("标定节点"), this);
    operation_loc_node_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F));
    connect(operation_loc_node_action_, SIGNAL(triggered()), this, SLOT(slotOperateLocNode()));

    operation_area_load_action_ = new QAction(QIcon(QString::fromUtf8(":/images/rect_load.png")), tr("加载图像"), this);
    connect(operation_area_load_action_, &QAction::triggered, this, [=]() {
        slotFileOpenRectMap();
    });

    operation_area_edit_action_ = new QAction(QIcon(QString::fromUtf8(":/images/rect_edit.png")), tr("增加矩形"), this);
    operation_area_edit_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_A));
    operation_area_edit_action_->setCheckable(true);
    connect(operation_area_edit_action_, &QAction::toggled, this, [=](bool checked) {
        operationAction(checked, ADD_AREA);
    });

    operation_area_ok_action_ = new QAction(QIcon(QString::fromUtf8(":/images/rect_ok.png")), tr("应用矩形"), this);
    connect(operation_area_ok_action_, &QAction::triggered, this, [=]() {
        slotFileUpdateMap();
    });

    operation_poly_edit_action_ = new QAction(QIcon(QString::fromUtf8(":/images/poly_edit.png")), tr("增加多边形"), this);
    operation_poly_edit_action_->setCheckable(true);
    connect(operation_poly_edit_action_, &QAction::toggled, this, [=](bool checked) {
        operationAction(checked, ADD_POLYGON);
    });

    operation_poly_ok_action_ = new QAction(QIcon(QString::fromUtf8(":/images/poly_ok.png")), tr("应用多边形"), this);
    connect(operation_poly_ok_action_, &QAction::triggered, this, [=]() {
        if(scene_) scene_->confirmPloygonItem();
#if 0
        if(poly_widget_) {
            QString id = poly_widget_->get_id();
            if(id.isEmpty()) {
                QMessageBox::warning(this, tr("保存多边形配置错误"), tr("未选择库位图片"));
                return;
            }
            bool ret = true;
            QVector<PolygonData> data;
            if(scene_) {
                scene_->slotGetPoly(data);
            }
            ret &= poly_widget_->save_config(id, data);
            QMessageBox::warning(this,
                tr("保存库位配置"), tr("保存库位（%1）配置%2").arg(id).arg(ret?"成功":"失败"));
        }
#endif
    });

    operation_poly_undo_action_ = new QAction(QIcon(QString::fromUtf8(":/images/poly_undo.png")), tr("撤销多边形"), this);
    connect(operation_poly_undo_action_, &QAction::triggered, this, [=]() {
        if(scene_) scene_->undoPloygonItem();
    });

    //命令操作
    edit_undo_action_       = new QAction(QIcon(QString::fromUtf8(":/images/undo.png")),    tr("撤销"), this);
    edit_redo_action_       = new QAction(QIcon(QString::fromUtf8(":/images/redo.png")),    tr("恢复"), this);
    edit_cut_action_        = new QAction(QIcon(QString::fromUtf8(":/images/cut.png")),     tr("剪贴"), this);
    edit_copy_action_       = new QAction(QIcon(QString::fromUtf8(":/images/copy.png")),    tr("复制"), this);
    edit_paste_action_      = new QAction(QIcon(QString::fromUtf8(":/images/paste.png")),   tr("粘贴"), this);
    edit_delete_action_     = new QAction(QIcon(QString::fromUtf8(":/images/delete.png")),  tr("删除"), this);
    edit_find_action_       = new QAction(QIcon(QString::fromUtf8(":/images/find.png")),    tr("查找"), this);
    edit_select_all_action_ = new QAction(QIcon(QString::fromUtf8(":/images/selectall.png")),tr("全选"), this);

    edit_delete_action_->setShortcut(tr("Delete"));
    connect(edit_delete_action_, SIGNAL(triggered()), this, SLOT(slotEditDelete()));
    
    edit_find_action_->setShortcut(tr("Ctrl+F"));
    connect(edit_find_action_, SIGNAL(triggered()), this, SLOT(slotEditFind()));

    edit_select_all_action_->setShortcut(tr("Ctrl+A"));
    connect(edit_select_all_action_, SIGNAL(triggered()), this, SLOT(slotEditSelectAll()));

    //导航动作1
    nav_init_location_ = new QAction(QIcon(QString::fromUtf8(":/images/location.png")), tr("初始定位"), this);
    nav_init_location_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_O));
    nav_init_location_->setCheckable(true);
    connect(nav_init_location_, SIGNAL(toggled(bool)), this, SLOT(slotOperateLocation(bool)));

    nav_target_action_ = new QAction(QIcon(QString::fromUtf8(":/images/navigation.png")), tr("设置目标点"), this);
    nav_target_action_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_T));
    nav_target_action_->setCheckable(true);
    connect(nav_target_action_, SIGNAL(toggled(bool)), this, SLOT(slotOperateTargetPt(bool)));

    nav_start_action_ = new QAction(QIcon(QString::fromUtf8(":/images/start.png")), tr("开始任务"), this);
    nav_start_action_->setShortcut(QKeySequence(Qt::Key_F1));
    connect(nav_start_action_, SIGNAL(triggered()), this, SLOT(slotStartNavigation()));
    nav_start_action_->setDisabled(false);

    nav_suspend_action_ = new QAction(QIcon(QString::fromUtf8(":/images/suspend.png")), tr("暂停任务"), this);
    nav_suspend_action_->setShortcut(QKeySequence(Qt::Key_F2));
    connect(nav_suspend_action_, SIGNAL(triggered(bool)), this, SLOT(slotSuspendNavigation(bool)));
    nav_suspend_action_->setDisabled(true);

    nav_stop_action_ = new QAction(QIcon(QString::fromUtf8(":/images/stop.png")), tr("停止任务"), this);
    nav_stop_action_->setShortcut(QKeySequence(Qt::Key_F3));
    connect(nav_stop_action_, SIGNAL(triggered()), this, SLOT(slotStopNavigation()));
    nav_stop_action_->setDisabled(true);

    nav_reset_action_ = new QAction(QIcon(QString::fromUtf8(":/images/correct.png")), tr("重置导航"), this);
    nav_reset_action_->setShortcut(QKeySequence(Qt::Key_F4));
    nav_reset_action_->setDisabled(false);
    connect(nav_reset_action_, SIGNAL(triggered(bool)), this, SLOT(slotResetNavigation()));

    nav_speed_up_action_ = new QAction(QIcon(QString::fromUtf8(":/images/speedup.png")), tr("加速"), this);
    nav_speed_up_action_->setShortcut(QKeySequence(Qt::Key_F5));
    connect(nav_speed_up_action_, SIGNAL(triggered()), this, SLOT(slotNavSpeedUp()));

    nav_speed_down_action_ = new QAction(QIcon(QString::fromUtf8(":/images/speeddown.png")), tr("减速"), this);
    nav_speed_down_action_->setShortcut(QKeySequence(Qt::Key_F6));
    connect(nav_speed_down_action_, SIGNAL(triggered()), this, SLOT(slotNavSpeedDown()));

    //导航动作2
    nav_init_pos_action_ = new QAction(QIcon(QString::fromUtf8(":/images/set_init_pos.png")), tr("设置初始点"), this);
    nav_init_pos_action_->setCheckable(true);
    connect(nav_init_pos_action_, SIGNAL(toggled(bool)), this, SLOT(slotNavInitPos(bool)));

    nav_set_goal_action_ = new QAction(QIcon(QString::fromUtf8(":/images/set_pose.png")), tr("设置单点导航"), this);
    connect(nav_set_goal_action_, SIGNAL(triggered()), this, SLOT(slotNavToGoal()));

    nav_set_mutil_goal_action_ = new QAction(QIcon(QString::fromUtf8(":/images/set_mutil_pose.png")), tr("设置多点导航"), this);
    connect(nav_set_mutil_goal_action_, SIGNAL(triggered()), this, SLOT(slotNavToMutilGoal()));

    nav_set_rest_action_ = new QAction(QIcon(QString::fromUtf8(":/images/set_return.png")), tr("返回休息点"), this);
    connect(nav_set_rest_action_, SIGNAL(triggered()), this, SLOT(slotNavToRestGoal()));

    //显示
    show_map_action_ = new QAction(QIcon(QString::fromUtf8(":/images/classes/Map.png")), tr("显示地图"), this);
    show_map_action_->setCheckable(true);
    show_map_action_->setChecked(true);
    connect(show_map_action_, SIGNAL(toggled(bool)), this, SLOT(slotShowMap(bool)));

    show_laser_action_ = new QAction(QIcon(QString::fromUtf8(":/images/classes/LaserScan.png")), tr("显示激光"), this);
    show_laser_action_->setCheckable(true);
    show_laser_action_->setChecked(true);
    connect(show_laser_action_, SIGNAL(toggled(bool)), this, SLOT(slotShowLaser(bool)));

    track_robot_action_ = new QAction(QIcon(QString::fromUtf8(":/images/classes/RobotModel.png")), tr("跟踪机器人"), this);
    track_robot_action_->setCheckable(true);
    connect(track_robot_action_, SIGNAL(toggled(bool)), this, SLOT(slotTrackRobot(bool)));

    //实施
    action_oper_remote_ = new QAction(QIcon(QString::fromUtf8(":/images/arrowpad.png")), tr("手动遥控"), this);
    action_oper_remote_->setShortcut(QKeySequence(Qt::ALT + Qt::Key_K));
    action_oper_remote_->setCheckable(true);
    connect(action_oper_remote_, SIGNAL(toggled(bool)), this, SLOT(slotOperateRemoteCtrl(bool)));

    camera_action_ = new QAction(QIcon(QString::fromUtf8(":/images/camare.png")), tr("相机"), this);
    camera_action_->setShortcut(QKeySequence(Qt::Key_F7));
    camera_action_->setCheckable(true);
    connect(camera_action_, SIGNAL(toggled(bool)), this, SLOT(slotDockCamera(bool)));

    site_scan_action_ = new QAction(QIcon(QString::fromUtf8(":/images/siteTab.png")), tr("任务点"), this);
    site_scan_action_->setShortcut(QKeySequence(Qt::Key_F8));
    site_scan_action_->setCheckable(true);
    connect(site_scan_action_, SIGNAL(toggled(bool)), this, SLOT(slotDockSite(bool)));

    edge_scan_action_ = new QAction(QIcon(QString::fromUtf8(":/images/edgeTab.png")), tr("任务路线"), this);
    edge_scan_action_->setShortcut(QKeySequence(Qt::Key_F9));
    edge_scan_action_->setCheckable(true);
    connect(edge_scan_action_, SIGNAL(toggled(bool)), this, SLOT(slotDockEdge(bool)));

    simulator_action_ = new QAction(QIcon(QString::fromUtf8(":/images/simulator.png")), tr("底盘仿真"), this);
    simulator_action_->setShortcut(QKeySequence(Qt::Key_F10));
    simulator_action_->setCheckable(true);
    connect(simulator_action_, SIGNAL(toggled(bool)), this, SLOT(slotDockSimulate(bool)));

    poly_action_ = new QAction(QIcon(QString::fromUtf8(":/images/grid.png")), tr("多边形管理"), this);
    poly_action_->setCheckable(true);
    connect(poly_action_, SIGNAL(toggled(bool)), this, SLOT(slotDockPoly(bool)));

    std::string ver("版本 ");
    ver += VERSION_MAJOY + ".";
    ver += VERSION_MINOR + ".";
    ver += VERSION_REVISION + ".";
    ver += VERSION_BUILD;
    help_about_ = new QAction(ver.c_str(), this);
}

void RobotViewerMainWind::createMenus()
{
    QMenuBar *menu_bar = menuBar();
    QMenu *file_menu = menu_bar->addMenu(tr("文件"));
    file_menu->addAction(file_open_map_action_);
    //file_menu->addAction(file_edit_map_action_);
    file_menu->addAction(file_update_map_action_);
    file_menu->addSeparator();
    file_menu->addAction(file_new_action_);
    file_menu->addAction(file_open_action_);
    file_menu->addSeparator();
    file_menu->addAction(file_save_action_);
    file_menu->addAction(file_save_as_action_);
    // file_menu->addAction(file_merger_action_);
    file_menu->addSeparator();
    // file_menu->addAction(file_open_mode_action_);
    file_menu->addSeparator();
    file_menu->addAction(file_upload_config_action_);
    file_menu->addAction(file_download_config_action_);
    file_menu->addSeparator();
    file_menu->addAction(file_exit_action_);

    QMenu *edit_menu = menu_bar->addMenu(tr("编辑"));
    edit_menu->addAction(edit_undo_action_);
    edit_menu->addAction(edit_redo_action_);
    edit_menu->addSeparator();
    edit_menu->addAction(edit_cut_action_);
    edit_menu->addAction(edit_copy_action_);
    edit_menu->addAction(edit_paste_action_);
    edit_menu->addAction(edit_delete_action_);
    edit_menu->addSeparator();
    edit_menu->addAction(edit_select_all_action_);
    edit_menu->addSeparator();
    edit_menu->addAction(edit_find_action_);

    QMenu *view_menu = menu_bar->addMenu(tr("视图"));
    view_menu->addAction(view_select_action_);
    view_menu->addAction(view_move_action_);
    view_menu->addAction(view_drag_action_);
    view_menu->addSeparator();
    view_menu->addAction(view_zoomin_action_);
    view_menu->addAction(view_zoomout_action_);
    
    QMenu *route_menu = menu_bar->addMenu(tr("路线"));
    route_menu->addSeparator();
    route_menu->addAction(operation_node_action_);
    route_menu->addAction(operation_site_action_);
    route_menu->addAction(operation_edge_action_);
    route_menu->addAction(operation_area_load_action_);
    route_menu->addAction(operation_area_edit_action_);
    route_menu->addAction(operation_area_ok_action_);
    route_menu->addAction(operation_poly_edit_action_);
    //route_menu->addAction(operation_poly_ok_action_);
    route_menu->addAction(operation_poly_undo_action_);
    route_menu->addSeparator();
    route_menu->addAction(operation_loc_site_action_);
    route_menu->addAction(operation_loc_node_action_);
    
    QMenu *nav_menu = menu_bar->addMenu(tr("导航"));
    nav_menu->addAction(nav_init_location_);
    nav_menu->addSeparator();
    nav_menu->addAction(nav_target_action_);
    nav_menu->addAction(nav_start_action_);
    nav_menu->addAction(nav_suspend_action_);
    nav_menu->addAction(nav_stop_action_);
    nav_menu->addAction(nav_reset_action_);
    nav_menu->addSeparator();
    nav_menu->addAction(nav_speed_up_action_);
    nav_menu->addAction(nav_speed_down_action_);

    QMenu *wgt_menu = menu_bar->addMenu(tr("插件"));
    wgt_menu->addAction(action_oper_remote_);
    wgt_menu->addSeparator();
    wgt_menu->addAction(site_scan_action_);
    wgt_menu->addAction(edge_scan_action_);
    wgt_menu->addSeparator();
    //wgt_menu->addAction(camera_action_);
    //wgt_menu->addAction(poly_action_);
    wgt_menu->addAction(simulator_action_);

    QMenu *help_menu = menu_bar->addMenu(tr("帮助"));
    help_menu->addAction(help_about_);
}

#define TEST_ROUTE
#define TEST_NAV
void RobotViewerMainWind::createToolBars()
{
    // file action
    QToolBar *file_toolbar = addToolBar(tr("文件"));
    file_toolbar->addAction(file_open_map_action_);
    //file_toolbar->addAction(file_edit_map_action_);
    //file_toolbar->addAction(file_update_map_action_);
#if 0
    file_toolbar->addSeparator();
    file_toolbar->addAction(file_new_action_);
    file_toolbar->addAction(file_open_action_);
#endif
    file_toolbar->addAction(file_save_action_);
    file_toolbar->addAction(file_save_as_action_);
#if 0
    file_toolbar->addSeparator();
    // file_toolbar->addAction(file_merger_action_);
    // file_toolbar->addAction(file_open_mode_action_);
    file_toolbar->addSeparator();
    file_toolbar->addAction(file_upload_config_action_);
    file_toolbar->addAction(file_download_config_action_);
#endif

    // edit action
    QToolBar *edit_toolbar = addToolBar(tr("编辑"));
#ifdef TEST_EDIT
    edit_toolbar->addAction(edit_undo_action_);
    edit_toolbar->addAction(edit_redo_action_);
    edit_toolbar->addSeparator();
    edit_toolbar->addAction(edit_cut_action_);
    edit_toolbar->addAction(edit_copy_action_);
    edit_toolbar->addAction(edit_paste_action_);
#endif
    edit_toolbar->addAction(edit_delete_action_);
    edit_toolbar->addSeparator();
    edit_toolbar->addAction(edit_select_all_action_);
    edit_toolbar->addSeparator();
    edit_toolbar->addAction(edit_find_action_);

    // route action
    QToolBar *route_toolbar = addToolBar(tr("路线"));
#ifdef TEST_ROUTE
    route_toolbar->addAction(operation_node_action_);
    route_toolbar->addAction(operation_site_action_);
    route_toolbar->addAction(operation_edge_action_);
#endif
    //route_toolbar->addAction(operation_area_load_action_);
    route_toolbar->addAction(operation_area_edit_action_);
    route_toolbar->addAction(operation_area_ok_action_);
    route_toolbar->addSeparator();
    route_toolbar->addAction(operation_poly_edit_action_);
    route_toolbar->addAction(operation_poly_undo_action_);
    //route_toolbar->addAction(operation_poly_ok_action_);

#ifdef TEST_ROUTE
    route_toolbar->addSeparator();
    route_toolbar->addAction(operation_loc_site_action_);
    route_toolbar->addAction(operation_loc_node_action_);
#endif

    // view action
    QToolBar *view_toolbar = addToolBar(tr("视图"));
    view_toolbar->addAction(view_select_action_);
    view_toolbar->addAction(view_move_action_);
    view_toolbar->addAction(view_drag_action_);
    view_toolbar->addSeparator();
    view_toolbar->addAction(view_zoomin_action_);
    view_toolbar->addAction(view_zoomout_action_);
    view_toolbar->addSeparator();
    view_toolbar->addAction(show_map_action_);
    view_toolbar->addAction(show_laser_action_);
    view_toolbar->addAction(track_robot_action_);

#ifdef TEST_NAV
    // nav action
    QToolBar *nav_toolbar = addToolBar(tr("导航"));
    nav_toolbar->addAction(nav_init_location_);
    nav_toolbar->addAction(nav_target_action_);
    nav_toolbar->addAction(nav_start_action_);
    nav_toolbar->addAction(nav_suspend_action_);
    nav_toolbar->addAction(nav_stop_action_);
    nav_toolbar->addAction(nav_reset_action_);
    nav_toolbar->addAction(nav_speed_up_action_);
    nav_toolbar->addAction(nav_speed_down_action_);
    nav_toolbar->addSeparator();
    nav_toolbar->addAction(nav_init_pos_action_);
    nav_toolbar->addAction(nav_set_goal_action_);
    nav_toolbar->addAction(nav_set_mutil_goal_action_);
    nav_toolbar->addAction(nav_set_rest_action_);
#endif

    // dock action
    QToolBar *dock_toolbar = addToolBar(tr("插件"));
#ifdef TEST_ROUTE
    dock_toolbar->addAction(action_oper_remote_);
    dock_toolbar->addSeparator();
    dock_toolbar->addAction(site_scan_action_);
    dock_toolbar->addAction(edge_scan_action_);
    dock_toolbar->addSeparator();
#endif
    //dock_toolbar->addAction(camera_action_);
    //dock_toolbar->addAction(poly_action_);
    dock_toolbar->addAction(simulator_action_);
}

void RobotViewerMainWind::createDockWindows()
{
    setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks | ForceTabbedDocks | VerticalTabs);

    camera_dock_ = new CDockWidget(tr("相机"), this);
    camera_wgt_ = new CameraWidget(config_file_, this);
    camera_dock_->setAllowedAreas(Qt::AllDockWidgetAreas);
    //camera_dock_->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    camera_dock_->setWidget(camera_wgt_);
    camera_dock_->hide();
    connect(camera_wgt_, &CameraWidget::subscribeImage, this,
        [&] (int frame, const QString &topic, bool enable) {
            return emit sigSubscribeImage(camera_sub_t(topic.toStdString(), frame, enable)); 
        });
    connect(camera_dock_, &CDockWidget::sigCloseEvent, this,
        [&] (QCloseEvent *event) {
            if(camera_action_) camera_action_->setChecked(false); 
        });
    addDockWidget(Qt::LeftDockWidgetArea, camera_dock_);

    site_dock_ = new CDockWidget(tr("任务点"), this);
    site_tablewidget_ = new DockSite(this);
    site_tablewidget_->SetScene(scene_);
    site_dock_->setAllowedAreas(Qt::RightDockWidgetArea);
    site_dock_->setWidget(site_tablewidget_);
    site_dock_->hide();
    connect(site_dock_, &CDockWidget::sigCloseEvent, this,
        [&] (QCloseEvent *) {
            if(site_scan_action_) site_scan_action_->setChecked(false); 
        });
    addDockWidget(Qt::RightDockWidgetArea, site_dock_);

    edge_dock_ = new CDockWidget(tr("任务路线"), this);
    edge_tablewidget_ = new DockEdge(this);
    edge_tablewidget_->SetScene(scene_);
    edge_dock_->setAllowedAreas(Qt::RightDockWidgetArea);
    edge_dock_->setWidget(edge_tablewidget_);
    edge_dock_->hide();
    connect(edge_dock_, &CDockWidget::sigCloseEvent, this,
        [&] (QCloseEvent *) {
            if(edge_scan_action_) edge_scan_action_->setChecked(false); 
        });
    addDockWidget(Qt::RightDockWidgetArea, edge_dock_);

    simulator_dock_ = new CDockWidget(tr("仿真"), this);
    simulator_widge_ = new DockSimulator(this);
    simulator_dock_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    simulator_dock_->setWidget(simulator_widge_);
    simulator_dock_->hide();
    connect(simulator_dock_, &CDockWidget::sigCloseEvent, this,
        [&] (QCloseEvent *) {
            if(simulator_action_) simulator_action_->setChecked(false); 
        });
    addDockWidget(Qt::RightDockWidgetArea, simulator_dock_);
}

void RobotViewerMainWind::createCentralWnd()
{
#ifdef ENABLE_TABLE
    center_view_tab_ = new QTabWidget();
    center_view_tab_->setContentsMargins(0,0,0,0);
    setCentralWidget(center_view_tab_);
    center_view_tab_->addTab(view_, "untitled.cfg");
    center_view_tab_->setCurrentWidget(view_);
#else
    setCentralWidget(view_);
#endif
}

void RobotViewerMainWind::createStatusBar()
{
    //程序状态
    lbl_operate_ = new QLabel(tr(" 显示 "));
    lbl_operate_->setMinimumWidth(50);
    lbl_operate_->setScaledContents(true);

    //全局鼠标位置
    lbl_movepos_ = new QLabel(tr(" 全局鼠标: 0.00m, 0.00m "));
    lbl_movepos_->setMinimumWidth(100);
    lbl_movepos_->setScaledContents(true);

    //实时地图鼠标位置
    lbl_mousepos_ = new QLabel(tr(" 实时鼠标: 0.00m, 0.00m "));
    lbl_mousepos_->setMinimumWidth(100);
    lbl_mousepos_->setScaledContents(true);

    //机器人位置
    lbl_robotpos_ = new QLabel(tr(" 机器坐标: 0.00m, 0.00m, 0度 "));
    lbl_robotpos_->setMinimumWidth(160);
    lbl_robotpos_->setScaledContents(true);

    //机器人位置
    lbl_scale_ = new QLabel(tr(" 缩放: 100% "));
    lbl_scale_->setMinimumWidth(60);
    lbl_scale_->setScaledContents(true);

#if 0
    //电池
    lbl_battary_ = new QLabel;
    lbl_battary_->setContentsMargins(0, 0, 0, 0);
    lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/novolt.png")));
    lbl_battary_->setScaledContents(true);

    lbl_voltage_ = new QLabel(QString("0 V "));
    lbl_voltage_->setScaledContents(true);

    QWidget *vol_wgt = new QWidget(this);
    vol_wgt->setMinimumHeight(22);
    QHBoxLayout *vol_lyt = new QHBoxLayout(vol_wgt);
    vol_lyt->setContentsMargins(0, 0, 0, 0);
    vol_lyt->addWidget(lbl_battary_);
    vol_lyt->addWidget(lbl_voltage_);

    //地图
    lbl_map_ = new QLabel(tr("  Map: None  "));
    lbl_map_->setScaledContents(true);
#endif

    statusBar()->setFixedHeight(26);
    statusBar()->setContentsMargins(0,0,0,0);
    statusBar()->addWidget(lbl_operate_);
    statusBar()->addWidget(lbl_movepos_);
    statusBar()->addWidget(lbl_mousepos_);
    statusBar()->addWidget(lbl_robotpos_);
    statusBar()->addWidget(lbl_scale_);
    //statusBar()->addWidget(vol_wgt);
    //statusBar()->addWidget(lbl_map_);

    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(tr(":/image/setup.ico")));
    trayIcon->show();
}

void RobotViewerMainWind::closeEvent(QCloseEvent *event)
{
    (void)event;
    is_close_ = true;
    enableStick(false);
    emit closeWidget();
}

void RobotViewerMainWind::enableStick(bool checked)
{
    // qDebug() << "enable = " << checked << std::endl;
    is_stick_press_ = checked;

    if (is_stick_press_) {
        control_stick_->startThread();
    } else {
        control_stick_->pauseThread();
    }
}

void RobotViewerMainWind::keyPressEvent(QKeyEvent *e)
{
    QMainWindow::keyPressEvent(e);
}

void RobotViewerMainWind::keyReleaseEvent(QKeyEvent *e)
{
    if (operate_type_ != REMOTE_CONTROL)
    {
        QMainWindow::keyReleaseEvent(e);
        return;
    }

    switch (e->key())
    {
    case Qt::Key_W:
        control_stick_->setSpeed(BodyUp);
        break;
    case Qt::Key_S:
        control_stick_->setSpeed(BodyDown);
        break;
    case Qt::Key_A:
        control_stick_->setSpeed(BodyLeft);
        break;
    case Qt::Key_D:
        control_stick_->setSpeed(BodyRight);
        break;
    case Qt::Key_Q:
        control_stick_->setSpeed(BodyStop);
        break;
    case Qt::Key_I:
        control_stick_->setSpeed(CameraUp);
        break;
    case Qt::Key_K:
        control_stick_->setSpeed(CameraDown);
        break;
    case Qt::Key_O:
    {
        control_stick_->setSpeed(CameraZero);
        break;
    }
        break;
    default:
        break;
    }
    QMainWindow::keyReleaseEvent(e);
}

void RobotViewerMainWind::operationAction(bool checked, ActionMode mode)
{
    if (checked) {
        swichOperateType(mode);
    } else if (scene_->getInsertType() == mode) {
        swichOperateType(ACTION_SELECT);
    } else {
        //todo
    }
}

void RobotViewerMainWind::swichOperateType(ActionMode type)
{
    operate_type_ = type;
    enableStick(type == REMOTE_CONTROL);
    view_->setInsertType(type);

    if (type != ACTION_SELECT && view_select_action_->isChecked())
    {
        view_select_action_->setChecked(false);
    }
    if (type != ACTION_HANDDRAG && view_drag_action_->isChecked())
    {
        view_drag_action_->setChecked(false);
    }
    if (type != ACTION_MOVE && view_move_action_->isChecked())
    {
        view_move_action_->setChecked(false);
    }
    if (type != ADD_AREA && operation_area_edit_action_->isChecked())
    {
        operation_area_edit_action_->setChecked(false);
    }
    if (type != ADD_POLYGON && operation_poly_edit_action_->isChecked())
    {
        operation_poly_edit_action_->setChecked(false);
    }
    if (type != ADD_EDGE && operation_edge_action_->isChecked())
    {
        operation_edge_action_->setChecked(false);
    }
    if (type != ADD_NODE && operation_node_action_->isChecked())
    {
        operation_node_action_->setChecked(false);
    }
    if (type != ADD_SITE && operation_site_action_->isChecked())
    {
        operation_site_action_->setChecked(false);
    }
    if (type != SET_LOCROBOT && nav_init_location_->isChecked())
    {
        nav_init_location_->setChecked(false);
    }
    if (type != REMOTE_CONTROL && action_oper_remote_->isChecked())
    {
        action_oper_remote_->setChecked(false);
    }
    if (type != SET_NAVTARGET && nav_target_action_->isChecked())
    {
        nav_target_action_->setChecked(false);
    }
}

void RobotViewerMainWind::loadDataFromFile(const QString &file_name)
{
    std::string file;
    QTextCodec *code = QTextCodec::codecForLocale();
    if (code) {
        file = code->fromUnicode(file_name).data();
    }

    // route config
    ProjectInfo data;
    if(!PRJ_CFG->loadData(file, data)) {
        QMessageBox::warning(this, tr("加载配置文件错误"),tr("加载配置文件失败"));
        return;
    }

#if 0
    // protect config
    if(!PRJ_CFG->loadProtectData("protect.json", data.edge)) {
        QMessageBox::warning(this, tr("加载文件错误"),tr("加载防护配置文件失败"));
        return;
    }
#endif

    if(scene_) {
        scene_->addNodeItems(data.node);
        scene_->addEdgeItems(data.edge);
        scene_->addSiteItems(data.site);
        //scene_->addAreaItems(data.area);
    }
}

void RobotViewerMainWind::saveDataToFile(const QString &file_name)
{
    static bool m_value(true);
    if (!m_value) {
        return;
    }
    m_value = false;

    std::vector<NodeData> node_data;
    std::vector<EdgeData> edge_data;
    std::vector<SiteData> site_data;
    std::vector<AreaData> area_data;
    std::string file;
    QTextCodec *code = QTextCodec::codecForLocale();
    if (code) {
        file = code->fromUnicode(file_name).data();
    }

    ProjectInfo info;
    if (scene_->getNodeDatas(node_data)) {
        info.node = node_data;
    }

    if(scene_->getEdgeDatas(edge_data)) {
        info.edge = edge_data;
#if 0
        if(!PRJ_CFG.saveProtectData("protect.json", edge_data)){
            QMessageBox::warning(this, tr("保存文件错误"),tr("保存防护配置文件失败"));
        }
#endif
    }

    if (scene_->getSiteDatas(site_data)) {
        info.site = site_data;
    }

    if (scene_->getAreaDatas(area_data)) {
        //info.area = area_data;
        if (map_area_cfg_.empty()) {
            m_value = true;
            QMessageBox::warning(this, tr("保存文件错误"),tr("区域配置文件路径为空"));
            return;
        }

        if (!PRJ_CFG->saveData(map_area_cfg_, area_data)) {
            QMessageBox::warning(this, tr("保存文件错误"),tr("保存区域配置文件失败"));
        } else {
            //write to temp file
            map_area_t update_data;
            transform(area_data, update_data);
            QFileInfo file_info(map_area_cfg_.c_str());
            QString map_path = file_info.path();
            QString map_name = file_info.baseName();
            QString file = map_path + "/" + map_name + "_strong.json";
            if (!PRJ_CFG->saveData(file.toStdString(), update_data)) {
                std::cout << "write json to file error, file = " << file.toStdString();
            }
        }
    }

    //PRJ_CFG->saveData(file, info);
    lbl_operate_->setText(tr("保存文件成功"));
    //nav_task_->LoadFile(file_name);

    m_value = true;
}

void RobotViewerMainWind::slotFileNew()
{

}

void RobotViewerMainWind::slotFileOpen()
{
    lbl_operate_->setText(tr("打开文件"));
    slotEditSelectAll();
    slotEditDelete();
    QString file_name = openNewFile();
    if (file_name.isEmpty()) {
        return;
    }
    nav_task_->LoadFile(file_name);
    open_file_name_ = file_name.toStdString();
    printf("open file ok: %s \n", open_file_name_.c_str());
}

void RobotViewerMainWind::slotFileOpenMap()
{
#if 0
    if(lbl_operate_) {
        lbl_operate_->setText(tr("打开地图文件"));
    }
    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Open Map"), tr("~/config/map/"), tr("map(*.pgm)"));
    if (file_name.isEmpty()) {
        return;
    }

    QFileInfo file_info(file_name);
    QString map_name = file_info.baseName();
    QString map_path = file_info.path();
    QStringList name_info = map_name.split("_");
    if(name_info.size() >= 3) {
        map_id_ = name_info[1].toUInt();
        section_id_ = name_info[2].toUInt(); 
    } else {
        QMessageBox::warning(this, tr("打开地图文件错误"), 
            tr("打开非地图文件或者地图文件命名出错\n%1").arg(file_name));
            return;
    }
    QString yaml_cfg = map_path + "/" + map_name + ".yaml";
    qDebug() << "yaml config: " <<  map_name << map_path << yaml_cfg;

    if(!slotFileOpenYaml(yaml_cfg)) {
        QMessageBox::warning(this, tr("打开yaml文件错误"), 
            tr("未能成功打开文件%1").arg(yaml_cfg));
        return;
    }

    std::vector<AreaData> area_data;
    QString json_cfg = map_path + "/" + map_name + ".json";
    qDebug() << "json config: " << map_name << map_path << json_cfg;
    if(!PRJ_CFG->loadData(json_cfg.toStdString(), area_data)) {
        qDebug() << QString("未能成功打开文件%1").arg(json_cfg);
        return;
    }
    map_area_cfg_ = json_cfg.toStdString();

    QImage img(file_name);
    img_w_ = img.width();
    img_h_ = img.height();

    MapData map_data;
    map_data.is_file_ = true;
    map_data.file_ = file_name.toStdString();
    if (is_map_draw_) {
        if(scene_) {
            scene_->removeAreaItem();
            scene_->removeMapItem();
        }
    }
    if(scene_) {
        scene_->addMapItem(map_data);
        scene_->addAreaItems(area_data);
    }

    is_map_draw_ = true;
#else
    slotFileOpenRectMap();
#endif
}

void RobotViewerMainWind::slotFileOpenRectMap()
{
    if(lbl_operate_) {
        lbl_operate_->setText(tr("打开地图文件"));
    }
    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Open Map"), tr("~/config/map/"), tr("map(*.pgm)"));
    if (file_name.isEmpty()) {
        return;
    }

    QFileInfo file_info(file_name);
    QString map_name = file_info.baseName();
    QString map_path = file_info.path();
    QStringList name_info = map_name.split("_");
    if(name_info.size() >= 3) {
        map_id_ = name_info[1].toUInt();
        section_id_ = name_info[2].toUInt(); 
    } else {
        QMessageBox::warning(this, tr("打开地图文件错误"), 
            tr("打开非地图文件或者地图文件命名出错\n%1").arg(file_name));
            return;
    }
    QString yaml_cfg = map_path + "/" + map_name + ".yaml";
    qDebug() << "yaml config: " <<  map_name << map_path << yaml_cfg;

    if(!slotFileOpenYaml(yaml_cfg)) {
        QMessageBox::warning(this, tr("打开yaml文件错误"), 
            tr("未能成功打开文件%1").arg(yaml_cfg));
        return;
    }

    std::vector<AreaData> area_data;
    QString json_cfg = map_path + "/" + map_name + ".json";
    qDebug() << "json config: " << map_name << map_path << json_cfg;
    if(!PRJ_CFG->loadData(json_cfg.toStdString(), area_data)) {
        qDebug() << QString("未能成功打开文件%1").arg(json_cfg);
        return;
    }
    map_area_cfg_ = json_cfg.toStdString();

    QImage img(file_name);
    img_w_ = img.width();
    img_h_ = img.height();
    slotFileOpenImage(file_name);

    if (is_map_draw_) {
        if(scene_) {
            scene_->removeAreaItem();
        }
    }
    if(scene_) {
        scene_->addAreaItems(area_data);
    }

    is_map_draw_ = true;
}

bool RobotViewerMainWind::slotFileOpenImage(const QString &file_name)
{
    if(lbl_operate_) {
        lbl_operate_->setText(tr("打开图像文件"));
    }
    if (file_name.isEmpty()) {
        if(scene_) {
            scene_->removeMapItem();
        }
        return false;
    }

    QFileInfo file_info(file_name);
    QString map_name = file_info.baseName();
    QString map_path = file_info.path();

    QImage img(file_name);
    img_w_ = img.width();
    img_h_ = img.height();

    MapData map_data;
    map_data.is_file_ = true;
    map_data.file_ = file_name.toStdString();
    if (is_map_draw_) {
        if(scene_) {
            scene_->removeMapItem();
            scene_->removeAreaItem();
        }
    }
    if(scene_) {
        scene_->addMapItem(map_data);
    }
    is_map_draw_ = true;
    return true;
}

bool RobotViewerMainWind::slotFileOpenYaml(const QString &file_name)
{
    bool ret = false;
    try {
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly)) {
            printf("open file error, file = %s\n",
                file_name.toStdString().c_str());
            return false;
        }
        while (!file.atEnd()) {
            QString line = file.readLine();
            if (line.isEmpty()) {
                continue;
            }
            QStringList str_list = line.split(":", QString::SkipEmptyParts);
            if (str_list.size() < 2) {
                continue;
            }
            if (str_list.first() == "image") {
                image_url_ = str_list.at(1);
            } else if (str_list.first() == "resolution") {
                resolution_ = str_list.at(1).toFloat();
            } else if (str_list.first() == "occupied_thresh") {
                occupied_thresh_ = str_list.at(1).toFloat();
            } else if (str_list.first() == "free_thresh") {
                free_thresh_ = str_list.at(1).toFloat();
            } else if (str_list.first() == "negate") {
                negate_ = str_list.at(1).toInt();
            } else if (str_list.first() == "origin") {
                QString org = str_list.at(1);
                org = org.replace("[", "");
                org = org.replace("]", "");
                QStringList pts = org.split(",");
                if (pts.size() >= 3) {
                    origin_ = QPointF(pts.at(0).toFloat(), pts.at(1).toFloat());
                }
            }
        }
        file.close();
        ret = true;
    } catch(std::exception &e) {
        printf("open file exception(%s), file = %s\n", 
            e.what(), file_name.toStdString().c_str()); 
        ret = false; 
    }
    return ret;
}

void RobotViewerMainWind::transform(std::vector<AreaData> &area_data, map_area_t &update_data)
{
    update_data.map_id = map_id_;
    update_data.section_id = section_id_;
    update_data.width = img_w_;
    update_data.height = img_h_;
    update_data.origin.x = origin_.x();
    update_data.origin.y = origin_.y();
    update_data.resolution = resolution_;
    for (AreaData &data : area_data) {
        polygon_t py_data;
        point_t pt;
        RectData &rc = data.rect_data;
        pt.x = rc.corner_x;
        pt.y = img_h_ - rc.corner_y;
        py_data.push_back(pt);
        pt.y = img_h_ - (rc.corner_y + rc.height);
        py_data.push_back(pt);
        pt.x = rc.corner_x + rc.width;
        pt.y = img_h_ - (rc.corner_y + rc.height);
        py_data.push_back(pt);
        pt.x = rc.corner_x + rc.width;
        pt.y = img_h_ - rc.corner_y;
        py_data.push_back(pt);
    }
}

void RobotViewerMainWind::slotFileUpdateMap()
{
    std::vector<AreaData> area_data;
    if (!is_map_draw_) {
        QMessageBox::warning(this, 
                tr("未能预先加载地图"), tr("未能预先加载地图"));
        return;
    }

    if (scene_) {
        bool ret = scene_->getAreaDatas(area_data);
        if(false == ret || area_data.empty()) {
             QMessageBox::warning(this, 
                tr("获取地图区域数据错误"), tr("获取地图区域数据错误"));
            return;
        }
    }

    map_area_t update_data;
    transform(area_data, update_data);
    bool ret = emit sigUpdateMap(update_data);
    QString info = QString("update map data %1").arg(ret? "success": "error");
    if(ret) {
        QMessageBox::information(this, "update map data", info);
    } else {
        QMessageBox::critical(this, "update map data", info);
    }
}

void RobotViewerMainWind::slotFileEditMap()
{

}

void RobotViewerMainWind::slotFileSave()
{
    if (lbl_operate_) {
        lbl_operate_->setText(tr("保存文件"));
    }
    saveDataToFile(QString(open_file_name_.c_str()));
}

void RobotViewerMainWind::slotFileSaveAs()
{
    if (lbl_operate_) {
        lbl_operate_->setText(tr("文件另存为"));
    }

    QString file_name = QString::fromStdString(open_file_name_);
    if (file_name.isEmpty()) {
        file_name = QFileDialog::getSaveFileName(this,
            tr("Save"), "Data/", tr("Data(*.json)"));
    } else {
        file_name = QFileDialog::getSaveFileName(this,
            tr("Save"), "Data/" + file_name, tr("Data(*.json)"));
    }

    if (file_name.isEmpty()) {
        return;
    }

    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    } else {
        open_file_name_ = file_name.toStdString();
        saveDataToFile(file_name);
#ifdef ENABLE_TABLE
        center_view_tab_->setTabText(0, open_file_name_.c_str());
#endif
        return;
    }
}

void RobotViewerMainWind::slotFileMerge()
{
    lbl_operate_->setText(tr("合并文件"));
    QString file_name = openNewFile();
    std::cout << file_name.toStdString() << std::endl;
    // slotFileSaveAs();
}

QString RobotViewerMainWind::openNewFile()
{
    QString file_name = QFileDialog::getOpenFileName(
        this, tr("Data"), "./", tr("Data(*.json)"));
    if (file_name.isEmpty()) {
        return file_name;
    }

    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, 
            tr("打开文件错误"), tr("未能成功打开文件").arg(file_name));
        return QString("");
    }

    count_time count_time;
    count_time.begin();
    loadDataFromFile(file_name);
    count_time.end();
    std::cout << "load config time(ms): " << count_time.get_time() << std::endl;

    return file_name;
}

void RobotViewerMainWind::slotFileUpload()
{
    DialogUploadFile w(config_file_, this);
    w.exec();
}

void RobotViewerMainWind::slotFileDownload()
{

}

void RobotViewerMainWind::slotFileExit()
{
    close();
}

void RobotViewerMainWind::slotEditDelete()
{
    lbl_operate_->setText(tr("删除"));
    if (scene_) {
        scene_->deleteItems();
    }
}

void RobotViewerMainWind::slotEditFind()
{
    lbl_operate_->setText(tr("查找"));
    if (!scene_ || !view_ || !find_dialog_)
        return;

    find_dialog_->exec();
    int selectType = find_dialog_->getSelectType();
    int id = find_dialog_->getSearchID();

    bool is_search = find_dialog_->search();
    if (!is_search) {
        return;
    }

    switch (selectType)
    {
    case DialogFind::FindSite:
    {
        if (!scene_->searchSite(id))  {
            QMessageBox::warning(this, tr("查找错误!"), tr("该任务点不存在!"));
        }
        break;
    }
    case DialogFind::FindNode:
    {
        if (!scene_->searchNode(id)) {
            QMessageBox::warning(this, tr("查找错误!"), tr("该节点不存在!"));
        }
        break;
    }
    case DialogFind::FindEdge:
    {
        if (!scene_->searchEdge(id)) {
            QMessageBox::warning(this, tr("查找错误!"), tr("该边不存在!"));
        }
        break;
    }
    default:
        break;
    }
}

void RobotViewerMainWind::slotEditSelectAll()
{
    if(scene_) {
        scene_->selectAll();
    }
}

void RobotViewerMainWind::slotOperateSelect(bool checked)
{
    operationAction(checked, ACTION_SELECT);
}

void RobotViewerMainWind::slotOperateMove(bool checked)
{
    operationAction(checked, ACTION_MOVE);
}

void RobotViewerMainWind::slotOperateHand(bool checked)
{
    operationAction(checked, ACTION_HANDDRAG);
}

void RobotViewerMainWind::slotOperateZoomIn()
{
    if (!is_close_ && scene_) {
        scene_->slotZoom(false);
    }
}

void RobotViewerMainWind::slotOperateZoomOut()
{
    if (!is_close_ && scene_) {
        scene_->slotZoom(true);
    }
}

void RobotViewerMainWind::slotOperateEdge(bool checked)
{
    operationAction(checked, ADD_EDGE);
}

void RobotViewerMainWind::slotOperateNode(bool checked)
{
    operationAction(checked, ADD_NODE);
}

void RobotViewerMainWind::slotOperateSite(bool checked)
{
    operationAction(checked, ADD_SITE);
}

void RobotViewerMainWind::slotOperateLocSite()
{
    if (nav_task_ && nav_task_->IsRun()) {
        slotSuspendNavigation(true);
    }
    scene_->addCurrentSite();
}

void RobotViewerMainWind::slotOperateLocNode()
{
    if (nav_task_ && nav_task_->IsRun()) {
        slotSuspendNavigation(true);
    }

    if (!scene_->addCurrentNode()) {
        QMessageBox::warning(this, 
            tr("添加节点错误"), tr("此处已存在节点"), QMessageBox::Ok);
        if (nav_task_ && nav_task_->IsRun()) {
            slotSuspendNavigation(true);
        }
    }
}

void RobotViewerMainWind::slotOperateLocation(bool checked)
{
    operationAction(checked, SET_LOCROBOT);
}

void RobotViewerMainWind::slotOperateRemoteCtrl(bool checked)
{
    operationAction(checked, REMOTE_CONTROL);
}

void RobotViewerMainWind::slotShowMap(bool checked)
{
    emit sigSensorEnable(kSensorMap, checked);
    if(scene_) {
        scene_->slotShowMap(checked);
    }
    update();
}

void RobotViewerMainWind::slotShowLaser(bool checked)
{
    emit sigSensorEnable(kSensorLaser, checked);
    if(scene_) {
        scene_->slotShowLaser(checked);
    }
    update();
}

void RobotViewerMainWind::slotTrackRobot(bool checked)
{
    if(scene_) {
        scene_->slotTrackRobot(checked);
    }
}

void RobotViewerMainWind::slotOperateTargetPt(bool checked)
{
    if (lbl_operate_ && checked) {
        lbl_operate_->setText(tr("设置任务目标点"));
        swichOperateType(SET_NAVTARGET);
    } else if (scene_ && lbl_operate_ && scene_->getInsertType() == SET_NAVTARGET) {
        lbl_operate_->setText(tr("选择"));
        swichOperateType(ACTION_SELECT);
    } else {
        //todo
    }
}

void RobotViewerMainWind::slotSuspendNavigation(bool checked)
{
    if (suspend_type_)
    {
        if (nav_task_->PauseTarget())
        {
            nav_suspend_action_->setIcon(QIcon(QString::fromUtf8(":/images/undo.png")));
            suspend_type_ = false;
        }
    }
    else
    {
        if (nav_task_->ResumeTarget())
        {
            nav_suspend_action_->setIcon(QIcon(QString::fromUtf8(":/images/suspend.png")));
            suspend_type_ = true;
        }
    }
}

void RobotViewerMainWind::slotStopNavigation()
{
    if (nav_task_->CancelTarget()) {
        slotNavigationTaskFinish(true);
    }
}

void RobotViewerMainWind::slotStartNavigation()
{
    std::vector<UPL> upls = scene_->getTargetList();
    if (upls.empty()) {
        QMessageBox::warning(this, 
            tr("导航任务出错"), tr("未选中任务点"), QMessageBox::Ok);
        return;
    }
    if (nav_task_->StartTarget(upls)) {
        nav_start_action_->setDisabled(true);
        nav_suspend_action_->setEnabled(true);
        nav_stop_action_->setEnabled(true);
        scene_->enableAimRefUpdate(true);
    } else {
        QMessageBox::warning(this, 
            tr("导航任务出错"), tr("任务正忙"), QMessageBox::Ok);
    }
}

void RobotViewerMainWind::slotResetNavigation()
{
#if 0
    GUPLSet upl_set;
    upl_set.set_type(GUPLSet_GUPLSetType_REFRESH);
    if(!AGVROS.CallSetNavigationUpl(upl_set))
    {
        QMessageBox::warning(this,tr("导航任务出错"),tr("重置导航失败"),QMessageBox::Ok);
    }
#endif
}

void RobotViewerMainWind::slotNavSpeedUp()
{

}

void RobotViewerMainWind::slotNavSpeedDown()
{

}

void RobotViewerMainWind::slotNavigationError(QVector<UPL> upls, int type)
{
    if (upls.empty() && type != 4)
    {
        return;
    }
    QString error_string;
    switch (type)
    {
    case RobotViewerNavigation::NaviagtionStartError:
    {
        error_string = "起点" + UPL2QString(upls[0]) + "搜索不到";
        break;
    }
    case RobotViewerNavigation::NaviagtionEndError:
    {
        error_string = "终点" + UPL2QString(upls[0]) + "搜索不到";
        break;
    }
    case RobotViewerNavigation::NaviagtionPathError:
    {
        error_string = "起点" + UPL2QString(upls[0]) + "到" + UPL2QString(upls[1]) + "路径搜索失败";
        break;
    }
    case RobotViewerNavigation::NaviagtionClose:
    {
        error_string = "导航程序未开";
        break;
    }
    }
    QMessageBox::warning(this, tr("导航任务出错"), error_string, QMessageBox::Ok);
    slotNavigationTaskFinish(true);
}

QString RobotViewerMainWind::UPL2QString(const UPL &upl)
{
    return QString("UPL(") + QString::number(upl.start_id) + "," + QString::number(upl.end_id) + ")";
}

void RobotViewerMainWind::slotNavigationTaskFinish(bool)
{
    nav_start_action_->setDisabled(false);
    nav_suspend_action_->setEnabled(false);
    nav_suspend_action_->setIcon(QIcon(QString::fromUtf8(":/images/suspend.png")));
    suspend_type_ = true;
    nav_stop_action_->setEnabled(false);
    scene_->clearTarget();
    scene_->clearRoute();
    scene_->enableAimRefUpdate(false);
}

void RobotViewerMainWind::slotNavInitPos(bool)
{
    if (!is_close_ && scene_) {
        scene_->slotNavInitPos();
    }
}

void RobotViewerMainWind::slotNavToGoal()
{
    if (!is_close_ && scene_) {
        scene_->slotNavToGoal();
    }
}

void RobotViewerMainWind::slotNavToMutilGoal()
{
    if (!is_close_ && scene_) {
        scene_->slotNavToMutilGoal();
    }
}

void RobotViewerMainWind::slotNavToRestGoal()
{
    if (!is_close_ && scene_) {
        scene_->slotNavToRest();
    }
}

void RobotViewerMainWind::slotMouseRightPress()
{
    operationAction(true, ACTION_SELECT);
    if(scene_) {
        scene_->clearTarget();
        // scene_->PopMenue();
    }
}

void RobotViewerMainWind::slotDockCamera(bool checked)
{
    if (checked) {
        lbl_operate_->setText(tr("相机"));
        camera_dock_->show();
    } else {
        lbl_operate_->setText(tr("显示"));
        camera_dock_->hide();
    }
}

void RobotViewerMainWind::slotDockSite(bool checked)
{
    if (checked) {
        if(lbl_operate_) lbl_operate_->setText(tr("任务点"));
        if(site_dock_) site_dock_->show();
    } else {
        if(lbl_operate_) lbl_operate_->setText(tr("显示"));
        if(site_dock_) site_dock_->hide();
    }
}

void RobotViewerMainWind::slotDockEdge(bool checked)
{
    if (checked) {
        if(lbl_operate_) lbl_operate_->setText(tr("任务路线"));
        if(edge_dock_) edge_dock_->show();
    } else {
        if(lbl_operate_) lbl_operate_->setText(tr("显示"));
        if(edge_dock_) edge_dock_->hide();
    }
}

void RobotViewerMainWind::slotDockSimulate(bool checked)
{
    if (checked) {
        lbl_operate_->setText(tr("仿真"));
        simulator_dock_->show();
    } else {
        lbl_operate_->setText(tr("显示"));
        simulator_dock_->hide();
    }
}

void RobotViewerMainWind::slotDockPoly(bool checked)
{
    if (checked) {
        if(lbl_operate_) lbl_operate_->setText(tr("多边形"));
        if(poly_dock_) poly_dock_->show();
        if(poly_dock_)  poly_dock_->show();
    } else {
        if(lbl_operate_) lbl_operate_->setText(tr("显示"));
        if(poly_dock_) poly_dock_->hide();
        if(poly_dock_)  poly_dock_->hide();
    }
}

void RobotViewerMainWind::timerEvent(QTimerEvent *event)
{
    (void*)event;
    if(scene_) {
        scene_->updateAll();
    }
    slotRobotPos(robot_pos_);
    //slotBatteryVol();
}

void RobotViewerMainWind::slotMovePos(QPointF mousepos)
{
    QString str = QString(" 全局鼠标：%1m, %2m ").
        arg(mousepos.x(), 0, 'f', 2).arg(mousepos.y(), 0, 'f', 2);
    if(lbl_movepos_) {
        lbl_movepos_->setText(str);
        lbl_movepos_->setScaledContents(true);
    }
}

void RobotViewerMainWind::slotMousePos(QPointF mousepos)
{
    QString str = QString(" 实时鼠标：%1m, %2m ").
        arg(mousepos.x(), 0, 'f', 2).arg(mousepos.y(), 0, 'f', 2);
    
    if (lbl_mousepos_) {
        lbl_mousepos_->setText(str);
        lbl_mousepos_->setScaledContents(true);
    }
}

void RobotViewerMainWind::slotScale(qreal val)
{
    QString str = QString(" 缩放：%1% ").arg(val*100, 0, 'f', 0);
    if (lbl_scale_) {
        lbl_scale_->setText(str);
        lbl_scale_->setScaledContents(true);
    }
}

void RobotViewerMainWind::slotRobotPos(const OrientPos &pos)
{
    if (!lbl_robotpos_)
        return;

    QString str = QString(" 机器位置：%1m, %2m, %3度").
        arg(pos.x, 0, 'f', 2).arg(pos.y, 0, 'f', 2).arg(pos.angle, 0, 'f', 2);
    lbl_robotpos_->setText(str);
    lbl_robotpos_->setScaledContents(true);
}

void RobotViewerMainWind::slotBatteryVol()
{
    if (lbl_battary_ == NULL || lbl_voltage_ == NULL)
        return;

    QString textInfo = QString("%1 V ").arg(battary_volt_, 0, 'g', 3);

    if (battary_volt_ > 41)
        lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/fullvolt.png")));
    else if (battary_volt_ > 39 && battary_volt_ <= 41)
        lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/highvolt.png")));
    else if (battary_volt_ > 36 && battary_volt_ <= 39)
        lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/normalvolt.png")));
    else if (battary_volt_ > 34 && battary_volt_ <= 36)
        lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/lowvolt.png")));
    else if (battary_volt_ > 32 && battary_volt_ <= 34)
        lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/chargevolt.png")));
    else if (battary_volt_ <= 32)
    {
        lbl_battary_->setPixmap(QPixmap(QString::fromUtf8(":/images/novolt.png")));
        textInfo += tr("电量较低，请充电 ");
    }
    lbl_voltage_->setText(textInfo);
    lbl_voltage_->setScaledContents(true);
}

void RobotViewerMainWind::slotMapPos(const MapData &map_data)
{
    (void)map_data;
    if (NULL == lbl_map_)
        return;
    QString str;
    lbl_map_->setText(str);
    lbl_map_->setScaledContents(true);
}

void RobotViewerMainWind::slotCameraImage(int frame, const QImage &img)
{
    if(!is_close_ && camera_wgt_) {
        camera_wgt_->showImage(frame, img);
    }
}

void RobotViewerMainWind::slotCameraTopic(const QVector<QString> &data)
{
    if(!is_close_ && camera_wgt_) {
        camera_wgt_->showTopicList(data);
    }
}

void RobotViewerMainWind::setRobotStatus(uint16_t status)
{
    if (scene_) {
        scene_->setRobotStatus(status);
    }
}

void RobotViewerMainWind::slotUpdateMaps(const QImage &data)
{
    if (!is_close_ && scene_) {
        scene_->slotUpdateMaps(data);
    }
}
void RobotViewerMainWind::slotUpdatePlannerPath(const QPolygonF &data)
{
    if (!is_close_ && scene_) {
        scene_->slotUpdatePlannerPath(data);
    }
}
void RobotViewerMainWind::slotUpdateOccupancyGrid(const occupancy_grid_t &data)
{
    if (!is_close_ && scene_) {
        scene_->slotUpdateOccupancyGrid(data);
    }
}

void RobotViewerMainWind::recvRobotPos(const OrientPos &robot_pos)
{
    if (!is_close_ && scene_) {
        robot_pos_ = robot_pos;
        scene_->setAgvPos(robot_pos_);
    }
}

void RobotViewerMainWind::slotLaserScan(const laser_scan_t &scan)
{

}

void RobotViewerMainWind::recvLaserScan3D(const Glaser3DBeam &scan)
{
    if (!is_close_ && scene_) {
        scene_->setLaserScan3D(scan);
    }
}

void RobotViewerMainWind::recvLaserScan(const QPolygonF &scan)
{
    if (!is_close_ && scene_) {
        scene_->setLaserScan(scan);
    }
}

void RobotViewerMainWind::recvReflector(const GReflector &reflector)
{
    if (!is_close_ && scene_) {
        scene_->setReflector(reflector);
    }
}

void RobotViewerMainWind::recvRoute(const QPolygonF &data)
{
    if (is_close_ || !scene_) {
        return;
    }
    std::vector<NodeData> node_datas;
    std::vector<EdgeData> edge_datas;
    std::vector<SiteData> site_datas;
    std::vector<AreaData> area_datas;
    for(int i = 0; i < data.size(); i++) {
        QPointF pt = data[i];
        NodeData node_data;
        node_data.id = i+1;
        node_data.pos = Pos(pt.x(), pt.y());
        node_datas.push_back(node_data);

        EdgeData edge_data;
        edge_data.id = i+1;
        edge_data.startid = i+1;
        edge_data.endid = i+2;
        edge_datas.push_back(edge_data);
    }
    if(scene_) {
        scene_->addNodeItems(node_datas);
        scene_->addEdgeItems(edge_datas);
        scene_->addSiteItems(site_datas);
        scene_->addAreaItems(area_datas);
    }
}

void RobotViewerMainWind::recvReferTarget(const VecPosition &refer_target)
{
    if (!is_close_ && scene_) {
        scene_->setReferencePos(refer_target);
    }
}

void RobotViewerMainWind::recvAimTarget(const VecPosition &call_back_aim_target)
{
    if (!is_close_ && scene_) {
        scene_->setAimPos(call_back_aim_target);
    }
}

int RobotViewerMainWind::round(double x)
{
    if (x > 0.0)
        x += 0.5;
    else
        x -= 0.5;
    return (int)x;
}

void RobotViewerMainWind::openApp(const QString &file_name)
{
    QString error_str;
    QString file = QCoreApplication::applicationDirPath() + file_name;
    if (!QFile::exists(file)) {
        error_str = tr("文件不存在: ") + file;
    } else {
        // HINSTANCE ret = ShellExecute(NULL, 
        //    "Open", file.toStdString().c_str(), NULL, NULL, SW_SHOW);
        int ret = 0;
        if (ret <= 32) {
            error_str = QString("打开应用程序错误：%1").arg(ret);
        }
    }
    if (!error_str.isEmpty()) {
        QMessageBox::warning(this, tr("打开出错"), error_str, QMessageBox::Ok);
    }
}
