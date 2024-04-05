#ifndef __COMWISE__ROBOT_VIEWER__MAINWIND__H__
#define __COMWISE__ROBOT_VIEWER__MAINWIND__H__

#include <memory>
#include <thread>
#include <string>
#include <QMainWindow>
#include "RobotViewerData.h"
#include "common/algorithm.h"

// menu
class QLabel;
class QAction;

// widget
class CDockWidget;
class RobotViewerStick;
class RobotViewerScene;
class RobotViewerView;
class RobotViewerConfig;
class RobotViewerMapConfig;
class RobotViewerNavigation;
class DialogFind;
class DockEdge;
class DockSite;
class DockSimulator;
class CameraWidget;

class StatusWidget;
class StatusDevice;
class StorageManage;
class DialogPolygon;
class PolyProp;

//message
class GLaserScan;
class Glaser3DBeam;
class LocOptionalData;
class GReflector;
class GNavigationUPL;
class CameraData;
class battery_state_t;
class camera_sub_t;
class laser_scan_t;
class occupancy_grid_t;
class map_area_t;

Q_DECLARE_METATYPE(QVector<UPL>)

class RobotViewerMainWind : public QMainWindow
{
    using RobotViewerStickPtr = std::shared_ptr<RobotViewerStick>;

    enum DockType {
        kDockStorage,
        kDockPoint,
        kDockEdge,
        kDockCamera,
        kDockStatus,
    };
    Q_OBJECT

public:
    RobotViewerMainWind(const std::string &file);
    ~RobotViewerMainWind();

    void setRobotStatus(uint16_t status);

signals:
    void closeWidget();
    void sigSensorEnable(int, bool);
    bool sigSubscribeImage(const camera_sub_t &);
    void sigMoveCamera();
    void sigMapImage(const QImage &);
    bool sigUpdateMap(const map_area_t &);
    void sigInitPos(const algo::RobotPose &);
    void sigSetGoal(const algo::RobotPose &);

public slots:
    //相机
    void slotCameraImage(int, const QImage &);
    void slotCameraTopic(const QVector<QString> &);

    //用于接收ros消息
    void slotLaserScan(const laser_scan_t &);
    void recvLaserScan(const QPolygonF &scan);
    void recvLaserScan3D(const Glaser3DBeam &scan);
    void recvReflector(const GReflector &reflector);
    void recvRoute(const QPolygonF &data);
    void recvReferTarget(const VecPosition &refer_target);
    void recvAimTarget(const VecPosition &call_back_aim_target);

    void recvRobotPos(const OrientPos &robotpos);
    void slotUpdateMaps(const QImage &);
    void slotUpdatePlannerPath(const QPolygonF &);
    void slotUpdateOccupancyGrid(const occupancy_grid_t &);

private slots:
    //菜单: 文件操作
    void slotFileOpenMap();
    void slotFileOpenRectMap();
    bool slotFileOpenImage(const QString &file);
    bool slotFileOpenYaml(const QString &file);
    void slotFileEditMap();
    void slotFileUpdateMap();
    void slotFileMerge();
    void slotFileNew();
    void slotFileOpen();
    void slotFileSave();
    void slotFileSaveAs();
    void slotFileExit();
    void slotFileUpload();
    void slotFileDownload();

    //菜单: 地图构建操作
    void slotOperateSelect(bool checked);
    void slotOperateMove(bool checked);
    void slotOperateHand(bool checked);
    void slotOperateZoomIn();
    void slotOperateZoomOut();
    void slotOperateEdge(bool checked);
    void slotOperateNode(bool checked);
    void slotOperateSite(bool checked);
    void slotOperateLocSite();
    void slotOperateLocNode();
    void slotOperateLocation(bool checked);
    void slotOperateRemoteCtrl(bool checked);

    //菜单: 地图编辑
    void slotEditDelete();
    void slotEditFind();
    void slotEditSelectAll();

