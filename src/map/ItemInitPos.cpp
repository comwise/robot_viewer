#include "ItemInitPos.h"
#include <QImage>
#include <QtGui>

//#define DrawImage

ItemInitPos::ItemInitPos(QGraphicsScene *sce)
  : ItemBase(GraphicsItemType::ITEM_INITPOS)
  , init_image_(NULL)
  , scene_(sce)
{
    setCacheMode(DeviceCoordinateCache);
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(-4);

#ifdef DrawImage
    init();
#endif
}

ItemInitPos::~ItemInitPos()
{
    uninit();
}

void ItemInitPos::init()
{
    init_image_ = new QImage(2*kInitWidth, 2*kInitHeight, QImage::Format_ARGB32);
    init_image_->fill(Qt::transparent);
    QPainter painter;
    painter.begin(init_image_);
    painter.setPen(Qt::red);
    //painter.setBrush(Qt::red);
    //painter.setBrushOrigin(0, 0);
    painter.drawPoint(0, 0);
    painter.drawEllipse(-10, -10, 20, 20);
    painter.drawLine(0, 0, kInitWidth, 0);
    painter.drawLine(0, 0, 0, kInitHeight);
    painter.drawPolygon(QPolygonF()<<QPointF(kInitWidth*(1-5/100), kInitWidth*5/100)<<QPointF(kInitWidth*(1-5/100), -kInitWidth*5/100)<<QPointF(kInitWidth, 0));
    painter.drawPolygon(QPolygonF()<<QPointF(kInitHeight*5/100, kInitHeight*(1-5/100))<<QPointF (-kInitHeight*5/100, kInitHeight*(1-5/100))<<QPointF(0, kInitHeight));
    painter.drawText(-50, -50, 100, 50, Qt::AlignHCenter, "(0 0)");
    painter.end();
}

void ItemInitPos::uninit()
{
    if (init_image_) {
        delete init_image_;
        init_image_ = NULL;
    }
}

QRectF ItemInitPos::boundingRect() const
{
    QRectF rect(-kInitWidth, -kInitHeight, 2*kInitWidth, 2*kInitHeight);
    return rect;
}

QPainterPath ItemInitPos::shape() const
{
    QPainterPath path;
    path.addRect(-kInitWidth, -kInitHeight, 2*kInitWidth, 2*kInitHeight);
    return path;
}

void ItemInitPos::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (painter) {
#ifndef DrawImage
        painter->setPen(Qt::red);
        painter->setBrush(Qt::red);
        painter->drawEllipse(-10, -10, 20, 20);
        painter->drawLine(0, 0, kInitWidth, 0);
        painter->drawLine(0, 0, 0, kInitHeight);
        painter->drawPolygon(QPolygonF()<<QPointF(kInitWidth-10, 10)<<QPointF(kInitWidth-10, -10)<<QPointF(kInitWidth, 0));
        painter->drawPolygon(QPolygonF()<<QPointF(10, kInitHeight-10)<<QPointF (-10, kInitHeight-10)<<QPointF(0, kInitHeight));
        painter->drawText(-50, -50, 100, 50, Qt::AlignHCenter, "(0 0)");
#else
        QRectF rect(-kInitWidth, -kInitHeight, 2*kInitWidth, 2*kInitHeight);
        if (init_image_ && painter)
            painter->setTransform(QTransform(QMatrix(1, 0, 0, 1, 0, 0)), true);
            painter->drawImage(rect, *init_image_);
#endif
    }
}
