#include "DialogPolygon.h"
#include "ui_DialogPolygon.h"
#include "common/position_widget.h"
#include <QMessageBox>
#include <QDebug>


DialogPolygon::DialogPolygon(bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogPolygon)
{
    init();
    ui->pushButton_ok->setVisible(enable);
    ui->pushButton_cancel->setVisible(enable);
}

DialogPolygon::DialogPolygon(const PolygonData &data, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogPolygon)
    , data_(data)
{
    init();
}

DialogPolygon::~DialogPolygon()
{
    delete ui;
    ui = nullptr;
}

void DialogPolygon::init()
{
    ui->setupUi(this);
    setData(data_);
}

void DialogPolygon::setData(const PolygonData &data)
{
    remove();

    ui->sbx_id->setValue(data.id);
    ui->le_desc->setText(data.desc.c_str());
    ui->sbx_area_id->setValue(data.area_id);
    ui->sbx_storage_id->setValue(data.storage_id);

    QPolygonF pts = data.data;
    for(int i = 0; i < pts.size(); i++) {
        QPointF pt = pts[i];
        PositionWidget *pos = new PositionWidget(pt, this);
        if(i+1 <= ui->lyt_pos->rowCount()) {
            QLabel *lbl = new QLabel(tr("point%1").arg(i));
            ui->lyt_pos->setWidget(i, QFormLayout::LabelRole, lbl);
            ui->lyt_pos->setWidget(i, QFormLayout::FieldRole, pos);
        } else {
            ui->lyt_pos->insertRow(i, tr("point%1").arg(i), pos);
        }
    }
    data_ = data;
}

PolygonData DialogPolygon::getData() const
{
    return data_;
}

bool DialogPolygon::getData(PolygonData &data)
{
    data.id = ui->sbx_id->value();
    data.area_id = ui->sbx_area_id->value();
    data.storage_id = ui->sbx_storage_id->value();
    data.desc = ui->le_desc->text().toStdString();

    QPolygonF poly;
    int row = ui->lyt_pos->count();
    for(int i = 0; i < row; i++) {
        QPointF pt;
        QLayoutItem *item = ui->lyt_pos->itemAt(i, QFormLayout::FieldRole);
        if(item) {
            PositionWidget *wgt = dynamic_cast<PositionWidget*>(item->widget());
            if (wgt) {
                pt = wgt->getPos();
                poly << pt;
            }
        }
    }
    data.data = poly;
    return true;
}

void DialogPolygon::on_pushButton_ok_clicked()
{
    PolygonData data;
    bool ret = getData(data);
    if(data.id <= 0) {
        msgWarn("库位配置", "当前多边形id无效");
        ret = false;
    } else if(data.area_id <= 0) {
        msgWarn("库位配置", "请检查分区id");
        ret = false;
    } else if(data.storage_id <= 0) {
        msgWarn("库位配置", "请检查库位id");
        ret = false;
    } else if(data.data.isEmpty()) {
        msgWarn("库位配置", "当前多边形数据无效");
        ret = false;
    }
    if(ret) {
        data_ = data;
        accept();
    } else {
        reject();
    }
}

void DialogPolygon::on_pushButton_cancel_clicked()
{
    reject();
}

void DialogPolygon::remove()
{
    QFormLayout *lyt = ui->lyt_pos;
    QLayoutItem *item{nullptr};
    while (lyt->count() > 0 && (item = lyt->takeAt(0))!= nullptr) {
        QWidget *wgt = item->widget();
        if(wgt) {
            lyt->removeWidget(wgt);
            wgt->setParent(nullptr);
            delete wgt;
            wgt = nullptr;
        }
        lyt->removeItem(item);
        delete item;
        item = nullptr;
    }
#if 0
    QList<PositionWidget*> pt_items = lyt->findChildren<PositionWidget*>();
    foreach(PositionWidget *item, pt_items) {
        delete item;
        item = nullptr;
    }
    QList<QLabel*> lb_items = lyt->findChildren<QLabel*>();
    foreach(QLabel *item, lb_items) {
        delete item;
        item = nullptr;
    }
#endif
}

void DialogPolygon::msgWarn(const QString &title, const QString &info)
{
    QMessageBox::warning(this, title, info);
}
