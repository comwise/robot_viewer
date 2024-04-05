#ifndef  __COMWISE__ROBOT_VIEWER__ITEM_BASE__H__
#define  __COMWISE__ROBOT_VIEWER__ITEM_BASE__H__

#include <QGraphicsObject>
#include "RobotViewerData.h"

class QGraphicsScene;

class ItemBase : public QGraphicsObject
{
    typedef unsigned int id_t;
public:
    explicit ItemBase(int type, QGraphicsObject *parent = nullptr)
        : QGraphicsObject(parent), type_(type) { }
    virtual ~ItemBase(){ }

    virtual id_t getID() const { return id_; }
    virtual void setID(id_t id) { id_ = id; }

    virtual int getType() const { return type_; }
    virtual void setType(int type) { type_ = type; }

protected:
    id_t id_{0};
    int type_{0};
};

#endif // __COMWISE__ROBOT_VIEWER__ITEM_BASE__H__