    //菜单: 显示
    void slotShowMap(bool);
    void slotShowLaser(bool);
    void slotTrackRobot(bool);

    //菜单: 服务
    void slotDockCamera(bool checked);
    void slotDockEdge(bool checked);
    void slotDockSite(bool checked);
    void slotDockSimulate(bool checked);
    void slotDockPoly(bool checked);

    //菜单: 导航
    void slotOperateTargetPt(bool);
    void slotStartNavigation();
    void slotSuspendNavigation(bool);
    void slotStopNavigation();
    void slotResetNavigation();
    void slotNavSpeedUp();
    void slotNavSpeedDown();
    void slotNavigationError(QVector<UPL>, int);
    void slotNavigationTaskFinish(bool);

    void slotNavInitPos(bool);
    void slotNavToGoal();
    void slotNavToMutilGoal();
    void slotNavToRestGoal();

    //状态栏
    void slotRobotPos(const OrientPos &pos);
    void slotMousePos(QPointF mousepos);
    void slotMovePos(QPointF mousepos);
    void slotMouseRightPress();
    void slotBatteryVol();
    void slotMapPos(const MapData &data);
    void slotScale(qreal val);

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void init();
    void deinit();

    //创建窗体
    void initForm();          //初始化窗体
    void initMembers();       //初始化成员
    void initSlots();         //初始化信号

    void createTitle();       //设置标题
    void createActions();     //创建动作
    void createMenus();       //创建菜单窗口
    void createToolBars();    //创建工具栏
    void createCentralWnd();  //创建主窗体
    void createDockWindows(); //创建停靠窗口
    void createStatusBar();   //创建状态栏

    //文件操作函数
    QString openNewFile();
    void loadDataFromFile(const QString &fileName);
    void saveDataToFile(const QString &fileName);

    //手柄
    void enableStick(bool enable);

    //切换操作
    void operationAction(bool checked, ActionMode mode);
    void swichOperateType(ActionMode type);

    //UPL转换
    QString UPL2QString(const UPL &upl);

    void transform(std::vector<AreaData> &area_data, map_area_t &update_data);

    //实用函数
    int round(double x);
    void openApp(const QString &file);

private:
    //file action
    QAction *file_update_map_action_{nullptr};
    QAction *file_edit_map_action_{nullptr};
    QAction *file_new_action_{nullptr};
    QAction *file_merger_action_{nullptr};
    QAction *file_open_action_{nullptr};
    QAction *file_save_action_{nullptr};
    QAction *file_save_as_action_{nullptr};
    QAction *file_open_map_action_{nullptr};
    QAction *file_open_mode_action_{nullptr};
    QAction *file_upload_config_action_{nullptr};
    QAction *file_download_config_action_{nullptr};
    QAction *file_exit_action_{nullptr};

    //edit action
    QAction *edit_undo_action_{nullptr};
    QAction *edit_redo_action_{nullptr};
    QAction *edit_cut_action_{nullptr};
    QAction *edit_copy_action_{nullptr};
    QAction *edit_paste_action_{nullptr};
    QAction *edit_delete_action_{nullptr};
    QAction *edit_find_action_{nullptr};
    QAction *edit_select_all_action_{nullptr};

    //view action
    QAction *view_select_action_{nullptr};
    QAction *view_move_action_{nullptr};
    QAction *view_drag_action_{nullptr};
    QAction *view_zoomin_action_{nullptr};
    QAction *view_zoomout_action_{nullptr};
    QAction *show_map_action_{nullptr};
	QAction *show_laser_action_{nullptr};
	QAction *track_robot_action_{nullptr};

