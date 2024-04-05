#include "map/robot_map.h"

#include <QDebug>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QColor>
#include <QCursor>
#include <QtMath>
#include <QTouchEvent>

namespace {
  static const int kInitWidth = 100;
  static const int kInitHeight = 100;
  static const int kInitSize = 6;
  static const double kScaleMin = 0.3;
  static const double kScaleDefault = 1;
  static const double PI = 3.1415926;
  static const char*  kConstRobotImage[] = 
    {"blue", "blue", "yellow", "red", "red"};
}  

RobotMap::RobotMap()
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setAcceptTouchEvents(true);
  setAcceptDrops(true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(ItemAcceptsInputMethod, true);
  setZValue(-35);
  init();
}

RobotMap::~RobotMap()
{
  deinit();
}

void RobotMap::init()
{
  moveBy(0, 0);

  kCursorMove = new QCursor(QPixmap("://images/cursor_move"), 0, 0);
  kCursorSetInitPose = new QCursor(QPixmap("://images/cursor_pos.png"), 0, 0);
  kCursorSet2DGoal = new QCursor(QPixmap("://images/cursor_pos.png"), 0, 0);

  for(int i = 0; i < 5; i++) {
    QImage img;
    QString img_file = QString("://images/robot_%1.png").arg(kConstRobotImage[i]);
    if(img.load(img_file)) {
      QMatrix matrix;
      matrix.rotate(90);
      auto img_obj = std::make_shared<QImage>(
          img.transformed(matrix, Qt::SmoothTransformation));
      kConstRobotImgs_[i] = img_obj;
    }
  }
  set_robot_status(robot_status_t::kStatusNormal);
  set_default_scale();
}

void RobotMap::deinit()
{

}

void RobotMap::set_robot_status(int status)
{
  if(status >= robot_status_t::kStatusNormal && status <= robot_status_t::kStatusFatal) {
    robot_img_ = kConstRobotImgs_[status];
    robot_state_ = status;
  }
}

void RobotMap::set_robot_size(const QSize &size)
{
  if(robot_img_) {
    robot_img_ = std::make_shared<QImage>(robot_img_->scaled(size));
  }
}

int RobotMap::qcolor_to_int(const QColor &color)
{
  return (int)(((unsigned int)color.blue() << 16) |
               (unsigned short)(((unsigned short)color.green() << 8) |
               color.red()));
}

void RobotMap::set_zoom_out()
{
  scale_value_ *= 1.1;
  setScale(scale_value_);
}

void RobotMap::set_zoom_in()
{
  scale_value_ *= 0.9;
  setScale(scale_value_);
}

void RobotMap::set_default_scale()
{
  setScale(kScaleDefault);
  moveBy(0, 0);
  scale_value_ = kScaleDefault;
}

void RobotMap::slot_set_init_pos()
{
  setCursor(*kCursorSetInitPose);
  curr_cursor_ = kCursorSetInitPose;
}

void RobotMap::slot_set_nav_goal()
{
  setCursor(*kCursorSet2DGoal);
  curr_cursor_ = kCursorSet2DGoal;
}

void RobotMap::slot_set_move_camera()
{
  setCursor(*kCursorMove);
  curr_cursor_ = kCursorMove;
}

void RobotMap::paint_image(int id, QImage *image)
{
  painter_img_[id] = image;
}

void RobotMap::paint_laser_scan(const QPolygonF &points)
{
  laser_points_ = points;
  update();
}

void RobotMap::paint_planner_path(const QPolygonF &path)
{
  planner_path_ = path;
  update();
}

void RobotMap::paint_maps(const QImage &map)
{
  if(map_img_ != map) {
    map_img_ = map;
    update();
  }
}

void RobotMap::paint_robot_pos(const algo::RobotPose &pos)
{
  //qDebug() << "pos:" << pos.x << " " << pos.y << " "<< pos.theta;
  robot_pos_ = QPointF(pos.x, pos.y);
  robot_yaw_ = pos.theta;
  update();
}

void RobotMap::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
  painter->setTransform(QTransform(QMatrix(1.0, 0.0, 0.0, 1.0,-map_img_.width()/2, -map_img_.height()/2)), true);
  drawMap(painter);
  drawRoboPos(painter);
  drawPlannerPath(painter);
  drawLaserScan(painter);
  drawTools(painter);
}

