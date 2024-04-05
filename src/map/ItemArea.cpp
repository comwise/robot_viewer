#include "ItemArea.h"
#include "RobotViewerScene.h"
#include <iostream>
#include "DialogArea.h"
#include <QPainter>


#define SAFE_DELETE(s, x)   \
    if(s && x) {            \
        s->removeItem(x);   \
        delete x;           \
    }                       \
    x = nullptr;

/*------------------------------------------------------------------------
*                               CornerItem
*--------------------------------------------------------------------------
*/
CornerItem::CornerItem(ItemArea *area_item)
    : area_item_(area_item)
    , set_pos_enable_(true)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(ItemCoordinateCache);
    setAcceptedMouseButtons(Qt::LeftButton);
    setParentItem(area_item);
    setZValue(-25);
}

QVariant CornerItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange || 
        change == QGraphicsItem::ItemPositionHasChanged) {
        if (set_pos_enable_ && area_item_) {
            area_item_->CornerChange(this);
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void CornerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(painter) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::green));
        painter->drawRect(boundingRect());
    }
}

QRectF CornerItem::boundingRect() const
{
    QRectF rect(-8, -8, 16, 16);
    return rect;
}

QPainterPath CornerItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void CornerItem::SetPos(const QPointF &point)
{
    set_pos_enable_ = false;
    setPos(point);
    set_pos_enable_ = true;
}

/*------------------------------------------------------------------------
*                               ItemArea
*--------------------------------------------------------------------------
*/
ItemArea::ItemArea(QPointF top_left, QPointF bottom_right, RobotViewerScene *sce)
    : ItemBase(GraphicsItemType::ITEM_ARAE)
    , top_left_(top_left)
    , bottom_right_(bottom_right)
    , scene_(sce)
{
    ItemInit();
    SetAreaData(top_left, bottom_right);
}

ItemArea::ItemArea(const AreaData &area_data, RobotViewerScene *sce)
    : ItemBase(GraphicsItemType::ITEM_ARAE)
    , scene_(sce)
{
    ItemInit();
    SetAreaData(area_data);
}

void ItemArea::ItemInit()
{
    setFlag(ItemIsSelectable, true);
    setFlag(ItemIsMovable, false);
    setCacheMode(ItemCoordinateCache);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setZValue(-25);
    //setOpacity(0.7);
}

ItemArea::~ItemArea()
{
#if 0
    SAFE_DELETE(scene(), top_left_item_);
    SAFE_DELETE(scene(), top_right_item_);
    SAFE_DELETE(scene(), bottom_left_item_);
    SAFE_DELETE(scene(), bottom_right_item_);
#endif
}

unsigned int ItemArea::getID() const
{
    return area_data_.id;
}

void ItemArea::setID(unsigned int id)
{
    area_data_.id = id;
}

void ItemArea::setDesc(const QString &desc)
{
    area_data_.desc = desc.toStdString();
}

void ItemArea::SetAreaData(const AreaData &area_data)
{
    area_data_ = area_data;

    top_left_.setX(area_data.rect_data.corner_x);
    top_left_.setY(area_data.rect_data.corner_y);
    bottom_right_.setX(area_data.rect_data.corner_x + area_data.rect_data.width);
    bottom_right_.setY(area_data.rect_data.corner_y + area_data.rect_data.height);
    //bottom_right_ = mapFromScene(bottom_right_);

    AreaUpdate();
}

void ItemArea::SetAreaData(const QPointF &top_left, const QPointF &bottom_right)
{
    QRectF rect(top_left, bottom_right);
    rect = rect.normalized();
    QPointF top = top_left;
    area_data_.rect_data.corner_x = top.x();
    area_data_.rect_data.corner_y = top.y();
    area_data_.rect_data.width = rect.width();
    area_data_.rect_data.height = rect.height();
}

AreaData ItemArea::GetAreaData()
{
    QRectF rect(top_left_, bottom_right_);
    rect = rect.normalized();
    area_data_.rect_data.corner_x = top_left_.x();
    area_data_.rect_data.corner_y = top_left_.y();
    area_data_.rect_data.width = rect.width();
    area_data_.rect_data.height = rect.height();
    return area_data_;
}

std::vector<CornerItem*> ItemArea::GetCornerItem()
{
    std::vector<CornerItem*> lst;
    lst.push_back(top_left_item_);
    lst.push_back(top_right_item_);
    lst.push_back(bottom_left_item_);
    lst.push_back(bottom_right_item_);
    return lst;
}

void ItemArea::SetTopLeft(QPointF pos)
{
    top_left_ = pos;
}

void ItemArea::SetBottomRight(QPointF pos)
{
    bottom_right_ = pos;
    prepareGeometryChange();
}

void ItemArea::AreaUpdate()
{
    QRectF rect(top_left_, bottom_right_);
    rect = rect.normalized();
    DrawCorner(rect);
}

QRectF ItemArea::boundingRect() const
{
    QRectF rect(top_left_, bottom_right_);
    return rect.normalized() | childrenBoundingRect();
}

