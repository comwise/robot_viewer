#include "ItemSite.h"
#include "ItemEdge.h"
#include "ItemNode.h"
#include "DialogSite.h"
#include "RobotViewerScene.h"
#include <QAction>
#include <QMenu>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>
#include <QPainter>

const double g_site_size = 50;

ItemSite::ItemSite(ItemEdge *parentedge, double percent, RobotViewerScene *scene)
    : ItemBase(GraphicsItemType::ITEM_SITE)
    , scale_factor_(1.0/0.3)
    , contextMenu_(NULL)
    , scene_(scene)
{
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsFocusable, true);
    setCacheMode(DeviceCoordinateCache);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setZValue(-0.5);

    shape_r = g_site_size;

    parent_edge_ = parentedge;
    parent_edge_->addSiteToEdge(this);

    site_data_.startid = parent_edge_->sourceNode()->getID();
    site_data_.endid = parent_edge_->destNode()->getID();
    site_data_.parentedgeid = parent_edge_->getID();
    site_data_.percent = percent;

    device_connected_ = false;
    hover_enter_ = false;

    is_array_temp_ = false;

    createContextMenu();

    connect(this, SIGNAL(mouseRightClicked(int)), scene_, SLOT(slotContextMenuEvent(int)));
}

ItemSite::~ItemSite()
{

}

void ItemSite::createContextMenu()
{
    contextMenu_ = new QMenu;

    QAction *attrAct = new QAction(tr("属性"), contextMenu_);
    connect(attrAct, SIGNAL(triggered()), this, SLOT(aboutAttribute()));

    contextMenu_->addSeparator();
    contextMenu_->addAction(attrAct);
}

void ItemSite::aboutAttribute()
{
    DialogSite dlg;
    dlg.setData(site_data_);
    if (dlg.exec() == QDialog::Accepted) {
        SiteData data = dlg.getData();
        if (data.pos.x != site_data_.pos.x || data.pos.y != site_data_.pos.y) {
            adjustWithEdgeMoved(QPointF(data.pos.x, data.pos.y));
        }
        setSiteData(data);
    }
}

SiteData ItemSite::getSiteData()
{
    return site_data_;
}

void ItemSite::setSiteData(const SiteData &site_data)
{
    site_data_ = site_data;
}

QRectF ItemSite::getBaseRect()
{
    double shaper = 2 * g_site_size / scale_factor_;
    QRectF rect(-shaper, -shaper, 2 * shaper, 2 * shaper);
    return rect;
}

QRectF ItemSite::boundingRect() const
{
    double shaper = 2 * g_site_size / scale_factor_;
    qreal adjust = 3.0 / scale_factor_;
    return QRectF(-shaper-adjust-2*shaper, -shaper-adjust-2*shaper,
            8*(shaper + adjust), 8*(shaper + adjust));
}

QPainterPath ItemSite::shape() const
{
    double shaper = g_site_size / scale_factor_;
    if (hover_enter_) {
        shaper = 2 * shape_r;
    }

    QPainterPath path;
    path.addEllipse(-shaper, -shaper, 2 * shaper, 2 * shaper);
    return path;
}

void ItemSite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    scale_factor_ = scale();
    painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
    drawCommonNode(painter);
}

