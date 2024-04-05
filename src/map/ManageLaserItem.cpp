#include "ManageLaserItem.h"
#include "ItemReflector.h"
#include <QGraphicsScene>

ManageLaserItem::ManageLaserItem() 
    : laser_heading_(0),
    laser_dist_(0),
    reflector_(nullptr),
    reflector_change_(false),
    laser_scan_change_(false),
    laser_scan_3d_(nullptr),
    laser_scan_3d_change_(false),
    scene_(NULL),
    is_visible_(true)
{
    laserscan_item_list_.clear();
    reflector_item_list_.clear();
}

void ManageLaserItem::SetSecne(QGraphicsScene *scene)
{
    scene_ = scene;
}

void ManageLaserItem::SetConfig(double laser_dist, double laser_head)
{
    laser_heading_ = laser_head;
    laser_dist_ = laser_dist;
}

void ManageLaserItem::setAgvPos(const OrientPos &agv_pos)
{
    std::lock_guard<std::mutex> lock(agv_pos_mutex_);
    scene_robotpos_ = agv_pos;
}

void ManageLaserItem::setReflector(const GReflector &reflector)
{
    std::lock_guard<std::mutex> lock(reflector_mutex_);
    //*reflector_ = reflector;
    reflector_change_ = true;
}

void ManageLaserItem::setLaserScan(const QPolygonF &laser_scan)
{
    std::lock_guard<std::mutex> lock(laser_scan_mutex_);
    laser_scan_ = laser_scan;
    laser_scan_change_ = true;
}

void ManageLaserItem::setLaserScan3D(const Glaser3DBeam &laser)
{
    std::lock_guard<std::mutex> lock(laser_scan_3d_mutex_);
    //*laser_scan_3d_ = laser;
    laser_scan_3d_change_ = true;
}

void ManageLaserItem::UpdateItems()
{
    if (NULL == scene_)
    {
        return;
    }

#if 0

    if (reflector_change_)
    {
        ClearReflector();
        GReflector rec;
        {
            std::lock_guard<std::mutex> lock(reflector_mutex_);
            rec = *reflector_;
        }
        DrawReflector(rec);
        reflector_change_ = false;
    }
#endif

    if (laser_scan_change_)
    {
        QPolygonF laser_scan;
        {
            std::lock_guard<std::mutex> lock(laser_scan_mutex_);
            laser_scan = laser_scan_;
        }
        DrawLaserScan(laser_scan);
        laser_scan_change_ = false;
    }

#if 0
    if (laser_scan_3d_change_)
    {
        Glaser3DBeam laser_scan_3d;
        {
            std::lock_guard<std::mutex> lock(laser_scan_3d_mutex_);
            laser_scan_3d.Swap(laser_scan_3d_);
        }
        DrawLaserScan3D(laser_scan_3d);
        laser_scan_3d_change_ = false;
    }
#endif
}

void ManageLaserItem::DrawLaserScan3D(const Glaser3DBeam &laser_data)
{
    OrientPos center_pos;
    {
        std::lock_guard<std::mutex> lock(agv_pos_mutex_);
        center_pos = scene_robotpos_;
    }
#if 0
    if (laserscan_item_list_.isEmpty()) {
        for (int i = 0; i < laser_data.laser_beams_size(); ++i) {
            double x = laser_data.laser_beams(i).x()*100;
            double y = laser_data.laser_beams(i).y()*100;
            QGraphicsRectItem* item = new QGraphicsRectItem(x, y, 1, 1);
            if (item) {
                item->setPen(QPen(Qt::red));
                item->setBrush(QBrush(Qt::red));
            }
            if (scene_) {
                scene_->addItem(item);
                laserscan_item_list_.push_back(item);
            }
        }
    }

    for (int i = 0; i < laser_data.laser_beams_size(); ++i) {
        double x = laser_data.laser_beams(i).x()*100;
        double y = laser_data.laser_beams(i).y()*100;
        
        if (i < laserscan_item_list_.size() && laserscan_item_list_[i])
        {
            laserscan_item_list_[i]->setVisible(is_visible_);
            if (is_visible_) {
                laserscan_item_list_[i]->setRect(x, y, 1, 1);
                laserscan_item_list_[i]->update();
            }
        }
    }

    if (scene_) {
        //scene_->update();
    }
#endif
}

