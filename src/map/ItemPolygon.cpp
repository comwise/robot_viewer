#include "ItemPolygon.h"
#include "RobotViewerScene.h"
#include <iostream>
#include <QPainter>
#include "DialogPolygon.h"

ItemPolygon::ItemPolygon(RobotViewerScene *sce)
    : scene_(sce)
{
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsFocusable, true);
    //setCacheMode(ItemCoordinateCache);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setZValue(-26);
}

ItemPolygon::~ItemPolygon()
{

}

int ItemPolygon::getID() const
{ 
    return data_.id;
}

void ItemPolygon::setID(int id)
{
    data_.id = id;
}

void ItemPolygon::setDesc(const QString &desc)
{
    data_.desc = desc.toStdString();
}

QString ItemPolygon::getDesc() const
{
    return data_.desc.c_str();
}

void ItemPolygon::setData(const PolygonData &data)
{
    data_ = data;
    setPolygon(data_.data);
}

PolygonData ItemPolygon::getData()
{
    data_.data = polygon();
    return data_;
}

void ItemPolygon::setStorage(int area_id, int storage_id)
{
    data_.area_id = area_id;
    data_.storage_id = storage_id;
}

void ItemPolygon::getStorage(int &area_id, int &storage_id)
{
    area_id = data_.area_id;
    storage_id = data_.storage_id;
}

void ItemPolygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    setPen(QPen(QColor(0, 128, 0), 5, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(QBrush(QColor(0, 0, 0, 60)));
    setFillRule(Qt::WindingFill);
    QGraphicsPolygonItem::paint(painter, option, widget);
    auto poly = polygon();
    for (int i = 0; i < poly.size(); ++i) {
        if(i == poly.size()-1 && poly[i] == poly[0]) {
            break;
        }
        QPointF pt = poly[i];
        painter->setPen(QPen(QColor(Qt::darkYellow), 3));
        painter->drawText(pt, QString("%1(%2,%3)").arg(i).arg(pt.x(), 0, 'f', 1).arg(pt.y(), 0, 'f', 1));
    }
    painter->setPen(QPen(QColor(Qt::magenta), 10));
    painter->drawText(boundingRect(), Qt::AlignCenter, data_.desc.c_str());
}

void ItemPolygon::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(scene_) {
        scene_->slotContextMenuEvent(2);
    }
    QGraphicsPolygonItem::contextMenuEvent(event);
}

void ItemPolygon::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    DialogPolygon dlg(getData());
    if(dlg.exec() == QDialog::Accepted) {
        data_ = dlg.getData();
        setPolygon(data_.data);
#if 0
        StorageEvent e;
        e.data = data_;
        if(scene_) {
            emit scene_->storageChanged(e);
        }
#endif
    }
    QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}
