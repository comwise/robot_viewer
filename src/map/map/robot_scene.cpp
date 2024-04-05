#include "map/robot_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QPainter>
#include <QDebug>

RobotScene::RobotScene(QObject *parent)
{
  setParent(parent);
  map_img_timer_ = new QTimer;
  map_img_timer_->setInterval(100);
  connect(map_img_timer_, SIGNAL(timeout()),
      this, SLOT(slot_pub_map_image()));
  map_img_timer_->start();
}

RobotScene::~RobotScene()
{
  if(map_img_timer_)
    delete map_img_timer_;
  map_img_timer_ = nullptr;
}

void RobotScene::slot_pub_map_image()
{
  QImage image(600, 600, QImage::Format_RGB888);
  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  render(&painter);
  emit sig_pub_map_image(image);
}

void RobotScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  last_pos_ = event->scenePos();
  is_pressed_ = true;
  QGraphicsScene::mousePressEvent(event);
}

void RobotScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  //qDebug() << __FUNCTION__ << event->scenePos();
  emit sig_cursor_pos(event->scenePos());
  if (is_pressed_) {
#if 0
    move(this->x() + (event->scenePos().X() - last_pos_.x()),
         this->y() + (event->scenePos().y() - last_pos_.y()));
    last_pos_ = event->scenePos();
#endif
  }
  QGraphicsScene::mouseMoveEvent(event);
}

void RobotScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  last_pos_ = event->scenePos();
  is_pressed_ = false;
  QGraphicsScene::mouseReleaseEvent(event);
}

void RobotScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
  QGraphicsScene::wheelEvent(event);
}