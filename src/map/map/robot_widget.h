#ifndef __COMWISE__ROBOT_VIEWER__ROBOT_WIDGET__H__
#define __COMWISE__ROBOT_VIEWER__ROBOT_WIDGET__H__

#include <memory>
#include <QWidget>
#include <QPointF>
#include "common/msg.h"

namespace Ui {
  class RobotWidget;
}

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QTimer;
class QSoundEffect;
class QMouseEvent;
QT_END_NAMESPACE

class occupancy_grid_t;

class RobotMap;
class RobotScene;
class RobotWidget : public QWidget
{
  Q_OBJECT
public:
  explicit RobotWidget(QWidget *parent = 0);
  ~RobotWidget();

  void set_robot_status(int status);

signals:
  void sig_move_camera();
  void sig_pub_map_image(const QImage &);

  void sig_init_pos(const algo::RobotPose &);
  void sig_set_goal(const algo::RobotPose &);

public slots:
  void slot_update_cursor_pos(const QPointF &);
  void slot_update_robot_pos(const algo::RobotPose &);
  void slot_update_laser_scan(const QPolygonF &);
  void slot_update_maps(const QImage &);
  void slot_update_planner_path(const QPolygonF &);
  void slot_update_occupancy_grid(const occupancy_grid_t &);

private slots:
  void on_btn_set_init_pos_clicked();
  void on_btn_set_single_goal_clicked();
  void on_btn_set_mutil_goal_clicked();
  void on_btn_set_break_goal_clicked();
  void on_cbx_map_type_currentIndexChanged(int index);

  void slot_set_select();
  void slot_move_camera_btn();

private:
  void init();
  void deinit();

  QPointF transform_scene_to_map(const QPointF &pos);
  QPointF transform_map_to_scene(const QPointF &pos);

private:
  Ui::RobotWidget *ui{nullptr};

  //map
  RobotScene *map_scene_{nullptr};
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

  // media
  QSoundEffect *media_player_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__ROBOT_WIDGET__H__
