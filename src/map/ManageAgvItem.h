#ifndef __COMWISE__ROBOT_VIEWER__MANAGE_AGV_ITEM__H__
#define __COMWISE__ROBOT_VIEWER__MANAGE_AGV_ITEM__H__

#include "ManageItemBase.h"

class QGraphicsScene;
class QGraphicsLineItem;
class ItemInitPos;
class ItemAgv;
class RobotViewerScene;

class RobotInitPos
{
public:
    RobotInitPos(QGraphicsScene *sce = NULL);
    ~RobotInitPos();

    void AddInitItem();
    void DelInitItem();

private:
    QGraphicsScene *scene_;
    ItemInitPos *init_item_;
};

class RobotInitLine
{
public:
    RobotInitLine(QGraphicsScene *sce = NULL);
    ~RobotInitLine();

    void AddItem(const QPointF &startPt, const QPointF &endPt);
    void DeleteItem();

private:
    QGraphicsScene *scene_;
    QGraphicsLineItem *line_item_;
};

class ManageAgvItem : public ManageItemBase<ItemAgv>
{
public:
    ManageAgvItem(QGraphicsScene *sce = NULL);
    ~ManageAgvItem();

    void SetSence(QGraphicsScene *sce);
    
    void addAgvItem();
    void removeAgvItem();

    void SetLocationPress(const QPointF &pt);
    void SetLocationMove(const QPointF &pt);
    void SetLocationRelease(const QPointF &pt);
    bool IsLocPressed();

    void UpdateRobotPos(const QPointF &pos, qreal angle);

private:
    void SetAngleItem(const QPointF &startPt, const QPointF &endPt);
    void DeleteAngleItem();

    void SetBeginPos(const QPointF &pt);
    void SetEndPos();
    void getRobotAngle(const QPointF &startPt, const QPointF &endPt);

private:
    RobotViewerScene *scene_;          //场景
    ItemAgv *agv_item_;         //机器对象
    RobotInitPos *init_pos_;       //xy坐标系
    QGraphicsLineItem *line_item_; //方向线对象

    bool is_init_pressed_; //初始定位按钮是否按下

    QPointF press_pos_; //按压时位置
    QPointF robot_pos_; //机器人位置
    qreal robot_angle_; //机器人角度
};

#endif // __COMWISE__ROBOT_VIEWER__MANAGE_AGV_ITEM__H__