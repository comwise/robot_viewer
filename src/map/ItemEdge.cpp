#include "ItemEdge.h"
#include "ItemNode.h"
#include "ItemControlPoint.h"
#include "ItemSite.h"
#include "DialogEdge.h"
#include <QPainter>
#include <QPen>

const double g_edge_size = 25;

ItemEdge::ItemEdge(ItemNode *sourceNode, ItemNode *destNode)
    : ItemBase(GraphicsItemType::ITEM_EDGE)
    , source_node_(sourceNode)
    , dest_node_(destNode)
    , scale_factor_(1.0/0.3)
{
    setFlag(ItemIsSelectable, true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    // this->setCacheMode(ItemCoordinateCache);
    setZValue(-10);

    hover_enter_ = false;
    source_node_ = sourceNode;
    dest_node_ = destNode;
    source_node_->addEdge(this);
    dest_node_->addEdge(this);
    edge_data_.startid = source_node_->getID();
    edge_data_.endid = dest_node_->getID();

    QPointF fstctrlpt_pos = initfirstCtrlPtPos(sourceNode, destNode);
    QPointF secctrlpt_pos = initsecCtrlPtPos(sourceNode, destNode);
    initCtrlPt(fstctrlpt_pos, secctrlpt_pos);
    adjust();
}

ItemEdge::~ItemEdge()
{
    
}

EdgeData &ItemEdge::getEdgeData()
{
    return edge_data_;
}

void ItemEdge::setEdgeData(const EdgeData &edgedata)
{
    edge_data_ = edgedata;
}

QRectF ItemEdge::boundingRect() const
{
    if (!source_node_ || !dest_node_)
        return QRectF();

    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);

    QPainterPathStroker stroker;
    stroker.setWidth(7.5 * g_edge_size / scale_factor_);
    path = stroker.createStroke(path);
    return (path.controlPointRect() | childrenBoundingRect());
}

QPainterPath ItemEdge::shape() const
{
    if (isSelected())
    {
        QPainterPath path;
        path.moveTo(sourcenode_pos_);
        path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);

        path.addPath(fst_ctrlpt_->mapToScene(fst_ctrlpt_->shape()));
        path.addPath(sec_ctrlpt_->mapToScene(sec_ctrlpt_->shape()));

        QPainterPathStroker stroker;
        stroker.setWidth(g_edge_size / scale_factor_);
        if (hover_enter_)
            stroker.setWidth(1.5 * g_edge_size / scale_factor_);
        return stroker.createStroke(path);
    }
    else
    {
        QPainterPath path;
        path.moveTo(sourcenode_pos_);
        path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);

        QPainterPathStroker stroker;
        stroker.setWidth(g_edge_size / scale_factor_);
        if (hover_enter_)
            stroker.setWidth(1.5 * g_edge_size / scale_factor_);
        return stroker.createStroke(path);
    }
}

void ItemEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    scale_factor_ = scale();
    drawEdgeCommon(painter);
}

