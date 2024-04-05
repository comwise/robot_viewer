#include "ManageAgvItem.h"
#include "RobotViewerScene.h"
#include "ItemAgv.h"
#include "ItemInitPos.h"
#include "RobotViewerLog.h"
#include "RobotViewerData.h"

RobotInitPos::RobotInitPos(QGraphicsScene *sce)
    : scene_(sce)
    , init_item_(NULL)
{
    AddInitItem();
}

RobotInitPos::~RobotInitPos()
{
    DelInitItem();
}

void RobotInitPos::AddInitItem()
{
    init_item_ = new ItemInitPos(scene_);
    scene_->addItem(init_item_);
}

void RobotInitPos::DelInitItem()
{
    scene_->removeItem(init_item_);
    delete init_item_;
    init_item_ = NULL;
}

RobotInitLine::RobotInitLine(QGraphicsScene *sce)
    : scene_(sce), line_item_(NULL)
{
}

RobotInitLine::~RobotInitLine()
{
}

void RobotInitLine::AddItem(const QPointF &startPt, const QPointF &endPt)
{
    if (NULL == line_item_)
    {
        line_item_ = new QGraphicsLineItem(QLineF(startPt, endPt));
        line_item_->setPen(QPen(Qt::magenta));
        scene_->addItem(line_item_);
    }
    else
    {
        line_item_->setLine(QLineF(startPt, endPt));
    }
}

void RobotInitLine::DeleteItem()
{
    if (line_item_ && scene_)
    {
        scene_->removeItem(line_item_);
        delete line_item_;
        line_item_ = NULL;
    }
}

ManageAgvItem::ManageAgvItem(QGraphicsScene *sce)
    : scene_(NULL)
    , agv_item_(NULL)
    , line_item_(NULL)
    , init_pos_(new RobotInitPos(sce))
    , is_init_pressed_(false)
    , robot_angle_(0)
{
}

ManageAgvItem::~ManageAgvItem()
{
}

void ManageAgvItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

void ManageAgvItem::addAgvItem()
{
    agv_item_ = new ItemAgv();
    AddItem(agv_item_);
}

void ManageAgvItem::removeAgvItem()
{
    DelItem(agv_item_);
}

void ManageAgvItem::UpdateRobotPos(const QPointF &pos, qreal angle)
{
    if (agv_item_)
    {
        // TRACE("update: x=%lf, y=%lf, angle=%lf\n", pos.x(), pos.y(), angle);
        agv_item_->setPos(pos);
        agv_item_->setRotation(angle);
        scene_->update();
    }
}

void ManageAgvItem::SetLocationPress(const QPointF &pt)
{
    TRACE("press: pos_x=%lf, pos_y=%lf\n", pt.x(), pt.y());
    is_init_pressed_ = true;
    press_pos_ = pt;
    robot_angle_ = 0;
    SetBeginPos(press_pos_);
    SetAngleItem(press_pos_, press_pos_);
}

void ManageAgvItem::SetLocationMove(const QPointF &pt)
{
    if (!press_pos_.isNull())
    {
        SetAngleItem(press_pos_, pt);
        getRobotAngle(press_pos_, pt);
        UpdateRobotPos(robot_pos_, robot_angle_);
    }
}

void ManageAgvItem::SetLocationRelease(const QPointF &pt)
{
    DeleteAngleItem();
    is_init_pressed_ = false;
    press_pos_ = QPoint();
    SetEndPos();
}

bool ManageAgvItem::IsLocPressed()
{
    return is_init_pressed_;
}

void ManageAgvItem::SetAngleItem(const QPointF &startPt, const QPointF &endPt)
{
    if (NULL == line_item_)
    {
        line_item_ = new QGraphicsLineItem(QLineF(startPt, endPt));
        line_item_->setPen(QPen(Qt::magenta));
        scene_->addItem(line_item_);
    }
    else
    {
        line_item_->setLine(QLineF(startPt, endPt));
    }
}

void ManageAgvItem::DeleteAngleItem()
{
    if (line_item_ && scene_)
    {
        scene_->removeItem(line_item_);
        delete line_item_;
        line_item_ = NULL;
    }
}

void ManageAgvItem::SetBeginPos(const QPointF &pos)
{
    robot_pos_ = pos;
    if (agv_item_)
    {
        agv_item_->setPos(pos);
    }
}

void ManageAgvItem::SetEndPos()
{
    if (agv_item_)
    {
        agv_item_->setPos(robot_pos_);
    }
    TRACE("end: x=%lf, y=%lf, angle=%lf\n", robot_pos_.x(), robot_pos_.y(), robot_angle_);

    OrientPos send_pos_;
    send_pos_.x = robot_pos_.x() / 100;
    send_pos_.y = robot_pos_.y() / 100;
    send_pos_.angle = robot_angle_ * 3.1415 / 180;

    // std::shared_ptr<Publisher<OrientPos>> pub = NODE.advertise<OrientPos>("msg_init_pos");
    // if (pub.get()) {
    //     pub->publish(send_pos_);
    // }
}

void ManageAgvItem::getRobotAngle(const QPointF &startPt, const QPointF &endPt)
{
#if 0
    VecPosition sp = VecPosition(startPt.x(), startPt.y());
    VecPosition ep = VecPosition(endPt.x(), endPt.y());
    robot_angle_ = (ep - sp).getDirection();
#endif
}
