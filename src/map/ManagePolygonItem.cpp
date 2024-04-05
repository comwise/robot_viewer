#include "ManagePolygonItem.h"
#include "RobotViewerScene.h"
#include <QDebug>

ManagePolygonItem::ManagePolygonItem()
{

}

void ManagePolygonItem::SetSence(QGraphicsScene *sce)
{
    scene_ = dynamic_cast<RobotViewerScene *>(sce);
    ManageItemBase::SetSence(sce);
}

void ManagePolygonItem::SetDefaultDesc(const QString &desc)
{
    default_desc_ = desc;
}

ItemPolygon* ManagePolygonItem::add(const QPolygonF &points)
{
    int id = ManageItemBase::GetUnusedId();
    return add(id, points);
}

ItemPolygon* ManagePolygonItem::add(int id, const QPolygonF &points)
{
    ItemPolygon *item = new ItemPolygon(scene_);
    if (item) {
        item->setID(id);
        item->setDesc(default_desc_ + QString::number(id));
        item->setPolygon(points);
        AddItem(item);
    }
    return item;
}

void ManagePolygonItem::remove(ItemPolygon *item)
{
    DelItem(item);
}

void ManagePolygonItem::remove(int id)
{
    if(auto item = find(id)) {
        DelItem(item);
    }
}

void ManagePolygonItem::removeAll()
{
    foreach (ItemPolygon* item, items_) {
        if (item) {
            DelItem(item);
        }
    }
}

bool ManagePolygonItem::setPolygon(int id, const QPolygonF &points)
{
    if(auto item = find(id)) {
        item->setPolygon(points);
        return true;
    }
    return false;
}

bool ManagePolygonItem::getPolygon(QVector<QPolygonF> &poly_data)
{
    poly_data.clear();
    foreach (ItemPolygon* item, items_) {
        if (item) {
            QPolygonF data = item->polygon();
            poly_data.push_back(data);
        }
    }
    return true;
}

bool ManagePolygonItem::setPolygon(const QVector<PolygonData> &poly_data)
{
    removeAll();

    for(auto &data : poly_data) {
        auto item = add(data.id, data.data);
        if(item) {
            item->setData(data);
        }
    }

    return true;
}

bool ManagePolygonItem::getPolygon(QVector<PolygonData> &poly_data)
{
    poly_data.clear();
    foreach (ItemPolygon* item, items_) {
        if (item) {
            PolygonData data = item->getData();
            poly_data.push_back(data);
        }
    }
    return true;
}

ItemPolygon* ManagePolygonItem::find(const QPointF &pos)
{
    foreach (ItemPolygon* item, items_) {
        if(item && item->contains(pos)) {
            return item;
        }
    }
    return nullptr;
}

ItemPolygon* ManagePolygonItem::find(int id)
{
    foreach (ItemPolygon* item, items_) {
        if(item && item->getID() == id) {
            return item;
        }
    }
    return nullptr;
}