void ItemEdge::adjust()
{
    if (!source_node_ || !dest_node_)
        return;

    QLineF line(mapFromItem(source_node_, 0, 0), mapFromItem(dest_node_, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();
    QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);

    if (length > qreal(20))
    {
        sourcenode_pos_ = mapFromItem(source_node_, 0, 0);
        destnode_pos_ = mapFromItem(dest_node_, 0, 0);
        fstctrlpt_pos_ = mapFromItem(fst_ctrlpt_, 0, 0);
        secctrlpt_pos_ = mapFromItem(sec_ctrlpt_, 0, 0);
        edge_data_.ctrlpos1.x = mapToScene(fstctrlpt_pos_).x();
        edge_data_.ctrlpos1.y = mapToScene(fstctrlpt_pos_).y();
        edge_data_.ctrlpos2.x = mapToScene(secctrlpt_pos_).x();
        edge_data_.ctrlpos2.y = mapToScene(secctrlpt_pos_).y();
    }
    else
    {
        sourcenode_pos_ = destnode_pos_ = line.p1();
    }

    if (!sitelist_.empty())
    {
        foreach (ItemSite *site, sitelist_)
        {
            site->adjustWithEdge();
        }
    }
}

void ItemEdge::drawEdgeCommon(QPainter *painter)
{
    if (!source_node_ || !dest_node_)
        return;

    QLineF line(sourcenode_pos_, destnode_pos_); //判断这条线的长度，是否画线
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);

    QPainterPathStroker stroker;
    stroker.setWidth(g_edge_size / scale_factor_);
    if (hover_enter_)
        stroker.setWidth(1.5 * g_edge_size / scale_factor_);

    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    path = stroker.createStroke(path);

    painter->setPen(Qt::NoPen);
    if (isSelected())
    {
        //画线
        painter->setPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        if (REVERSE_ALLOWED == edge_data_.backward)
        {
            painter->setBrush(QColor(255, 0, 100, 255));
        }
        else
        {
            painter->setBrush(QColor(0, 68, 192, 255));
        }
    }
    else
    {
        //画线
        if (REVERSE_ALLOWED == edge_data_.backward)
        {
            painter->setBrush(QColor(255, 100, 255, 255));
        }
        else
        {
            painter->setBrush(QColor(0, 255, 255, 255));
        }
    }
    if (edge_data_.sz_laser_bank == -1 || edge_data_.tim310_laser_bank == -1)
    {
        painter->setBrush(QBrush(Qt::yellow));
    }
    painter->drawPath(path);
    drawEdgeArrow(painter);
    // drawTangentLine(painter);
}

void ItemEdge::drawEdgeArrow(QPainter *painter)
{
    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);
    int length = path.length() / 40;

    switch (edge_data_.direction)
    {
    case POSITIVE_WAY:
    {
        QPointF arrowendnear = path.pointAtPercent(0.95);
        QPointF arrowend = path.pointAtPercent(1);
        QPolygonF polygon;
        calculateArrow(polygon, arrowendnear, arrowend);
        // painter->setPen(QPen(Qt::yellow));
        painter->setBrush(QColor(Qt::blue));
        painter->drawPolygon(polygon);
        break;
    }

    case NEGTIVE_WAY:
    {
        QPointF arrowendnear = path.pointAtPercent(0.05);
        QPointF arrowend = path.pointAtPercent(0);
        QPolygonF polygon;
        calculateArrow(polygon, arrowendnear, arrowend);
        // painter->setPen(QPen(Qt::yellow));
        painter->setBrush(QColor(Qt::blue));
        painter->drawPolygon(polygon);
        break;
    }

    case BOTH_WAY:
    {
        QPointF arrowendnear = path.pointAtPercent(0.95);
        QPointF arrowend = path.pointAtPercent(1);
        QPolygonF polygon;
        calculateArrow(polygon, arrowendnear, arrowend);
        // painter->setPen(QPen(Qt::yellow));
        painter->setBrush(QColor(Qt::blue));
        painter->drawPolygon(polygon);

        QPolygonF polygon1;
        arrowendnear = path.pointAtPercent(0.05);
        arrowend = path.pointAtPercent(0);
        calculateArrow(polygon1, arrowendnear, arrowend);
        // painter->setPen(QPen(Qt::yellow));
        painter->setBrush(QColor(Qt::blue));
        painter->drawPolygon(polygon1);
        break;
    }
    default:
        break;
    }
}

void ItemEdge::calculateArrow(QPolygonF &polygon, QPointF &arrowendnear, QPointF &arrowend)
{
    double pathlength = 2.5 * g_edge_size / scale_factor_;

    if (hover_enter_)
        pathlength = 1.5 * pathlength;

    QLineF dir_line(arrowendnear, arrowend);
    double dir_angle = (360 - dir_line.angle()) * M_PI / 180;

    double k = tan(dir_angle);
    double nk = -1 / k;
    double normal_agl = atan(nk);

#if 1
    QPointF ar0 = arrowend - QPointF(pathlength * cos(dir_angle), pathlength * sin(dir_angle));
    QPointF ar1 = ar0 + QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
    QPointF ar2 = ar0 - QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
    polygon << arrowend << ar1 << ar2;

#else
    QPointF ar0 = arrowend - 1.5 * QPointF(pathlength * cos(dir_angle), pathlength * sin(dir_angle));
    QPointF ar1 = ar0 + QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
    QPointF ar2 = ar0 - QPointF(pathlength * cos(normal_agl), pathlength * sin(normal_agl));
    QPointF ar3 = arrowend - QPointF(pathlength * cos(dir_angle), pathlength * sin(dir_angle));
    polygon << arrowend << ar1 << ar3 << ar2;
#endif
}

void ItemEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void ItemEdge::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void ItemEdge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void ItemEdge::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    DialogEdge edge_dlg(this);
    edge_dlg.setEdgeAttribute(edge_data_);
    edge_dlg.exec();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void ItemEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem::contextMenuEvent(event);
}

QPointF ItemEdge::initfirstCtrlPtPos(ItemNode *srcNode, ItemNode *desNode)
{
    QPointF srcPos, desPos;
    if(srcNode && desNode) {
        srcPos = srcNode->scenePos();
        desPos = desNode->scenePos();
    }
    return ((desPos - srcPos) / 3 + srcPos);
}

QPointF ItemEdge::initsecCtrlPtPos(ItemNode *srcNode, ItemNode *desNode)
{
    QPointF srcPos, desPos;
    if(srcNode && desNode) {
        srcPos = srcNode->scenePos();
        desPos = desNode->scenePos();
    }
    return ((desPos - srcPos) * 2 / 3 + srcPos);
}

void ItemEdge::initCtrlPt(QPointF fstPos, QPointF secPos)
{
    fst_ctrlpt_ = new ItemControlPoint(this, source_node_);
    sec_ctrlpt_ = new ItemControlPoint(this, dest_node_);
    if (!fst_ctrlpt_ || !sec_ctrlpt_) {
        return;
    }

    fst_ctrlpt_->setVisible(false);
    sec_ctrlpt_->setVisible(false);

    edge_data_.ctrlpos1.x = fstPos.x();
    edge_data_.ctrlpos1.x = fstPos.y();
    edge_data_.ctrlpos2.x = secPos.x();
    edge_data_.ctrlpos2.y = secPos.y();
    fst_ctrlpt_->setPos(fstPos);
    sec_ctrlpt_->setPos(secPos);
}

//////////////////////////////////////////////////////////////////////////

ItemNode *ItemEdge::sourceNode() const
{
    return source_node_;
}

ItemNode *ItemEdge::destNode() const
{
    return dest_node_;
}

ItemControlPoint *ItemEdge::getFirstControlPoint()
{
    return fst_ctrlpt_;
}

ItemControlPoint *ItemEdge::getSecControlPoint()
{
    return sec_ctrlpt_;
}

unsigned int ItemEdge::getID() const
{
    return edge_data_.id;
}

void ItemEdge::setID(unsigned int id)
{
    edge_data_.id = id;
}

void ItemEdge::intWeight()
{
    edge_data_.weight = getPath().length();
}

double ItemEdge::getWeight()
{
    return edge_data_.weight;
}

void ItemEdge::setWeight(double weight)
{
    edge_data_.weight = weight;
}

int ItemEdge::getBackwardable()
{
    return edge_data_.backward;
}

void ItemEdge::setBackwardable(int backwardable)
{
    edge_data_.backward = (RobotBackable)backwardable;
}

int ItemEdge::getDirection()
{
    return edge_data_.direction;
}

void ItemEdge::setDirection(int direction)
{
    edge_data_.direction = (EdgeDir)direction;
}

int ItemEdge::getEdgePriority()
{
    return edge_data_.edgepriority;
}

void ItemEdge::setEdgePriority(int pri)
{
    edge_data_.edgepriority = pri;
}

void ItemEdge::setFirstCtrlPtPos(QPointF pos)
{
    if (fst_ctrlpt_) {
        fst_ctrlpt_->setPos(pos);
    }
}

void ItemEdge::setSecCtrlPtPos(QPointF pos)
{
    if (sec_ctrlpt_) {
        sec_ctrlpt_->setPos(pos);
    }
}

void ItemEdge::showControlPoint(void)
{
    if (fst_ctrlpt_ && sec_ctrlpt_) {
        fst_ctrlpt_->setVisible(true);
        sec_ctrlpt_->setVisible(true);
    }
}

