#include "map/robot_widget.h"
#include "ui_robot_widget.h"
//#include <QSoundEffect>
#include <QMouseEvent>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include "map/robot_map.h"
#include "map/robot_scene.h"
#include "common/msg.h"

#define SAFE_DELETE(p) \
  if (p) {             \
    delete p;          \
  }                    \
  p = nullptr;

//#define ENABLE_TF

RobotWidget::RobotWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::RobotWidget)
{
  init();
}

RobotWidget::~RobotWidget()
{
  delete ui;
}

void RobotWidget::init()
{
  ui->setupUi(this);
  ui->label_map_type->setVisible(false);
  ui->cbx_map_type->setVisible(false);
  ui->widget_rviz->setVisible(false);
  
  // map scene
  map_scene_ = new RobotScene(this);
  map_scene_->clear();
  robot_map_ = new RobotMap();
  map_scene_->addItem(robot_map_);
  ui->mapViz->setScene(map_scene_);
  robot_map_->installEventFilter(map_scene_);

  // sound effect
  //media_player_ = new QSoundEffect;

  //node to map_scene
  connect(map_scene_, &RobotScene::sig_pub_map_image,
          this, [=](const QImage &img) {
            emit sig_pub_map_image(img); 
          });
  connect(map_scene_, &RobotScene::sig_cursor_pos, 
          this, &RobotWidget::slot_update_cursor_pos);
  connect(robot_map_, &RobotMap::sig_pub_init_pos,
          this, [=](const algo::RobotPose &data) { 
            emit sig_init_pos(data);
          });
  connect(robot_map_, &RobotMap::sig_pub_2d_goal,
          this, [=](const algo::RobotPose &data) { 
            emit sig_set_goal(data); 
          });
}

void RobotWidget::deinit()
{
  SAFE_DELETE(robot_map_);
  SAFE_DELETE(map_scene_);
  //SAFE_DELETE(media_player_);
  SAFE_DELETE(ui);
}

void RobotWidget::set_robot_status(int status)
{
  if(robot_map_) {
    robot_map_->set_robot_status(status);
  }
}

void RobotWidget::on_cbx_map_type_currentIndexChanged(int index)
{
  switch (index)
  {
  case 0:
  {
    ui->mapViz->show();
    break;
  }
  case 1:
  {
    ui->mapViz->hide();
    break;
  }
  default:
    break;
  }
}

void RobotWidget::on_btn_set_init_pos_clicked()
{
  if (robot_map_) {
    robot_map_->slot_set_init_pos();
  }
  // ui->label_map_msg->setText("请在地图中选择机器人的初始位置");
}

void RobotWidget::on_btn_set_single_goal_clicked()
{
  if (robot_map_) {
    robot_map_->slot_set_nav_goal();
  }
  //  ui->label_map_msg->setText("请在地图中选择机器人导航的目标位置");
}

void RobotWidget::on_btn_set_mutil_goal_clicked()
{

}

void RobotWidget::on_btn_set_break_goal_clicked()
{
  // qnode_->set_goal(ui->label_frame->text().toStdString(),ui->label_return_x->text().toDouble(),
  //       ui->label_return_y->text().toDouble(),ui->label_return_z->text().toDouble(),
  //       ui->label_return_w->text().toDouble());
#if 0
  if (nullptr == media_player_) {
    media_player_ = new QSoundEffect;
  }
  media_player_->setSource(QUrl::fromLocalFile("://media/start_return.wav"));
  media_player_->play();
#endif
}

void RobotWidget::slot_move_camera_btn()
{
  if (robot_map_) {
    robot_map_->slot_set_move_camera();
  }
}

void RobotWidget::slot_set_select()
{

}

void RobotWidget::slot_update_cursor_pos(const QPointF &pos)
{
  QPointF map_pos = transform_scene_to_map(pos);
  QString map_info = QString("(%1, %2)").
      arg(map_pos.x(), 0, 'f', 4).arg(map_pos.y(), 0, 'f', 4);
  QString scene_info = QString("(%1, %2)").
      arg(pos.x(), 0, 'f', 4).arg(pos.y(), 0, 'f', 4);
  ui->label_pos_map->setText(map_info);
  ui->label_pos_scene->setText(scene_info);
}

void RobotWidget::slot_update_robot_pos(const algo::RobotPose &pt)
{
  if (robot_map_) {
#ifdef ENABLE_TF
    QPointF pos = transform_map_to_scene(QPointF(pt.x, pt.y));
#else
    QPointF pos(pt.x, pt.y);
#endif
    robot_map_->paint_robot_pos(algo::RobotPose(pos.x(), pos.y(), pt.theta));
  }
}

void RobotWidget::slot_update_laser_scan(const QPolygonF &data)
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
  }
}

void RobotWidget::slot_update_maps(const QImage &data)
{
  if (robot_map_) {
    robot_map_->paint_maps(data);
  }
}

void RobotWidget::slot_update_planner_path(const QPolygonF &data)
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

void RobotWidget::slot_update_occupancy_grid(const occupancy_grid_t &data)
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
}

QPointF RobotWidget::transform_scene_to_map(const QPointF &pos)
{
  QPointF robot_pos;
  robot_pos.setX((pos.x() - scene_center_point_.x()) * map_resolution_ +
               map_center_point_.x());
  robot_pos.setY(-1 * (pos.y() - scene_center_point_.y()) * map_resolution_ +
               map_center_point_.y());
  return robot_pos;
}

QPointF RobotWidget::transform_map_to_scene(const QPointF &pos)
{
  QPointF robot_pos;
  robot_pos.setX((pos.x() - map_center_point_.x()) / map_resolution_ +
               scene_center_point_.x());
  robot_pos.setY(-1 * (pos.y() - map_center_point_.y()) / map_resolution_ +
               scene_center_point_.y());
  return robot_pos;
}
