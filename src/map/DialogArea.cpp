#include "DialogArea.h"
#include "ui_DialogArea.h"


DialogArea::DialogArea(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::DialogArea)
{
    init();
}

DialogArea::DialogArea(const AreaData &data, QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::DialogArea)
    , data_(data)
{
    
    init();
}

DialogArea::~DialogArea()
{
    delete ui;
    ui = nullptr;
}

void DialogArea::init()
{
    ui->setupUi(this);
    setData(data_);
}

void DialogArea::setData(const AreaData &area_data)
{
    ui->sbx_id->setValue(area_data.id);
    ui->le_desc->setText(QString().fromStdString(area_data.desc));
    ui->dsbx_cornerx->setValue(area_data.rect_data.corner_x);
    ui->dsbx_cornery->setValue(area_data.rect_data.corner_y);
    ui->dsbx_width->setValue(area_data.rect_data.width);
    ui->dsbx_height->setValue(area_data.rect_data.height);
    ui->cbx_feature->setCurrentIndex(area_data.feature);
}

AreaData DialogArea::getData() const
{
    return data_;
}

void DialogArea::on_pushButton_ok_clicked()
{
    data_.id                    = ui->sbx_id->value();
    data_.desc                  = ui->le_desc->text().toStdString();
    data_.rect_data.corner_x    = ui->dsbx_cornerx->value();
    data_.rect_data.corner_y    = ui->dsbx_cornery->value();
    data_.rect_data.width       = ui->dsbx_width->value();
    data_.rect_data.height      = ui->dsbx_height->value();
    data_.feature               = ui->cbx_feature->currentIndex();
    accept();
}

void DialogArea::on_pushButton_cancel_clicked()
{
    reject();
}