void RobotMap::drawTools(QPainter *painter)
{
  if (curr_cursor_ == kCursorSetInitPose || curr_cursor_ == kCursorSet2DGoal)
  {
    //绘制箭头
    if (pressed_point_.x() != 0 && pressed_point_.y() != 0 &&
        pressing_point_.x() != 0 && pressing_point_.y() != 0)
    {
      painter->setPen(QPen(QColor(0, 255, 0, 255), 2));
      //计算线弧度
      double theta = qAtan((pressing_point_.y() - pressed_point_.y()) /
                           (pressing_point_.x() - pressed_point_.x()));
      //根据固定长度计算箭头主体结束点坐标
      double dy = sin(theta) * 20;
      double dx = cos(theta) * 20;
      QPointF startPoint, endPoint;
      startPoint = pressed_point_;
      if (pressing_point_.x() - pressed_point_.x() > 0)
      {
        endPoint = QPointF(pressed_point_.x() + dx, pressed_point_.y() + dy);
      }
      else
      {
        endPoint = QPointF(pressed_point_.x() - dx, pressed_point_.y() - dy);
      }
      QLineF line(startPoint, endPoint);
      painter->drawLine(line);
      QLineF v = line.unitVector();
      if (!v.isNull())
      {
        v.setLength(5);
        v.translate(QPointF(line.dx(), line.dy()));
        QLineF n = v.normalVector();
        n.setLength(n.length() * 0.5);
        QLineF n2 = n.normalVector().normalVector();
        QPointF p1 = v.p2();
        QPointF p2 = n.p2();
        QPointF p3 = n2.p2();
        // painter->setBrush(QBrush(color));
        if (p1.isNull() == false && p2.isNull() == false)
        {
          QLineF lineA(p1, p2);
          if (lineA.length() > 4)
          {
            painter->drawLine(lineA);
          }
        }
        if (p2.isNull() == false && p3.isNull() == false)
        {
          QLineF lineB(p2, p3);
          if (lineB.length() > 4)
          {
            painter->drawLine(lineB);
          }
        }
        if (p3.isNull() == false && p1.isNull() == false)
        {
          QLineF lineC(p3, p1);
          if (lineC.length() > 4)
          {
            painter->drawLine(lineC);
          }
        }
      }
    }
  }
}

void RobotMap::drawMap(QPainter *painter)
{
  if(painter) {
    painter->drawImage(0, 0, map_img_);
  }
}

void RobotMap::drawRoboPos(QPainter *painter)
{
  painter->setPen(QPen(QColor(255, 0, 0, 255), 1, 
      Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter->save();

  painter->translate(robot_pos_);
  painter->rotate(algo::rad2deg(-robot_yaw_));

  QColor clr = get_current_color();
#if 0
  painter->setPen(Qt::red);
  painter->setBrush(Qt::red);
  painter->drawEllipse(-10, -10, 20, 20);
  painter->drawLine(0, 0, kInitWidth, 0);
  painter->drawLine(0, 0, 0, kInitHeight);
  painter->drawPolygon(QPolygonF()<<QPointF(kInitWidth-10, 10)<<QPointF(kInitWidth-10, -10)<<QPointF(kInitWidth, 0));
  painter->drawPolygon(QPolygonF()<<QPointF(10, kInitHeight-10)<<QPointF (-10, kInitHeight-10)<<QPointF(0, kInitHeight));
  painter->drawText(-50, -50, 100, 50, Qt::AlignHCenter, "(0 0)");
#endif
  
  painter->drawPoint(QPoint(0, 0));
  if(robot_img_) {
    painter->drawImage(QPoint(-robot_img_->width() / 2, -robot_img_->height() / 2), *robot_img_);
  }

#if 0
  QRadialGradient gradient(0, 0, 2*kInitSize);
  gradient.setColorAt(0, Qt::green);
  gradient.setColorAt(1, Qt::green);
  painter->setPen(QPen(Qt::black, 0));
  painter->setBrush(gradient);
  painter->drawEllipse(-2*kInitSize, -2*kInitSize,  4*kInitSize, 4*kInitSize);
  painter->setBrush(QBrush(QColor(255,0,128,150)));
  painter->drawPolygon(QPolygonF()
      << QPointF(kInitSize, 0)
      << QPointF(-0.6*kInitSize,  1*kInitSize)
      << QPointF(0, 0)
      << QPointF(-0.6*kInitSize, -1*kInitSize));
#endif

  painter->restore();
}

QColor RobotMap::get_current_color()
{
  switch (robot_state_)
  {
  case robot_status_t::kStatusNormal:
    return Qt::green;
  case robot_status_t::kStatusWarn:
    return Qt::yellow;
  case robot_status_t::kStatusError:
    return Qt::red;
  default:
    return Qt::green;
  }
}

void RobotMap::drawLaserScan(QPainter *painter)
{
  painter->setPen(QPen(QColor(255, 0, 0, 255), 1));
  painter->drawPoints(laser_points_);
}

void RobotMap::drawPlannerPath(QPainter *painter)
{
  painter->setPen(QPen(QColor(0, 0, 0, 255), 1));
  painter->drawPoints(planner_path_);
}

QRectF RobotMap::boundingRect() const
{
  return QRectF(-map_img_.width()/2, -map_img_.height()/2, map_img_.width(), map_img_.height());
}

void RobotMap::move(double x, double y)
{ 
  moveBy(x, y);
}

bool RobotMap::event(QEvent *event)
{
#if 0
	switch (event->type())
	{
	case QEvent::TouchBegin:
	case QEvent::TouchUpdate:
	case QEvent::TouchEnd:
	{
    //qDebug() << "RobotMap::touch event: " << event;
		auto touch_event = static_cast<QTouchEvent*>(event);
		auto &&touch_pt = touch_event->touchPoints();
		if (touch_pt.count() == 2)
		{
			const auto &first_pt = touch_pt.first();
			const auto &second_pt = touch_pt.last();
			auto scale_factor = QLineF(first_pt.pos(), second_pt.pos()).length() / 
				QLineF(first_pt.startPos(), second_pt.startPos()).length();
			if (touch_event->touchPointStates() & Qt::TouchPointReleased) {
				scale_view(scale_factor, second_pt.pos());
			}
		}
		else if (touch_pt.size() == 1)
		{
			const auto &first_pt = touch_pt.first();
			const auto &current_pos = first_pt.pos();
			const auto &last_pos = first_pt.lastPos();
			const auto &delta_pos = current_pos - last_pos;
			auto dx = int(delta_pos.x());
			auto dy = int(delta_pos.y());
			if (dx != 0 || dy != 0) {
				if(dx * dy > 0.001)
				 	scale_view(dx, current_pos);
				else 
					scale_view(1, current_pos);
			}
		}
		break;
	}
	default:
		break;
	}
  #endif
	return QGraphicsObject::event(event);
}

void RobotMap::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  setCursor(Qt::CrossCursor);
  scale_view(event->delta(), event->pos());
}

void RobotMap::scale_view(qreal scale, const QPointF &pt)
{
  if ((scale > 0) && (scale_value_ >= 50))
  {
    return;
  }
  else if ((scale < 0) && (scale_value_ <= kScaleMin))
  {
    // ResetItemPos(); //重置图片大小和位置，自适应控件窗口大小
    return;
  }
  else
  {
    qreal origin_scale = scale_value_;
    if (scale > 0) {
      scale_value_ *= 1.1; //每次放大10%
    } else {
      scale_value_ *= 0.9; //每次缩小10%
    }
    setScale(scale_value_);
    if (scale > 0) { 
      //使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
      moveBy(-pt.x() * origin_scale * 0.1,
             -pt.y() * origin_scale * 0.1); 
    } else {
      //使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
      moveBy(pt.x() * origin_scale * 0.1,
             pt.y() * origin_scale * 0.1); 
    }
  }
}

void RobotMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  //qDebug() << "RobotMap" << __FUNCTION__ << event;
  if (event->button() == Qt::LeftButton) {
    if (curr_cursor_ != kCursorMove) {
      pressed_point_ = event->pos();
    }
    start_pos_ = event->pos(); //鼠标左击时，获取当前鼠标在图片中的坐标，
    is_pressed_ = true;          //标记鼠标左键被按下
  } else if (event->button() == Qt::RightButton) {
    // ResetItemPos();//右击鼠标重置大小
  }
  event->accept();
  QGraphicsItem::mousePressEvent(event);
}