QPainterPath ItemArea::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void ItemArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    //painter->setTransform(QTransform(QMatrix(1, 0, 0, -1, 0, 0.0)), true);

    QRectF rec = boundingRect();
    rec = rec.normalized();
    QRectF rect(top_left_, bottom_right_);
    rec = rect;
    QPen pen_frame, pen_frame_selected, pen_text, pen_text_selected;
    QBrush brush, brush_selected;

    // 绘制矩形区域
    pen_frame.setColor(QColor(0, 0, 0, 60));
    pen_frame.setWidth(10);
    pen_frame.setStyle(Qt::NoPen);
    pen_frame_selected.setColor(QColor(0, 0, 0, 120));
    pen_frame_selected.setWidth(20);
    pen_frame_selected.setStyle(Qt::NoPen);

    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(0, 0, 0, 60));
    brush_selected.setStyle(Qt::SolidPattern);
    brush_selected.setColor(QColor(0, 0, 0, 120));
    painter->setPen(pen_frame);
    painter->setBrush(brush);

    if (isSelected()) {
        painter->setPen(pen_frame_selected);
        painter->setBrush(brush_selected);
    }
    painter->drawRect(rec);

    //绘制文字说明
    bool text_turn = false;
    if (rec.width() < rec.height()) {
        //text_turn = true;
    }
    pen_text.setColor(QColor(255, 255, 255, 120));
    pen_text.setWidth(10);
    pen_text_selected.setColor(QColor(255, 255, 255, 255));
    pen_text_selected.setWidth(20);

    painter->setFont(QFont("Times", 20, QFont::Bold));
    QRectF rectf(rec.topLeft().x(), rec.topLeft().y(), rec.width(), rec.height());
    if (text_turn) {
        painter->rotate(270);
        QMatrix mat = matrix();
        mat.rotate(270);
        rectf = mat.mapRect(rectf);
    }
    
    painter->setPen(pen_text);
    if (isSelected()) {
        painter->setPen(pen_text_selected);
    }
    painter->drawText(rectf, Qt::AlignCenter, QString::fromStdString(area_data_.desc));

    if(scene_ && scene_->getInsertType() != ACTION_MOVE) {
        CornerItemShow(false);
    }
}

void ItemArea::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (scene_ && scene_->getInsertType() == ACTION_MOVE) {
        CornerItemShow(true);
    }
    QGraphicsItem::hoverEnterEvent(event);
}

void ItemArea::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    CornerItemShow(false);
    QGraphicsItem::hoverLeaveEvent(event);
}

void ItemArea::CornerItemShow(bool show)
{
    if (!top_left_item_) {
        return;
    }
    top_left_item_->setVisible(show);
    top_right_item_->setVisible(show);
    bottom_left_item_->setVisible(show);
    bottom_right_item_->setVisible(show);
    update();
}

void ItemArea::DrawCorner(QRectF &rect)
{
    if (!top_left_item_) {
        top_left_item_ = new CornerItem(this);
        top_right_item_ = new CornerItem(this);
        bottom_left_item_ = new CornerItem(this);
        bottom_right_item_ = new CornerItem(this);
    }
    rect = rect.normalized();
    top_left_ = rect.topLeft();
    QPointF top_right = rect.topRight();
    bottom_right_ = rect.bottomRight();
    QPointF bottom_left = rect.bottomLeft();
    SetAreaData(top_left_, bottom_right_);

    top_left_item_->SetType(CornerItem::TOP_LEFT);
    bottom_right_item_->SetType(CornerItem::TOP_LEFT);
    catercorner_1_items_.first = top_left_item_;
    catercorner_1_items_.second = bottom_right_item_;

    top_right_item_->SetType(CornerItem::BOTTOM_LEFT);
    bottom_left_item_->SetType(CornerItem::BOTTOM_LEFT);
    catercorner_2_items_.first = top_right_item_;
    catercorner_2_items_.second = bottom_left_item_;

    top_left_item_->SetPos(top_left_);
    top_right_item_->SetPos(top_right);
    bottom_left_item_->SetPos(bottom_left);
    bottom_right_item_->SetPos(bottom_right_);
}

void ItemArea::CornerChange(CornerItem *item)
{
    int type = item->GetType();
    switch (type)
    {
    case CornerItem::TOP_LEFT:
    {
        QPointF pos = catercorner_1_items_.first->pos();
        QPointF bottom_pos = catercorner_1_items_.second->pos();
        QPointF po1(pos.x(), bottom_pos.y());
        QPointF po2(bottom_pos.x(), pos.y());
        QRectF rect(pos, bottom_pos);
        QRectF rect1 = rect.normalized();
        top_left_ = rect1.topLeft();
        bottom_right_ = rect1.bottomRight();
        catercorner_2_items_.first->SetPos(po1);
        catercorner_2_items_.second->SetPos(po2);
        prepareGeometryChange();
        break;
    }
    case CornerItem::BOTTOM_LEFT:
    {
        QPointF pos = catercorner_2_items_.first->pos();
        QPointF bottom_pos = catercorner_2_items_.second->pos();
        QPointF po1(pos.x(), bottom_pos.y());
        QPointF po2(bottom_pos.x(), pos.y());
        QRectF rect(pos, bottom_pos);
        QRectF rect1 = rect.normalized();
        top_left_ = rect1.topLeft();
        bottom_right_ = rect1.bottomRight();
        catercorner_1_items_.first->SetPos(po1);
        catercorner_1_items_.second->SetPos(po2);
        prepareGeometryChange();
        break;
    }
    default:
        break;
    }
}

void ItemArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void ItemArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void ItemArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (scene_->getInsertType() == ACTION_MOVE) {
        QPointF pos = catercorner_1_items_.first->pos();
        QPointF bottom_pos = catercorner_1_items_.second->pos();
        top_left_ = pos;
        bottom_right_ = bottom_pos;
        prepareGeometryChange();
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void ItemArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    DialogArea dlg;
    dlg.setData(GetAreaData());
    if(dlg.exec() == QDialog::Accepted) {
        AreaData data = dlg.getData();
        SetAreaData(data);
    }
    QGraphicsItem::mouseDoubleClickEvent(event);
}
