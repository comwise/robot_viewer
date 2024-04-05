#include "RobotViewerSimulator.h"
#include <thread>
#include <math.h>
#include "common/count_time.hpp"

#define PI 3.141592654


RobotViewerSimulator::RobotViewerSimulator()
  : chassis_period_(100)
  , is_start_(false)
  , vx_(0)
  , vw_(0)
  , vtheta_(0)
{
    init();
}

RobotViewerSimulator::~RobotViewerSimulator()
{
    deinit();
}

void RobotViewerSimulator::init()
{
    is_loop_ = true;
    thr_ = std::make_shared<std::thread>(
        std::bind(&RobotViewerSimulator::timerThread, this));
}

void RobotViewerSimulator::deinit()
{
    stopTimer();
    if(thr_ && thr_->joinable()) {
        thr_->join();
    }
    thr_ = nullptr;
}

void RobotViewerSimulator::setSpeedCallback(const speed_cb_t &cb)
{
    cb_ = cb;
}

void RobotViewerSimulator::setPeriod(int period)
{
    chassis_period_ = period;
}

void RobotViewerSimulator::setInitPos(double x, double y, double angle)
{
    vx_     = 0;
    vy_     = 0;
    vtheta_ = 0;
    vw_     = 0;
    is_start_ = false;
}

void RobotViewerSimulator::setSpeed(const SpeedControl &vel)
{
    if(vel.mode == SpeedControl::kControlSpeed) {
        vx_ = vel.vx;
        vy_ = vel.vy;
        vw_  = vel.w;
    }
#if 0
    std::cout << "speed(" << vx_ << ", " << vy_
        << "), w=" << (double)vw_*180/PI << ")" << std::endl;
#endif
}

void RobotViewerSimulator::startTimer()
{
    is_start_ = true;
}

void RobotViewerSimulator::stopTimer()
{
    is_start_ = false;
}

void RobotViewerSimulator::timerThread()
{
    while(is_loop_) {

        std::this_thread::sleep_for(
            std::chrono::milliseconds(chassis_period_));

        if (!is_start_) {
            continue;
        }

        static count_time count_timer;
        count_timer.end();
        float time = count_timer.get_time();
        count_timer.begin();
#if 0
        ODO->set_v(vx_);
        ODO->set_w(vw_);
        ODO->set_theta(vtheta_);
        ODO->OnTimeCaller(time);

        OrientPos pose_;
        pose_.set_map_name("");
        OrientPos orientpos;
        orientpos.x = ODO->get_pos_x();
        orientpos.y = ODO->get_pos_y();
        orientpos.angle = ODO->get_pos_theta();
        pose_.laser_state_ = 0;
        pose_.set_pose(orientpos);
#endif

#if 0
        std::cout << "v(" << vx_<< ", " << vtheta_*180/PI << ", "<< vw_ << ")"
            <<" pos("<<ODO->get_pos_x() << ", " << ODO->get_pos_y()
            << ", " << (double)ODO->get_pos_theta()*180/PI << ")" << std::endl;
#endif
    }
}

void RobotViewerSimulator::OnTimeCaller(double time_one)
{
    double t = time_one/1000;
    //odo_theta_ += vw_* t;

    while(odo_theta_ < -2*PI) {
        odo_theta_ += 2*PI;
    }

    while(odo_theta_ > 2*PI) {
        odo_theta_ -= 2*PI;
    }

#if 1
    odo_x_ += vx_ * t * cos(odo_theta_);
    odo_y_ += vx_ * t * sin(odo_theta_);
#else
    double vx = vx_*cos(odo_theta_);
    double vy = vx_*sin(odo_theta_);
    double delta_x = vx*cos(odo_theta_) + vy*sin()
#endif
}
