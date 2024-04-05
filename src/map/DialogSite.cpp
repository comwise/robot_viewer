#include "DialogSite.h"
#include "ui_DialogSite.h"

DialogSite::DialogSite(QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSite)
    , pos_changed_(false)
    , posx_changed_(0)
    , posy_changed_(0)
{
    init();
}

DialogSite::DialogSite(const SiteData &area, QDialog *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSite)
    , data_(area)
    , pos_changed_(false)
    , posx_changed_(0)
    , posy_changed_(0)
{
    init();
}

DialogSite::~DialogSite()
{
    delete ui;
    ui = nullptr;
}

void DialogSite::init()
{
    ui->setupUi(this);
    setData(data_);
}

void DialogSite::setData(const SiteData &sitedata)
{
    ui->sbx_id->setValue(sitedata.id);
    ui->cbx_precise_stop->setCurrentIndex(sitedata.precisestop);
    ui->cbx_direction->setCurrentIndex(sitedata.direction);
    ui->cbx_feature->setCurrentIndex(sitedata.func_character);
    ui->sbx_pri->setValue(sitedata.priority);
    ui->sbx_wait_time->setValue(sitedata.waittime);

    ui->sbx_startid->setValue(sitedata.startid);
    ui->sbx_endid->setValue(sitedata.endid);
    ui->dsbx_posx->setValue(CoordSystemTransf::mapLocal2World(sitedata.pos.x));
    ui->dsbx_posy->setValue(CoordSystemTransf::mapLocal2World(sitedata.pos.y));
    ui->dsbx_per->setValue(sitedata.percent);

    data_ = sitedata;
}

SiteData DialogSite::getData() const
{
    return data_;
}

void DialogSite::on_btn_ok_clicked()
{
    data_.direction = (SiteDir)ui->cbx_direction->currentIndex();
    data_.precisestop = (StopWay)ui->cbx_precise_stop->currentIndex();
    data_.func_character = (SiteFuncCharacter)ui->cbx_feature->currentIndex();
    data_.priority = ui->sbx_pri->value();
    data_.waittime = ui->sbx_wait_time->value();

    if (pos_changed_) {
        QPointF pos(CoordSystemTransf::mapWorld2Local(ui->dsbx_posx->value()),
                    CoordSystemTransf::mapWorld2Local(ui->dsbx_posy->value()));
        data_.pos.x = pos.x();
        data_.pos.y = pos.y();
        pos_changed_ = false;
    }
    accept();
}

void DialogSite::on_btn_cancel_clicked()
{
    reject();
}

void DialogSite::on_dsbx_posx_valueChanged(double arg1)
{
    (void)arg1;
    if (posx_changed_ > 0) {
        pos_changed_ = true;
    }
    if (posx_changed_ < 1) {
        posx_changed_++;
    }
}

void DialogSite::on_dsbx_posy_valueChanged(double arg1)
{
    (void)arg1;
    if (posy_changed_ > 0) {
        pos_changed_ = true;
    }
    if (posy_changed_ < 1) {
        posy_changed_++;
    }
}
