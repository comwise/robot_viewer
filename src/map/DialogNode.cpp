#include "DialogNode.h"
#include "ui_DialogNode.h"
#include "ItemNode.h"

DialogNode::DialogNode(ItemNode *node)
    : node_item_(node)
    , ui(new Ui::DialogNode)
{
    ui->setupUi(this);
    init();
}

DialogNode::~DialogNode()
{
    delete ui;
    ui = nullptr;
}

void DialogNode::init()
{
    if(nullptr == node_item_) {
        return;
    }
    NodeData nodedata = node_item_->getNodeData();
    ui->sbx_id->setValue(nodedata.id);
    ui->dsbx_posx->setValue(CoordSystemTransf::mapLocal2World(node_item_->pos().x()));
    ui->dsbx_posy->setValue(CoordSystemTransf::mapLocal2World(node_item_->pos().y()));
    ui->cmb_spin->setCurrentIndex(nodedata.spin);
    ui->cmb_tshaped->setCurrentIndex(nodedata.tshaped);
}

void DialogNode::on_btn_ok_clicked()
{
    if(nullptr == node_item_) {
        return;
    }
    node_item_->setPos(QPointF(
        CoordSystemTransf::mapWorld2Local(ui->dsbx_posx->value()),
        CoordSystemTransf::mapWorld2Local(ui->dsbx_posy->value())));
    node_item_->setSpin(ui->cmb_spin->currentIndex());
    node_item_->setTshaped(ui->cmb_tshaped->currentIndex());
    close();
}

void DialogNode::on_btn_cancel_clicked()
{
    close();
}
