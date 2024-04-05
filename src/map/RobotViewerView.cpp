#include "RobotViewerView.h"
#include "RobotViewerScene.h"
#include "RobotViewerData.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QDebug>

namespace {
    static const double kInitViewScale = 1;
    static const double kScaleFactor = 1.1;
}

RobotViewerView::RobotViewerView(QWidget *parent)
    : QGraphicsView(parent)
{
    setCacheMode(CacheBackground);//背景缓存模式
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); //视口更新模式
    setRenderHint(QPainter::Antialiasing);//抗锯齿
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setMouseTracking(true); //跟踪鼠标
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //setMatrix(QMatrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0), true); //转化下视图

    scale(kInitViewScale, kInitViewScale);
    scale_history_ = kInitViewScale;
    //centerOn(0, 0);

    setMinimumSize(400, 400);

    setInsertCursor(ACTION_SELECT);
}

bool RobotViewerView::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
#if 0
        //qDebug() << "RobotViewerView::touch event: " << event;
        auto touch_event = static_cast<QTouchEvent*>(event);
        auto &&touch_pt = touch_event->touchPoints();
        if (touch_pt.count() == 2)
        {
            const auto &first_pt = touch_pt.first();
            const auto &second_pt = touch_pt.last();
            auto scale_factor = QLineF(first_pt.pos(), second_pt.pos()).length() / 
                QLineF(first_pt.startPos(), second_pt.startPos()).length();
            if (touch_event->touchPointStates() & Qt::TouchPointReleased) {
                scaleView(scale_factor);
            }
        }
#endif
        break;
    }
    default:
        break;
    }
    return QGraphicsView::event(event);
}

void RobotViewerView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        moveViewPos(0, -100);
        break;
    case Qt::Key_Down:
        moveViewPos(0, 100);
        break;
    case Qt::Key_Left:
        moveViewPos(-100, 0);
        break;
    case Qt::Key_Right:
        moveViewPos(100, 0);
        break;
    case Qt::Key_Period:
        scaleView(qreal(kScaleFactor));
        break;
    case Qt::Key_Comma:
        scaleView(1 / qreal(kScaleFactor));
        break;
    case Qt::Key_Escape:
    {
        setInsertType(ACTION_SELECT);
        QGraphicsView::keyPressEvent(event);
        break;
    }
    case Qt::Key_Space:
    {
        QPointF pos(0, 0);
        int value = 1 /*((RobotViewerScene *)scene())->getSelectedNodeCenter(pos)*/;
        if (value != 0)
        {
            scaleView(1 / scale_history_);
            // centerOn(pos);//聚焦于选择的中心
        }
        break;
    }
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void RobotViewerView::wheelEvent(QWheelEvent *event)
{
    scaleView(event->delta());
}

void RobotViewerView::scaleView(qreal scale_val)
{
    // qreal factor = transform().scale(scale_val, scale_val).mapRect(QRectF(0, 0, 1, 1)).width();
    //qDebug() << scale_val << scale_history_;
    if (scale_history_ > 50 && scale_val > 0) {
        return;
    } else if (scale_history_ < 0.3 && scale_val < 0) {
        return;
    } else {
        if (scale_val > 0) {
            scale(kScaleFactor, kScaleFactor);
        } else {
            scale(1.0/kScaleFactor, 1.0/kScaleFactor);
        }
        updateSceneScale();
    }
}

void RobotViewerView::updateSceneScale()
{
    scale_history_ = this->matrix().m11();
    sigScaleChanged(scale_history_);
    ((RobotViewerScene *)scene())->scaleItem(scale_history_);
}

double RobotViewerView::getScaleHistory()
{
    return scale_history_;
}

void RobotViewerView::moveViewPos(int x, int y)
{
    setTransformationAnchor(NoAnchor);
    QTransform transform;
    transform.translate(x, y);
    setTransform(transform, true);
    setTransformationAnchor(AnchorViewCenter);
}

void RobotViewerView::setInsertType(int type)
{
    setInsertCursor(type);
    setInsertDragMode(type);
    ((RobotViewerScene *)scene())->setInsertType(type);
}

QCursor RobotViewerView::setInsertCursor(int type)
{
    QCursor cursor(Qt::ArrowCursor);
    switch (type)
    {
    case ACTION_SELECT:
        cursor = QCursor(Qt::ArrowCursor);
        break;
    case ACTION_MOVE:
        cursor = QCursor(Qt::SizeAllCursor);
        break;
    case ADD_NODE:
        cursor = QCursor(QPixmap(QString::fromUtf8(":/images/map/node.png")));
        break;
    case ADD_SITE:
        cursor = QCursor(QPixmap(QString::fromUtf8(":/images/map/site.png")));
        break;
    case ADD_EDGE:
        cursor = QCursor(QPixmap(QString::fromUtf8(":/images/map/pen.png")));
        break;
    case ADD_AREA:
        cursor = QCursor(Qt::CrossCursor);
        break;
    case SET_LOCROBOT:
        cursor = QCursor(QPixmap(QString::fromUtf8(":/images/map/location.png")));
        break;
    case SET_NAVTARGET:
        cursor = QCursor(QPixmap(QString::fromUtf8(":/images/map/navigation.png")));
        break;
    default:
        cursor = QCursor(Qt::ArrowCursor);
        break;
    }
    setCursor(cursor);
    return cursor;
}

QGraphicsView::DragMode RobotViewerView::setInsertDragMode(int type)
{
    QGraphicsView::DragMode drag_Mode;
    switch (type)
    {
    case ACTION_HANDDRAG:
        drag_Mode = ScrollHandDrag;
        break;
    default:
        drag_Mode = NoDrag;
        break;
    }
    setDragMode(drag_Mode);
    return drag_Mode;
}

void RobotViewerView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton || 
        event->button() == Qt::RightButton) {
        emit sigSetInsertType(ACTION_SELECT);
    }
    QGraphicsView::mousePressEvent(event);
}

void RobotViewerView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pt = mapToScene(event->pos());
    emit sigMovePosChanged(pt);
    QGraphicsView::mouseMoveEvent(event);
}