void ItemEdge::selectControlPoint()
{
    if (fst_ctrlpt_ && sec_ctrlpt_) {
        fst_ctrlpt_->setSelected(true);
        sec_ctrlpt_->setSelected(true);
    }
}

void ItemEdge::hideControlPoint(void)
{
    if (fst_ctrlpt_ && sec_ctrlpt_) {
        fst_ctrlpt_->setVisible(false);
        sec_ctrlpt_->setVisible(false);
    }
}

//删除一条边，需要把它从点的列表中删除
void ItemEdge::removeFromNode()
{
    if (source_node_ && dest_node_) {
        source_node_->removeEdge(this);
        dest_node_->removeEdge(this);
    }
}

//以下是关于任务点的操作
//将一个任务点加入属于该边的任务点列表
void ItemEdge::addSiteToEdge(ItemSite *site)
{
    int index = sitelist_.indexOf(site);
    if (index == -1) {
        sitelist_ << site;
    }
}

void ItemEdge::removeSiteFromEdge(ItemSite *site)
{
    int index = sitelist_.indexOf(site);
    if (index != -1) {
        sitelist_.removeAt(index);
    }
}

QList<ItemSite *> ItemEdge::getSiteFromEdge() const
{
    return sitelist_;
}

QPainterPath ItemEdge::getPath()
{
    QPainterPath path;
    path.moveTo(sourcenode_pos_);
    path.cubicTo(fstctrlpt_pos_, secctrlpt_pos_, destnode_pos_);
    return path;
}

QList<ItemSite *> ItemEdge::sortSitesByPercent()
{
    QList<ItemSite *> sitelist = sitelist_;
    ItemSite *tempsite = NULL;
    for (int i = 0; i < sitelist.size(); i++)
    {
        for (int j = 0; j < sitelist.size() - i; j++)
        {
            if (sitelist[j]->getPercentOfEdge() > sitelist[j + 1]->getPercentOfEdge())
            {
                tempsite = sitelist[j];
                sitelist[j] = sitelist[j + 1];
                sitelist[j + 1] = tempsite;
                tempsite = NULL;
            }
        }
    }
    return sitelist;
}

void ItemEdge::flushSitesPriority()
{
    if (edge_data_.edgepriority == 0)
        return;

    foreach (ItemSite *site, sitelist_)
    {
        if(nullptr == site) {
            continue;
        }

        double per = site->getPercentOfEdge();
        if (per < 0.0001) {
            per = 0.0001;
        }
        if ((SiteDir)(site->getDirection()) == POSITIVE_DIR) {
            site->setSitePriority(edge_data_.edgepriority + per / 2);
        } else {
            site->setSitePriority(edge_data_.edgepriority + 1 - per / 2);
        }
        // site->update();
    }
}

double ItemEdge::getEdgeAngle()
{
    QPointF s, e;
    if (source_node_ && dest_node_) {
        QPointF s = source_node_->scenePos();
        QPointF e = dest_node_->scenePos();
    }
    QLineF line(s, e);
    // VecPosition s1(s.x(), s.y());
    // VecPosition e1(e.x(), e.y());
    // VecPosition p = e1-s1;

    return line.angle();
}

void ItemEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = true;
    update();
}

void ItemEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = false;
    update();
}

double ItemEdge::getEdgeSpeed()
{
    return edge_data_.edge_speed;
}

void ItemEdge::setEdgeSpeed(double speed)
{
    edge_data_.edge_speed = speed;
}

double ItemEdge::getNearestPosPercent(const QPointF &pos, QPointF &nearest_pos)
{
    QPainterPath path = getPath();
    double percent = 0.001;
    double dis = 9999999999;
    double nearest_percent = 0.0;
    for (percent = 0; percent <= 1; percent += 0.001) {
        QPointF percent_pos = path.pointAtPercent(percent);
        QLineF line(pos, percent_pos);
        if (line.length() < dis) {
            nearest_percent = percent;
            nearest_pos = percent_pos;
            dis = line.length();
        }
    }
    return nearest_percent;
}