void ItemSite::drawCommonNode(QPainter *painter)
{
    QColor light_color_;
    QColor main_color_;
    QColor arrow_color_;

    if (hover_enter_)
    {
        shape_r = 2 * g_site_size / scale_factor_;
    }
    else
    {
        shape_r = g_site_size / scale_factor_;
    }

    if (site_data_.yaanback != HEAD_IN_FRONT) //叉齿在前 ，箭头颜色为洋葱绿
    {
        arrow_color_ = QColor(Qt::cyan);
    }
    else
    {
        arrow_color_ = QColor(Qt::red); //叉齿在后 ，箭头颜色为红色
    }

    main_color_ = QColor(Qt::red);
    light_color_ = QColor(Qt::red);
    if (device_connected_)
    {
        main_color_ = QColor(Qt::green);
        light_color_ = QColor(Qt::green);
    }

    if (is_array_temp_)
    {
        main_color_ = QColor(255, 0, 128, 150);
        light_color_ = QColor(255, 0, 128, 150);
        arrow_color_ = QColor(255, 0, 128, 150);
    }

    QRadialGradient gradient(-3, -3, 10);
    QPainterPath path = parent_edge_->getPath();
    double angle = path.angleAtPercent(site_data_.percent);
    if (site_data_.direction == POSITIVE_DIR)
    {
        angle = angle;
    }
    else
    {
        angle = 180 + angle;
        if (angle >= 360)
        {
            angle = angle - 360;
        }
    }

    QPen pen;
    if (!isSelected())
    {
        gradient.setColorAt(1, light_color_);
        gradient.setColorAt(0, main_color_);
        pen = QPen(Qt::black, 0);
    }
    else
    {
        gradient.setColorAt(0, light_color_);
        gradient.setColorAt(1, main_color_);
        pen = QPen(Qt::yellow, 0.2);
    }
    if (is_array_temp_)
    {
        pen = QPen(Qt::NoPen);
    }
    painter->setPen(pen);
    painter->setBrush(gradient);
    painter->rotate(angle);
    painter->drawEllipse(-shape_r, -shape_r, 2 * shape_r, 2 * shape_r);
    painter->setBrush(QBrush(arrow_color_));

    painter->drawPolygon(QPolygonF()
                         << QPointF(shape_r, 2 * shape_r)
                         << QPointF(-0.6 * shape_r, 0.8 * shape_r + 2 * shape_r)
                         << QPointF(0, 2 * shape_r)
                         << QPointF(-0.6 * shape_r, -0.8 * shape_r + 2 * shape_r));

    //////////////////////////////////////////////////////////////////////////精确到点
    if (site_data_.precisestop == STOP_PRECISE)
    {
        painter->setPen(QPen(Qt::red, 4));
        painter->drawLine(QLineF(QPointF(-0.75 * shape_r, -0.75 * shape_r), QPointF(-1.5 * shape_r, -1.5 * shape_r)));
        painter->drawLine(QLineF(QPointF(0.75 * shape_r, -0.75 * shape_r), QPointF(1.5 * shape_r, -1.5 * shape_r)));
        painter->drawLine(QLineF(QPointF(0.75 * shape_r, 0.75 * shape_r), QPointF(1.5 * shape_r, 1.5 * shape_r)));
        painter->drawLine(QLineF(QPointF(-0.75 * shape_r, 0.75 * shape_r), QPointF(-1.5 * shape_r, 1.5 * shape_r)));
    }

    //绘制文字
    painter->rotate(-angle);
    QRectF rect = QRectF(QPointF(-shape_r, -shape_r), QSizeF(2 * shape_r, 2 * shape_r));
    painter->setFont(QFont("Times", 0.9 * shape_r, QFont::Bold));
    QString str;
    str.setNum(site_data_.id);

    painter->setPen(Qt::black);
    if (device_connected_)
    {
        painter->setPen(Qt::black);
    }
    painter->drawText(rect, Qt::AlignCenter, str);
    QRectF rec = QRectF(QPointF(-shape_r, shape_r), QSizeF(2 * shape_r, shape_r));
    painter->setPen(QPen(QColor(Qt::black), 0.5));
    painter->setFont(QFont("Times", 0.8 * shape_r, QFont::Bold));
    if (site_data_.func_character == CHARGE)
    {
        painter->drawText(rec, Qt::AlignCenter, "充电");
    }
    else if (site_data_.func_character == CHARHE_PREPARE)
    {
        painter->drawText(rec, Qt::AlignCenter, "准备");
    }
    else if (site_data_.func_character == STRART_OUT)
    {
        painter->drawText(rec, Qt::AlignCenter, "出库");
    }
}

QVariant ItemSite::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if ((change == QGraphicsItem::ItemPositionChange) || (change == QGraphicsItem::ItemPositionHasChanged))
    {
        QPointF newPos = value.toPointF();
        QPointF near_pos;
        // parent_edge_->getNearestPosPercent(newPos,near_pos);
        parent_edge_->flushSitesPriority();
        setPosData(newPos);
        /*QVariant & pos_value = const_cast<QVariant &>(value);
        pos_value = QVariant(newPos);*/
    }
    return QGraphicsItem::itemChange(change, value);
}

void ItemSite::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    GraphicsItemFlags itemflags = flags();
    if (((itemflags & QGraphicsItem::ItemIsMovable) >> itemflag(QGraphicsItem::ItemIsMovable)) == 1)
    {
        QPointF newPos = event->scenePos();
        adjustWithEdgeMoved(newPos);
        // graphicscene_->setAlignSite(event->scenePos());//节点自动对齐
    }
    // QGraphicsItem::mouseMoveEvent(event);
}

void ItemSite::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    aboutAttribute();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void ItemSite::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void ItemSite::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = false;
    QGraphicsItem::hoverLeaveEvent(event);
}

void ItemSite::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    emit mouseRightClicked(1);
    if (contextMenu_) {
        contextMenu_->exec(event->screenPos());
    }
}

void ItemSite::removeSiteFromEdge()
{
    if (parent_edge_) {
        parent_edge_->removeSiteFromEdge(this);
    }
}