    //route action
    QAction *operation_node_action_{nullptr};
    QAction *operation_edge_action_{nullptr};
    QAction *operation_site_action_{nullptr};
    QAction *operation_area_edit_action_{nullptr};
    QAction *operation_area_load_action_{nullptr};
    QAction *operation_area_ok_action_{nullptr};
    QAction *operation_area_save_action_{nullptr};
    QAction *operation_poly_load_action_{nullptr};
    QAction *operation_poly_edit_action_{nullptr};
    QAction *operation_poly_ok_action_{nullptr};
    QAction *operation_poly_undo_action_{nullptr};
    QAction *operation_loc_site_action_{nullptr};
    QAction *operation_loc_node_action_{nullptr};

    //导航Action
    QAction *nav_init_location_{nullptr};
    QAction *nav_target_action_{nullptr};
    QAction *nav_start_action_{nullptr};
    QAction *nav_suspend_action_{nullptr};
    QAction *nav_stop_action_{nullptr};
    QAction *nav_reset_action_{nullptr};
    QAction *nav_speed_up_action_{nullptr};
    QAction *nav_speed_down_action_{nullptr};

    QAction *nav_init_pos_action_{nullptr};
    QAction *nav_set_goal_action_{nullptr};
    QAction *nav_set_mutil_goal_action_{nullptr};
    QAction *nav_set_rest_action_{nullptr};

    // manual remote
    QAction *action_oper_remote_{nullptr};

    //view tab
    QTabWidget *center_view_tab_{nullptr};

    //camera dock
    QAction *camera_action_{nullptr};
    CameraWidget *camera_wgt_{nullptr};
    CDockWidget *camera_dock_{nullptr};

    //edge dock
    QAction *edge_scan_action_{nullptr};
    DockEdge *edge_tablewidget_{nullptr};
    CDockWidget *edge_dock_{nullptr};

    //site dock
    QAction *site_scan_action_{nullptr};
    DockSite *site_tablewidget_{nullptr};
    CDockWidget *site_dock_{nullptr};

    // simulator dock
    QAction *simulator_action_{nullptr};
    DockSimulator *simulator_widge_{nullptr};
    CDockWidget *simulator_dock_{nullptr};

    // poly dock
    QAction *poly_action_{nullptr};
    StorageManage *poly_widget_{nullptr};
    CDockWidget *poly_dock_{nullptr};
    PolyProp *poly_prop_{nullptr};

    //查找栏
    DialogFind *find_dialog_{nullptr};

    //帮助
    QAction *help_about_{nullptr};

    //状态栏
    QLabel *lbl_operate_{nullptr};  //当前操作
    QLabel *lbl_mousepos_{nullptr}; //鼠标位置
    QLabel *lbl_movepos_{nullptr};  //鼠标位置(地图编辑)
    QLabel *lbl_robotpos_{nullptr}; //车体位置
    QLabel *lbl_battary_{nullptr};  //电池状态
    QLabel *lbl_voltage_{nullptr};  //电池电压
    QLabel *lbl_map_{nullptr};      //地图信息
    QLabel *lbl_scale_{nullptr};    //缩放信息

    //主要窗体
    RobotViewerMapConfig *map_2d_{nullptr};
    RobotViewerConfig *file_{nullptr};
    RobotViewerScene *scene_{nullptr};
    RobotViewerView *view_{nullptr};
    RobotViewerNavigation *nav_task_{nullptr};

private:
    QString config_file_; //config file
    std::string open_file_name_;
    std::string map_area_cfg_;

    bool is_close_{false};
    bool is_map_draw_{false};

    OrientPos robot_pos_; // location
    bool suspend_type_{false}; // navigation
    double battary_volt_{0.0}; // battery

    // stick
    RobotViewerStickPtr control_stick_{nullptr};
    bool is_stick_press_{false};

    std::size_t operate_type_{0};

    //map info
    uint32_t map_id_{0};
    uint32_t section_id_{0};
    QString image_url_;
    float resolution_{1.0};
    QPointF origin_;
    float occupied_thresh_{0.0};
    float free_thresh_{0.0};
    int negate_{0};
    uint32_t img_w_{0};
    uint32_t img_h_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__MAINWIND__H__