void RobotMap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  //qDebug() << "RobotMap" << __FUNCTION__ << event;
  pressing_point_ = event->pos();
  if (nullptr == curr_cursor_) {
    setCursor(*kCursorMove);
    curr_cursor_ = kCursorMove;
  }
  if (is_pressed_ && curr_cursor_ == kCursorMove) {
    QPointF point = (event->pos() - start_pos_) * scale_value_;
    moveBy(point.x(), point.y());
    update();
  }
  QGraphicsItem::mouseMoveEvent(event);
}

void RobotMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  //qDebug() << "RobotMap" << __FUNCTION__ << event;
  is_pressed_ = false;
  //如果是选择点位模式 重置
  if (curr_cursor_ == kCursorSetInitPose)
  {
    is_other_cursor_ = false;
    algo::RobotPose target_pos;
    target_pos.x = pressed_point_.x();
    target_pos.y = pressed_point_.y();
    target_pos.theta = algo::getAngle(pressed_point_.x(), pressed_point_.y(),
                                      pressing_point_.x(), pressing_point_.y());
    emit sig_pub_init_pos(target_pos);
    pressed_point_ = QPointF(0, 0);
    pressing_point_ = QPointF(0, 0);
    this->setCursor(*kCursorMove); //设置自定义的鼠标样式
    curr_cursor_ = kCursorMove;
  }
  else if (curr_cursor_ == kCursorSet2DGoal)
  {
    is_other_cursor_ = false;
    algo::RobotPose init_pos;
    init_pos.x = pressed_point_.x();
    init_pos.y = pressed_point_.y();
    init_pos.theta = algo::getAngle(pressed_point_.x(), pressed_point_.y(),
                                    pressing_point_.x(), pressing_point_.y());
    emit sig_pub_2d_goal(init_pos);
    pressed_point_ = QPointF(0, 0);
    pressing_point_ = QPointF(0, 0);
    this->setCursor(*kCursorMove);
    curr_cursor_ = kCursorMove;
  }
  QGraphicsItem::mouseReleaseEvent(event);
}

void RobotMap::hoverMoveEvent(QGraphicsSceneHoverEvent *event) 
{
  //qDebug() << __FUNCTION__ << event->pos() << event->scenePos();
  emit sig_cursor_pos(event->pos());
  QGraphicsItem::hoverMoveEvent(event);
}