QPointF ItemEdge::getControlPoint(double percent,
        std::pair<QPointF, QPointF> &first,
        std::pair<QPointF, QPointF> &second)
{
    QPainterPath path = getPath();
    QPointF percent_pos = path.pointAtPercent(percent);
    QPainterPath path2;
    path2.moveTo(destnode_pos_);
    path2.cubicTo(secctrlpt_pos_, fstctrlpt_pos_, sourcenode_pos_);
    QPointF Q4 = path2.pointAtPercent((1.0 - percent));
    QPointF Q0 = percent_pos;
    QPointF percent_pos1 = path.pointAtPercent(percent / 2.0);
    QPointF fir, sec;
    getControlPoint(1.0 - percent, destnode_pos_, secctrlpt_pos_, fstctrlpt_pos_, 
        sourcenode_pos_, Q0, percent_pos1, sourcenode_pos_, fir, sec);
    first.first = fir;
    first.second = sec;
    Q0 = percent_pos;
    percent_pos1 = path.pointAtPercent((1.0 + percent) / 2.0);
    getControlPoint(percent, sourcenode_pos_, fstctrlpt_pos_, secctrlpt_pos_, 
        destnode_pos_, Q0, percent_pos1, destnode_pos_, fir, sec);
    second.first = fir;
    second.second = sec;
    return percent_pos;
}

void ItemEdge::getControlPoint(double a, 
    QPointF P0, QPointF P1, QPointF P2, QPointF P3, 
    QPointF Q0, QPointF Qa, QPointF Q3, 
    QPointF &first, QPointF &second)
{
    QPointF m = P3 - 3 * P2 + 3 * P1 - P0;
    QPointF n = P0 - 2 * P1 + P2;
    QPointF l = P1 - P0;
    QPointF l1 = a * a * m + 2 * a * n + l;
    QPointF ll = P3 - P2;

    if (fabs(l1.x()) < 0.000001)
    {
        first = (Q3 - Q0) / 3 + Q0;
        second = 2 * (Q3 - Q0) / 3 + Q0;
        return;
    }
    double ka = l1.y() / l1.x();
    double k1 = ll.y() / ll.x();
    if (fabs(k1 - ka) < 0.000001)
    {
        first = (Q3 - Q0) / 3 + Q0;
        second = 2 * (Q3 - Q0) / 3 + Q0;
        return;
    }
    QPointF Q1, Q2;
    double x = (4 * Q3.y() + 4 * Q0.y() + 8 * k1 * Qa.x() - 8 * Qa.y() - 4 * k1 * Q3.x() - (k1 + 3 * ka) * Q0.x()) / (3 * k1 - 3 * ka);
    Q1.setX(x);
    Q1.setY((Q0.y() + ka * (Q1.x() - Q0.x())));
    double x2 = (8 * Qa.x() - Q3.x() - 3 * Q1.x() - Q0.x()) / 3;
    Q2.setX(x2);
    Q2.setY(Q3.y() - k1 * (Q3.x() - x2));
    first = Q1;
    second = Q2;
}

void ItemEdge::setControlPointFlag(QGraphicsItem::GraphicsItemFlag flag, bool enable)
{
    if (fst_ctrlpt_ && sec_ctrlpt_) {
        fst_ctrlpt_->setFlag(flag, enable);
        sec_ctrlpt_->setFlag(flag, enable);
    }
}

void ItemEdge::setControlPointSelect(bool select)
{
    if (fst_ctrlpt_ && sec_ctrlpt_) {
        fst_ctrlpt_->setSelected(select);
        sec_ctrlpt_->setSelected(select);
    }
}

void ItemEdge::removeSites()
{
    sitelist_.clear();
}

QPointF ItemEdge::getFirstCtrlPtPos()
{
    return fstctrlpt_pos_;
}

QPointF ItemEdge::getSecCtrlPtPos()
{
    return secctrlpt_pos_;
}

void ItemEdge::setDefaultBank(int tim_bank, int sz_bank)
{
    edge_data_.tim310_laser_bank = tim_bank;
    edge_data_.sz_laser_bank = sz_bank;
}

void ItemEdge::setDefaultSensor(double slow, double stop)
{
    if (edge_data_.global_slow == 0) {
        edge_data_.global_slow = slow;
    }

    if (edge_data_.global_stop == 0) {
        edge_data_.global_stop = stop;
    }
}