void ManageLaserItem::DrawLaserScan(const QPolygonF &curr_scan)
{
    OrientPos centerpos;
    {
        std::lock_guard<std::mutex> lock(agv_pos_mutex_);
        centerpos = scene_robotpos_;
    }
#if 0
    QGraphicsRectItem *laseritem;
    if (laserscan_item_list_.isEmpty()) {
        for (int i = 0; i < curr_scan.beams_size(); ++i) {
            laseritem = new QGraphicsRectItem(0, 0, 1, 1);
            laseritem->setPen(QPen(Qt::red));
            laseritem->setBrush(QBrush(Qt::red));
            laserscan_item_list_.push_back(laseritem);
            scene_->addItem(laseritem);
        }
    }

    for (int i = 0; i < curr_scan.beams_size(); ++i) 
    {
        if (laser_dist_<0)
        {
            laser_dist_= - laser_dist_;
        }
        double d_angle = -90 + i * 0.5;
        if (curr_scan.beams_size()!=361)
        {
            d_angle=curr_scan.beams(i).angle()*180 / M_PI;
        }
        VecPosition addOn(curr_scan.beams(i).dist() * 100.0f, d_angle, POLAR); 
        addOn.setX(addOn.getX() + laser_dist_/10);
        addOn = addOn.rotate(VecPosition::normalizeAngle(centerpos.angle + laser_heading_));
        VecPosition center_pos(centerpos.x, centerpos.y, CARTESIAN); // cm
        VecPosition laserpos = VecPosition(center_pos.getX() + addOn.getX(), center_pos.getY()+ addOn.getY());
        if (i < laserscan_item_list_.size())
        {
            QGraphicsRectItem* pRectItem = laserscan_item_list_[i];
            if(pRectItem != NULL)
            {
                pRectItem->setRect(laserpos.getX(), laserpos.getY(), 4, 4);
                pRectItem->update();
            }
        }
    }
#else
    QGraphicsRectItem *laseritem = nullptr;
    if (laserscan_item_list_.isEmpty()) {
        for (int i = 0; i < curr_scan.size(); ++i) {
            QPointF pt = curr_scan[i];
            laseritem = new QGraphicsRectItem(0, 0, 1, 1);
            laseritem->setPen(QPen(Qt::red));
            laseritem->setBrush(QBrush(Qt::red));
            
            laserscan_item_list_.push_back(laseritem);
            scene_->addItem(laseritem);
        }
    }
    for (int i = 0; i < curr_scan.size(); ++i)  {
        if (i < laserscan_item_list_.size()) {
            QPointF pt = curr_scan[i];
            QGraphicsRectItem* pRectItem = laserscan_item_list_[i];
            if(pRectItem) {
                pRectItem->setRect(pt.x(), pt.y(), 2, 2);
                pRectItem->update();
            }
        }
    }
#endif
}

void ManageLaserItem::DrawReflector(const GReflector &reflector)
{
    OrientPos center_pos;
    {
        std::lock_guard<std::mutex> lock(agv_pos_mutex_);
        center_pos = scene_robotpos_;
    }
#if 0
    ItemReflector *reflector_item;
    if (reflector_item_list_.isEmpty()) {
        for (int i = 0; i < reflector.reflectors_size(); ++i) 
        {
            reflector_item = new ItemReflector("");
            reflector_item->setDetect(2);
            reflector_item_list_.push_back(reflector_item);
            scene_->addItem(reflector_item);
        }
    }
    else
    {
        return;
    }

    for (int i = 0; i < reflector.reflectors_size(); ++i) 
    {
        GReflPos reflector_pos = reflector.reflectors(i);
        VecPosition addOn(reflector_pos.x() * 100, reflector_pos.y() * 100, CARTESIAN);
        addOn.rotate(center_pos.angle);
        VecPosition m_center_pos(center_pos.x, center_pos.y, CARTESIAN); // cm
        VecPosition reflect_absolut_pos = VecPosition(
            m_center_pos.getX() + addOn.getX(),
            m_center_pos.getY() + addOn.getY());
        reflector_item_list_[i]->setPos(reflect_absolut_pos.getX(), reflect_absolut_pos.getY());
        reflector_item_list_[i]->update();
    }
#endif
}

void ManageLaserItem::ClearReflector()
{
    if (!reflector_item_list_.isEmpty())
    {
        for (int i = 0; i < reflector_item_list_.size(); i++)
        {
            scene_->removeItem(reflector_item_list_[i]);
            delete reflector_item_list_[i];
            reflector_item_list_[i] = NULL;
        }
        reflector_item_list_.clear();
    }
}

void ManageLaserItem::SetLaserVisible(bool flag)
{
    is_visible_ = flag;
}
