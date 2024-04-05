#include "DockSimulator.h"
#include <QtGui>
#include <QTextCodec>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include "RobotViewerSimulator.h"

#define Pi 3.1415926

DockSimulator::DockSimulator(QWidget* widget)
  : QDialog(widget)
  , isStartSim_(false)
  , initX_(0)
  , initY_(0)
  , initA_(0)
{
    initCodec();
    initSetting();
    initForm();
    initOthers();
}

DockSimulator::~DockSimulator()
{
    writeSetting();
}

void DockSimulator::initCodec()
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
}

void DockSimulator::initSetting()
{
    readSetting();
}

void DockSimulator::initForm()
{
    QFormLayout *layout = new QFormLayout;

    leInitPos_[0] = new QLineEdit;
    leInitPos_[1] = new QLineEdit;
    leInitPos_[2] = new QLineEdit;

    btnSaveSim_ = new QPushButton(tr("保存"));
    connect(btnSaveSim_, SIGNAL(clicked()), this, SLOT(slotSaveSim()));

    btnStartSim_ = new QPushButton(tr("开始"));
    connect(btnStartSim_, SIGNAL(clicked()), this, SLOT(slotStartSim()));

    layout->addRow(tr("初始坐标:"), new QWidget);
    layout->addRow(tr("X坐标: "), leInitPos_[0]);
    layout->addRow(tr("Y坐标: "), leInitPos_[1]);
    layout->addRow(tr("角度: "),  leInitPos_[2]);
    layout->addRow(tr(" "), btnSaveSim_);
    layout->addRow(tr(" "), btnStartSim_);

    setLayout(layout);

    setWindowTitle(tr("仿真"));
}

void DockSimulator::initOthers()
{
    setInitPos(initX_, initY_, initA_);
    simulate_.reset(new RobotViewerSimulator);
    simulate_->setPeriod(simPeriod_);
    connect(this, SIGNAL(notify(const QString&)), this, SLOT(slotShowInfo(const QString&)));
    startTimer(500);
}

void DockSimulator::setInitPos(double x, double y, double angle)
{
    leInitPos_[0]->setText(QString(tr("%1")).arg(x));
    leInitPos_[1]->setText(QString(tr("%1")).arg(y));
    leInitPos_[2]->setText(QString(tr("%1")).arg(angle));
}

void DockSimulator::getInitPos()
{
    initX_ = leInitPos_[0]->text().toDouble();
    initY_ = leInitPos_[1]->text().toDouble();
    initA_ = leInitPos_[2]->text().toDouble();
}

void DockSimulator::readSetting()
{
    QSettings settings;
    initX_ = settings.value("InitX", 0).toDouble();
    initY_ = settings.value("InitY", 0).toDouble();
    initA_ = settings.value("InitTheta",0).toDouble();
    simPeriod_ = settings.value("ThreadPeriod", 20).toInt();
}

void DockSimulator::writeSetting()
{
    getInitPos();
    QSettings settings;
    settings.setValue("InitX", initX_);
    settings.setValue("InitY", initY_);
    settings.setValue("InitTheta", initA_);
}

void DockSimulator::buttonClicked(bool& is_start,QPushButton * btn)
{
    is_start = !is_start;
    btn->setText(is_start? tr("停止"): tr("开始"));
}

void DockSimulator::slotSaveSim()
{
     writeSetting();
}

void DockSimulator::slotStartSim()
{
    buttonClicked(isStartSim_, btnStartSim_);

    if(isStartSim_) {
        getInitPos();
        simulate_->setInitPos(initX_, initY_, initA_*Pi/180);
        simulate_->startTimer();
    } else {
        simulate_->stopTimer();
    }
}

void DockSimulator::slotShowInfo(const QString& info)
{

}
