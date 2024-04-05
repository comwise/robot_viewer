#include "ItemReflector.h"
#include <QPainter>
#include <QPen>
#include <QRadialGradient>

ItemReflector::ItemReflector(const QString &desc)
    : ItemBase(GraphicsItemType::ITEM_REFLECTOR)
    , desc_(desc)
    , shape_r_(25)
    , detect_(false)
{
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-20);
}

void ItemReflector::setDetect(int detect)
{
    detect_ = detect;
}

QRectF ItemReflector::boundingRect() const
{
    QRectF rect(-2 * shape_r_, -2 * shape_r_, 4 * shape_r_, 4 * shape_r_);
    return rect;
}

void ItemReflector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect(-shape_r_, -shape_r_, 2 * shape_r_, 2 * shape_r_);
    painter->setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    switch (detect_)
    {
    case 1:
    {
        painter->setBrush(QBrush(Qt::blue));
        break;
    }
    case 2:
    {
        painter->setBrush(QBrush(Qt::red));
        break;
    }
    case 3:
    {
        painter->setBrush(QBrush(Qt::cyan));
        break;
    }
    case 4:
    {
        painter->setBrush(QBrush(Qt::darkMagenta));
        break;
    }
    default:
    {
        break;
    }
    }
    painter->drawEllipse(rect);

    painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);
    painter->setFont(QFont("Times", shape_r_ / 3, QFont::Bold));
    painter->setPen(Qt::black);
    painter->drawText(rect, Qt::AlignCenter, desc_);
}

QPainterPath ItemReflector::shape() const
{
    QPainterPath path;
    path.addRect(-shape_r_, -shape_r_, 2 * shape_r_, 2 * shape_r_);
    return path;
}
