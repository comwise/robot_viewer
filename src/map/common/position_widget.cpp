#include "position_widget.h"
#include "ui_position_widget.h"

PositionWidget::PositionWidget(const QPointF &pt, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PositionWidget)
{
    ui->setupUi(this);
    setPos(pt);
}

PositionWidget::~PositionWidget()
{
    delete ui;
}

void PositionWidget::setPos(const QPointF &pt)
{
    ui->dsbx_x->setValue(pt.x());
    ui->dsbx_y->setValue(pt.y());
}

QPointF PositionWidget::getPos()
{
    QPointF pt;
    pt.setX(ui->dsbx_x->value());
    pt.setY(ui->dsbx_y->value());
    return pt;
}
