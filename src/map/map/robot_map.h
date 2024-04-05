#ifndef __COMWISE__ROBOT_VIEWER__ROBOT_MAP__H__
#define __COMWISE__ROBOT_VIEWER__ROBOT_MAP__H__

#include <memory>
#include <QGraphicsObject>
#include <QPolygon>
#include "common/msg.h"

QT_BEGIN_NAMESPACE
class QCursor;
class QGraphicsSceneWheelEvent;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;
class QStyleOptionGraphicsItem;
class QPainter;
class QTouchEvent;
QT_END_NAMESPACE

class RobotMap : public QGraphicsObject {
  Q_OBJECT
  using image_ptr_t = std::shared_ptr<QImage>;
public:
  enum { Type = 2000 };
  int type() const { return Type; }
  
public:
  RobotMap();
  ~RobotMap();

  void set_robot_status(int status);
  void set_robot_size(const QSize &size);

  void set_zoom_out();
  void set_zoom_in();
  void set_default_scale();
  
  void move(double x, double y);

signals:
  void sig_cursor_pos(const QPointF &);
  void sig_pub_init_pos(const algo::RobotPose &pose);
  void sig_pub_2d_goal(const algo::RobotPose &pose);

public slots:
  void paint_image(int, QImage*);
  void paint_maps(const QImage &);
  void paint_robot_pos(const algo::RobotPose &pos);
  void paint_planner_path(const QPolygonF &);
  void paint_laser_scan(const QPolygonF &);

  void slot_set_init_pos();
  void slot_set_nav_goal();
  void slot_set_move_camera();

private:
  QRectF boundingRect() const;

  virtual bool event(QEvent *event) override;

  virtual void wheelEvent(QGraphicsSceneWheelEvent *event) override;

  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
  void init();
  void deinit();

  void drawMap(QPainter *painter);
  void drawRoboPos(QPainter *painter);
  void drawLaserScan(QPainter *painter);
  void drawPlannerPath(QPainter *painter);
  void drawTools(QPainter *painter);

  void scale_view(qreal scale, const QPointF &pt);

  QColor get_current_color();
  int qcolor_to_int(const QColor &color);

private:
  qreal scale_value_ {1};

  //press point
  bool is_pressed_{false};
  QPointF start_pos_;
  QPointF pressed_point_;
  QPointF pressing_point_;
  
  //path
  QPolygonF planner_path_;

  //laser
  QPolygonF laser_points_;
  
  //map
  QImage map_img_;
  
  //robot image 
  std::map<int, image_ptr_t> kConstRobotImgs_;
  image_ptr_t  robot_img_;

  //robot state and pos
  int robot_state_{0};
  QPointF robot_pos_;
  double robot_yaw_{0};

  //image object
  std::map<int, QImage*> painter_img_;

  //cursor
  bool is_other_cursor_{false};
  QCursor *kCursorMove {nullptr};
  QCursor *kCursorSetInitPose {nullptr};
  QCursor *kCursorSet2DGoal {nullptr};
  QCursor *curr_cursor_ {nullptr};

};

#endif // __COMWISE__ROBOT_VIEWER__ROBOT_MAP__H__
