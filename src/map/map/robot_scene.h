#ifndef __COMWISE__ROBOT_VIEWER__ROBOT_SCENE__H__
#define __COMWISE__ROBOT_VIEWER__ROBOT_SCENE__H__

#include <QGraphicsScene>
#include <QPointF>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QTimer;
class QImage;
QT_END_NAMESPACE

class RobotScene : public QGraphicsScene
{
  Q_OBJECT
public:
  explicit RobotScene(QObject *parent = 0);
  ~RobotScene();

signals:
  void sig_cursor_pos(QPointF);
  void sig_pub_map_image(const QImage &);

private slots:
  void slot_pub_map_image(); // publish map image

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) override;

private:
  QPointF last_pos_;
  bool is_pressed_{false};

  // pub map image
  QTimer *map_img_timer_{nullptr};
};

#endif // __COMWISE__ROBOT_VIEWER__ROBOT_SCENE__H__
