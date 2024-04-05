#ifndef __COMWISE__ROBOT_VIEWER__ITEM_MAP__H__
#define __COMWISE__ROBOT_VIEWER__ITEM_MAP__H__

#include "ItemBase.h"

QT_BEGIN_NAMESPACE
class QPainter;
class QStyleOptionGraphicsItem;
QT_END_NAMESPACE

class RobotViewerScene;
class ItemMap : public ItemBase
{
public:
    enum { Type = GraphicsItemType::ITEM_MAP };
    int type() const { return Type; }

public:
    ItemMap(const MapData &map_data, RobotViewerScene *map_scene);
    ~ItemMap();

    void showGrid(bool);

protected:
    virtual QPainterPath shape() const override;
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, 
        const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
    
private:
    void flush();

private:
    RobotViewerScene *scene_{nullptr};
    double size_w_{0};
    double size_h_{0};
    MapData map_data_;
    QImage *show_image_{nullptr};
    QVector<QLineF> grid_lines_;
    bool show_grid_{false};
};

#endif //__COMWISE__ROBOT_VIEWER__ITEM_MAP__H__
