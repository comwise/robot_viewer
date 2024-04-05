#include <QtGui/QPainter>
#include <QtGui/QPen>
#include "ItemMap.h"
#include "RobotViewerScene.h"
#include <QMatrix>
#include <QDebug>

namespace {
    static const int kSizeScale = 1;
}

ItemMap::ItemMap(const MapData &map_data, RobotViewerScene *sce)
    : ItemBase(GraphicsItemType::ITEM_MAP)
    , map_data_(map_data)
    , scene_(sce)
    , show_image_(nullptr)
    , show_grid_(false)
{
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    setAcceptDrops(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setAcceptedMouseButtons(Qt::NoButton);
    setZValue(-30);

    flush();
}

ItemMap::~ItemMap()
{
    if (nullptr != show_image_) {
        delete show_image_;
    }
    show_image_ = nullptr;
}

void ItemMap::flush()
{
    std::string img_file = map_data_.file_;
    show_image_ = new QImage();
    if(!show_image_->load(img_file.c_str())) {
        qDebug() << "load image file failed, file = " << img_file.c_str();
        return;
    }
    //show_image_ = new QImage(show_image_->mirrored(false, true));
    size_w_ = show_image_->width();
    size_h_ = show_image_->height();
}

void ItemMap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //painter->setTransform(QTransform(QMatrix(1.0, 0.0, 0.0, -1.0, 0, 0)), true);
    if (painter && show_image_) {
        painter->drawImage(QRectF(0, 0,
            size_w_*kSizeScale, size_h_*kSizeScale), *show_image_);
    }

    if (painter && show_grid_) {
        painter->setPen(QPen(Qt::gray, 0));
        painter->drawLines(grid_lines_);
    }
}

QRectF ItemMap::boundingRect() const
{
    return QRectF(0, 0, size_w_*kSizeScale, size_h_*kSizeScale);
}

QPainterPath ItemMap::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void ItemMap::showGrid(bool flag)
{
    show_grid_ = flag;
    update();
}