void ItemSite::setParentEdge(ItemEdge *parentedge)
{
    parent_edge_ = parentedge;
    parentedge->addSiteToEdge(this);
    site_data_.startid = parentedge->getEdgeData().startid;
    site_data_.endid = parentedge->getEdgeData().endid;
    QPointF nearst_pos;
    site_data_.percent = parent_edge_->getNearestPosPercent(QPointF(site_data_.pos.x, site_data_.pos.y), nearst_pos);
    setPosData(nearst_pos);
    site_data_.parentedgeid = parent_edge_->getID();
    resetPos(site_data_.percent);
}

ItemEdge *ItemSite::getParentEdge()
{
    return parent_edge_;
}

unsigned int ItemSite::getSouceNodeID()
{
    return site_data_.startid;
}

unsigned int ItemSite::getDestNodeID()
{
    return site_data_.endid;
}

void ItemSite::setPercentOfEdge(double percent)
{
    site_data_.percent = percent;
}

double ItemSite::getPercentOfEdge()
{
    return site_data_.percent;
}

void ItemSite::adjustWithEdge()
{
    QPainterPath path = parent_edge_->getPath();
    QPointF pos = path.pointAtPercent(this->getPercentOfEdge());
    this->setPos(pos);
}

void ItemSite::adjustWithEdgeMoved(QPointF newPos)
{
    QPointF near_pos;
    double percent = parent_edge_->getNearestPosPercent(newPos, near_pos);
    site_data_.percent = percent;
    site_data_.pos.x = near_pos.x();
    site_data_.pos.y = near_pos.y();
    setPos(near_pos);
}

void ItemSite::resetPos(double percent)
{
    setPercentOfEdge(percent);
    QPainterPath path = parent_edge_->getPath();
    QPointF pos = path.pointAtPercent(percent);
    setPos(pos);
}

double ItemSite::getPosx()
{
    return site_data_.pos.x;
}

double ItemSite::getPosy()
{
    return site_data_.pos.y;
}

unsigned int ItemSite::getID()
{
    return site_data_.id;
}

void ItemSite::setID(unsigned int id)
{
    site_data_.id = id;
}

double ItemSite::getSitePriority()
{
    return site_data_.priority;
}

void ItemSite::setSitePriority(double pro)
{
    site_data_.priority = pro;
}

int ItemSite::getWaitTime()
{
    return site_data_.waittime;
}

void ItemSite::setWaitTime(int time)
{
    site_data_.waittime = time;
}

int ItemSite::getDirection()
{

    return site_data_.direction;
}

void ItemSite::setDirection(int dir)
{
    site_data_.direction = (SiteDir)dir;
}

int ItemSite::getFuncCharacter()
{
    return site_data_.func_character;
}

void ItemSite::setFuncCharacter(int funccharacter)
{
    site_data_.func_character = (SiteFuncCharacter)funccharacter;
}

int ItemSite::getPreciseStop()
{
    return site_data_.precisestop;
}

void ItemSite::setPreciseStop(int precisestop)
{
    site_data_.precisestop = (StopWay)precisestop;
}

double ItemSite::getAngle()
{
    QPainterPath path = parent_edge_->getPath();
    double angle = 360 - path.angleAtPercent(site_data_.percent);
    if (getDirection() == 0)
    {
        angle = angle;
    }
    else
    {
        angle = 180 + angle;
        if (angle >= 360)
        {
            angle = angle - 360;
        }
    }
    return angle;
}

int ItemSite::itemflag(GraphicsItemFlag flag)
{
    if (flag == 0)
        return 0;
    int i = 0;
    int intflag = (int)flag;
    while (true)
    {
        intflag = intflag / 2;
        if (intflag == 0)
        {
            break;
        }
        i++;
    }
    return i;
}

void ItemSite::resetParentEdge(ItemEdge *parentedge, double percent)
{
    parent_edge_->removeSiteFromEdge(this);
    parent_edge_ = parentedge;

    site_data_.parentedgeid = parent_edge_->getID();
    site_data_.startid = parent_edge_->sourceNode()->getID();
    site_data_.endid = parent_edge_->destNode()->getID();
    site_data_.percent = percent;

    QPainterPath path = parentedge->getPath();
    this->setPos(path.pointAtPercent(percent));

    parent_edge_->addSiteToEdge(this);
}

void ItemSite::comeBack()
{
    QPainterPath path = parent_edge_->getPath();
    QPointF pos = path.pointAtPercent(site_data_.percent);
    setPos(pos);
}

void ItemSite::setArrayTemp(bool flag)
{
    is_array_temp_ = flag;
    setFlag(ItemIsMovable, !flag);
    setFlag(ItemIsSelectable, !flag);
}

void ItemSite::setPosData(QPointF posf)
{
    site_data_.pos.x = posf.x();
    site_data_.pos.y = posf.y();
}
