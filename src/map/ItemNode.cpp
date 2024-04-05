#include "ItemNode.h"
#include "ItemEdge.h"
#include "DialogNode.h"
#include <QPainter>
#include <QPen>
#include <QRadialGradient>

namespace {
const double g_node_size = 50;
}

ItemNode::ItemNode()
    : ItemBase(GraphicsItemType::ITEM_NODE)
    , scale_factor_(1.0/0.3)
    , hover_enter_(false)
    , align_(false)
    , pressed_(false)
{
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsSelectable, true);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsFocusable, true);
    setCacheMode(DeviceCoordinateCache);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setZValue(-1);
}

ItemNode::~ItemNode()
{
    
}

QRectF ItemNode::boundingRect() const
{
    double shaper = 2 * g_node_size / scale_factor_;
    double adjust = 3.0 / scale_factor_;
    return QRectF(-2.0 * shaper - adjust, -2.0 * shaper - adjust, 4.0 * (shaper + adjust), 4.0 * (shaper + adjust));
}

QPainterPath ItemNode::shape() const
{
    double shaper = 0;
    if (hover_enter_)
    {
        shaper = 2 * g_node_size / scale_factor_;
    }
    else
    {
        shaper = g_node_size / scale_factor_;
    }

    QPainterPath path;
    path.addRect(-shaper, -shaper, 2.0 * shaper, 2.0 * shaper);
    return path;
}

void ItemNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    scale_factor_ = scale();
    painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
    drawCommonNode(painter);
}

void ItemNode::drawCommonNode(QPainter *painter)
{
    QColor main_color(Qt::yellow);
    if (hover_enter_)
    {
        shape_r = 2 * g_node_size / scale_factor_;
    }
    else
    {
        shape_r = g_node_size / scale_factor_;
    }

    if (NO_SPIN == node_data_.spin)
    {
        main_color = QColor(Qt::yellow);
    }
    else
    {
        main_color = QColor(Qt::magenta);
    }

    QPen pen;
    QRadialGradient gradient(-3 / scale_factor_, -3 / scale_factor_, 10 / scale_factor_);
    if (!isSelected())
    {
        gradient.setColorAt(0, main_color);
        gradient.setColorAt(1, main_color);
        pen = QPen(Qt::black, 0);
    }
    else
    {
        gradient.setColorAt(1, QColor(main_color).light(120));
        gradient.setColorAt(0, QColor(main_color).light(120));
        pen = QPen(Qt::red, 0.5 / scale_factor_);
    }
    painter->setBrush(gradient);
    painter->setPen(pen);
    painter->drawRect(QRectF(-shape_r, -shape_r, 2.0 * shape_r, 2.0 * shape_r));
    if (align_ && pressed_)
    {
        painter->drawLine(QLineF(QPointF(-2 * shape_r, 0), QPointF(-0.5 * shape_r, 0)));
        painter->drawLine(QLineF(QPointF(0.5 * shape_r, 0), QPointF(2 * shape_r, 0)));
        painter->drawLine(QLineF(QPointF(0, -2 * shape_r), QPointF(0, -0.5 * shape_r)));
        painter->drawLine(QLineF(QPointF(0, 0.5 * shape_r), QPointF(0, 2 * shape_r)));
    }

    //绘制文字
    QRectF rect = QRectF(QPointF(-shape_r, -shape_r), QSizeF(2.0 * shape_r, 2.0 * shape_r));
    QFont font("Times");
    font.setPointSizeF(shape_r * 0.7);
    painter->setFont(font);
    QString draw_text;
    draw_text.setNum(node_data_.id);
    if (TSHAPED == node_data_.tshaped)
        draw_text += "T";
    painter->drawText(rect, Qt::AlignCenter, draw_text);
}

void ItemNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed_ = true;
    QGraphicsItem::mousePressEvent(event);
}

void ItemNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    pressed_ = false;
    align_ = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

void ItemNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void ItemNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    DialogNode dialog(this);
    dialog.exec();
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void ItemNode::addEdge(ItemEdge *edge)
{
    int index = edgeList_.indexOf(edge);
    if (index == -1)
        edgeList_ << edge;
}

void ItemNode::removeEdge(ItemEdge *edge)
{
    int index = edgeList_.indexOf(edge);
    if (index != -1)
        edgeList_.removeAt(index);
}

QList<ItemEdge *> ItemNode::getEdges() const
{
    return edgeList_;
}

double ItemNode::getPosx() const
{
    return node_data_.pos.x;
}

double ItemNode::getPosy() const
{
    return node_data_.pos.y;
}

unsigned int ItemNode::getID() const
{
    return node_data_.id;
}

void ItemNode::setID(unsigned int id)
{
    node_data_.id = id;
}

QVariant ItemNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if ((change == QGraphicsItem::ItemPositionChange) || (change == QGraphicsItem::ItemPositionHasChanged))
    {
        align_ = true;
        foreach (ItemEdge *edge, edgeList_)
        {
            if(edge) {
                edge->adjust();
                edge->intWeight();
            }
        }
        QPointF newpos = value.toPointF();
        setPosData(newpos);
    }
    return QGraphicsItem::itemChange(change, value);
}

void ItemNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = true;
    update();
}

void ItemNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hover_enter_ = false;
    update();
}

void ItemNode::setPosData(const QPointF &pos)
{
    node_data_.pos.x = pos.x();
    node_data_.pos.y = pos.y();
}

void ItemNode::setSpin(int val)
{
    node_data_.spin = (Spin)val;
}

void ItemNode::setTshaped(int val)
{
    node_data_.tshaped = (Tshaped)val;
}

NodeData &ItemNode::getNodeData()
{
    return node_data_;
}

void ItemNode::setNodeData(const NodeData &node_data)
{
    node_data_ = node_data;
}
