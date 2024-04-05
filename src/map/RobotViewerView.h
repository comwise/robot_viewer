#ifndef __COMWISE__ROBOT_VIEWER__VIEW__H__
#define __COMWISE__ROBOT_VIEWER__VIEW__H__

#include <QGraphicsView>

QT_BEGIN_NAMESPACE
class QTextStream;
class QKeyEvent;
class QMouseEvent;
class QFocusEvent;
class QPaintEvent;
QT_END_NAMESPACE

class RobotViewerView : public QGraphicsView
{
    Q_OBJECT
public:
    RobotViewerView(QWidget *parent = 0);
    
    double getScaleHistory();
    void scaleView(qreal scaleFactor);
    void updateSceneScale();

public slots:
    void setInsertType(int type);

protected:
    virtual bool event(QEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

signals:
    void sigSetInsertType(int);
    void sigMousePosChanged(QPointF);
    void sigMovePosChanged(QPointF);
    void sigScaleChanged(qreal);

private:
    QCursor setInsertCursor(int type);
    QGraphicsView::DragMode setInsertDragMode(int type);

    void moveViewPos(int x, int y);

private:
    qreal scale_history_{1.0};
};

#endif // __COMWISE__ROBOT_VIEWER__VIEW__H__
