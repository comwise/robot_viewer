#include "DialogEdge.h"
#include "ui_DialogEdge.h"
#include "ItemEdge.h"

namespace {
    static const int SensorSize = 7;
}

DialogEdge::DialogEdge(ItemEdge *edge) 
    : ui_(new Ui::DialogEdge)
    , item_edge_(edge)
{
    ui_->setupUi(this);
}

DialogEdge::~DialogEdge()
{
    delete ui_;
    ui_ = nullptr;
}

void DialogEdge::setEdgeAttribute(const EdgeData &edgedata)
{
    setWindowTitle(QString("路径(%1-%2)属性").arg(edgedata.startid).arg(edgedata.endid));
    ui_->startIdSpinBox->setValue(edgedata.startid);
    ui_->endIdSpinBox->setValue(edgedata.endid);
    ui_->weightSpinBox->setValue(CoordSystemTransf::mapLocal2World(edgedata.weight));
    ui_->directionComboBox->setCurrentIndex(edgedata.direction);
    ui_->backwardComboBox->setCurrentIndex(edgedata.backward);
    ui_->prioritySpinBox->setValue(edgedata.edgepriority);
    ui_->SZLaserSpinBox->setValue(edgedata.sz_laser_bank);
    ui_->TimLaserSpinBox->setValue(edgedata.tim310_laser_bank);
    ui_->speedSpinBox->setValue(edgedata.edge_speed);
    ui_->sensorSpinBox->setValue(edgedata.reference_mode);
    setSensorAttribute(edgedata);
}

void DialogEdge::getEdgeAttribute(EdgeData &edgedata) const
{
    edgedata.direction          = (EdgeDir)ui_->directionComboBox->currentIndex();
    edgedata.edgepriority       = ui_->prioritySpinBox->value();
    edgedata.weight             = CoordSystemTransf::mapWorld2Local(ui_->weightSpinBox->value());
    edgedata.backward           = (RobotBackable)ui_->backwardComboBox->currentIndex();
    edgedata.sz_laser_bank      = ui_->SZLaserSpinBox->value();
    edgedata.tim310_laser_bank  = ui_->TimLaserSpinBox->value();
    edgedata.edge_speed         = ui_->speedSpinBox->value();
    edgedata.reference_mode     = ui_->sensorSpinBox->value();
    getSensorAttribute(edgedata);
}

void DialogEdge::setSensorAttribute(const EdgeData &edgedata)
{
    QTableWidget *tab_list_ = ui_->sensorTableWidget;

    for (std::size_t row = 0; row < SensorSize; row++) {
        SensorConfig sc;
        std::string sensorName;
        QTableWidgetItem *item = NULL;
        if (item = tab_list_->verticalHeaderItem(row)) {
            sensorName = item->text().toStdString();
        }
        if (sensorName.empty()) {
            return;
        }

        SensorMap::const_iterator cit = edgedata.sensor_config.find(sensorName);
        if (cit == edgedata.sensor_config.end()) {
            sc.sensor_name = sensorName;
            sc.slow_distance = edgedata.global_slow;
            sc.stop_distance = edgedata.global_stop;
        } else {
            sc = cit->second;
        }

        QTableWidgetItem *task_name = new QTableWidgetItem(QString(tr("%1").arg(sc.sensor_enable)));
        tab_list_->setItem(row, EdgeColEnable, task_name);

        QTableWidgetItem *begin_pos = new QTableWidgetItem(QString(tr("%1").arg(sc.edge_enable)));
        tab_list_->setItem(row, EdgeColDirection, begin_pos);

        QTableWidgetItem *end_pos = new QTableWidgetItem(QString(tr("%1").arg(sc.slow_distance)));
        tab_list_->setItem(row, EdgeColSlow, end_pos);

        QTableWidgetItem *speed_wgt = new QTableWidgetItem(QString(tr("%1").arg(sc.stop_distance)));
        tab_list_->setItem(row, EdgeColStop, speed_wgt);
    }
}

void DialogEdge::getSensorAttribute(EdgeData &edgedata) const
{
    QTableWidget *tab_list_ = ui_->sensorTableWidget;
    std::size_t cnt = tab_list_->rowCount();
    for (std::size_t i = 0; i < cnt; i++) {
        SensorConfig sc;
        QTableWidgetItem *item = NULL;

        if (item = tab_list_->verticalHeaderItem(i)) {
            sc.sensor_name = item->text().toStdString();
        }
        if (item = tab_list_->item(i, EdgeColEnable)) {
            sc.sensor_enable = item->text().toInt() != 0;
        }
        if (item = tab_list_->item(i, EdgeColDirection)) {
            sc.edge_enable = item->text().toInt();
        }
        if (item = tab_list_->item(i, EdgeColSlow)) {
            sc.slow_distance = item->text().toDouble();
        }
        if (item = tab_list_->item(i, EdgeColStop)) {
            sc.stop_distance = item->text().toDouble();
        }
        edgedata.sensor_config[sc.sensor_name] = sc;
    }
}

void DialogEdge::on_okPushButton_clicked()
{
    if (NULL == item_edge_) {
        return;
    }

    EdgeData edgedata = item_edge_->getEdgeData();
    getEdgeAttribute(edgedata);
    item_edge_->setEdgeData(edgedata);
    item_edge_->flushSitesPriority();

    close();
}

void DialogEdge::on_cancelPushButton_clicked()
{
    close();
}
