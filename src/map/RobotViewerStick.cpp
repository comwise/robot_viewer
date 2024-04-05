#include "RobotViewerStick.h"
#include "RobotViewerLog.h"
#include <thread>

//speed vx 1000 = 1m/s
//theta Θ  10   = 1°
static const double PION           = 3.1415926;
static const double ACC_DEFAULT_V  = 10.0;
static const double ACC_DEFAULT_W  = 10.0;
static const double ACC_MAX_V      = 10000; //10m/s
static const double DELTA_VX       = 100.0;//0.1m/s
static const double DELTA_THETA    = 30.0; //3°
static const double DELTA_YT_ANGLE = 30.0;
static const double DELTA_HD_ANGLE = 1.0;
static const double DELTA_YT_SPEED = 7.0;//2~10
static const double SPEED_MAX_VX   = 1000; //1m/s
static const double SPEED_MAX_VY   = 300; //30°
static const double ANGLE_MAX_YT   = 360;
static const double ANGLE_MAX_HD   = 25;


RobotViewerStick::RobotViewerStick()
  : acc_max_v_(ACC_DEFAULT_V)
  , acc_max_yt_(0.0)
  , delta_vx_(DELTA_VX)
  , delta_theta_(DELTA_THETA)
  , delta_yt_angle_(DELTA_YT_ANGLE)
  , delta_hd_angle_(DELTA_HD_ANGLE)
  , delta_yt_speed_(DELTA_YT_SPEED)
  , emergency_stop_enable_(false)
  , is_pause_thread_(true)
{
    init();
}

RobotViewerStick::~RobotViewerStick()
{

}

void RobotViewerStick::init()
{
    try {
        //NODE.advertise<SpeedControl>("msg_robot_speed_control");
    } catch (...) {
        TRACE("connect message error: node exception\n");
    }

    //work_thr_ = std::make_shared<std::thread>(std::bind(&RobotViewerStick::workThread, this));
}

void RobotViewerStick::startThread()
{
    TRACE("start stick thread\n");
    is_pause_thread_ = false;
    curr_speed_      = RobotSpeed();
    target_speed_    = RobotSpeed();
    {
        std::unique_lock <std::mutex> lck(wait_thread_mtx_);
        wait_thread_cond_.notify_one();
    }
}

void RobotViewerStick::pauseThread()
{
    //TRACE("pause stick thread\n");
    is_pause_thread_ = true;
    curr_speed_      = RobotSpeed();
    target_speed_    = RobotSpeed();

    if(work_thr_ && work_thr_->joinable()) {
        work_thr_->join();
    }
    work_thr_ = nullptr;
}

void RobotViewerStick::workThread()
{
    while(true) {
        {
            std::unique_lock <std::mutex> lck(wait_thread_mtx_);
            while(is_pause_thread_) {
                //TRACE("pause stick...\n");
                wait_thread_cond_.wait(lck);
            }
        }

        //Sleep(10);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // set speed smooth
        setSpeedSmooth();

        TRACE("curr_speed: vx=%lfm/s, theta=%lf°\n", curr_speed_.vx/1000.0, curr_speed_.vy/10);
        TRACE("ytA=%lf, hdA=%lf, ytV=%d\n", target_yt_.yt_angle, target_yt_.hd_angle, target_yt_.speed);

#if 0
        // publish speed to drive
        SpeedControl speed;
        speed.set_type(SpeedControl::ROBOT_SPEED_CONTROL);
        speed.mutable_speed()->set_v(curr_speed_.vx/1000.0);
        speed.mutable_speed()->set_vy((curr_speed_.vy/10)/180*PION);
        speed.mutable_speed()->set_w(0);
        //NODE.advertise<SpeedControl>("msg_robot_speed_control")->publish(speed);

        GPtzCtrl yt;
        yt.set_angle1(target_yt_.yt_angle);
        yt.set_angle2(target_yt_.hd_angle);
        yt.set_cmd(target_yt_.yt_cmd);
        yt.set_motor_type(target_yt_.motor_type);
        yt.set_speed(target_yt_.speed);
        //NODE.advertise<GPtzCtrl>("msg_yuntai_control")->publish(yt);
#endif
    }
}

void RobotViewerStick::setSpeed(MessageCommand cmd)
{
    TRACE("set speed command: %d\n", cmd);
    switch(cmd)
    {
    case BodyUp:
        acc_max_v_         = ACC_DEFAULT_V;
        target_speed_.vx  += delta_vx_;
        break;
    case BodyDown:
        acc_max_v_         = ACC_DEFAULT_V;
        target_speed_.vx  -= delta_vx_;
        break;
    case BodyLeft:
        acc_max_v_         = 2*ACC_DEFAULT_V;
        target_speed_.vy  += delta_theta_;
        break;
    case BodyRight:
        acc_max_v_         = 2*ACC_DEFAULT_V;
        target_speed_.vy  -= delta_theta_;
        break;
    case BodyZero: //stop speed slowly
        acc_max_v_         = ACC_DEFAULT_V;
        target_speed_.vx   = 0;
        target_speed_.vy   = 0;
        break;
    case BodyStop:  //stop speed quickly
        acc_max_v_         = ACC_MAX_V;
        target_speed_.vx   = 0;
        target_speed_.vy   = 0;
        target_speed_.w    = 0;
        break;

    case CameraUp:
        target_yt_.hd_angle   += delta_hd_angle_;
        target_yt_.motor_type  = HdMotor;
        target_yt_.yt_cmd      = YtAngleCmd;
        break;
    case CameraDown:
        target_yt_.hd_angle   -= delta_hd_angle_;
        target_yt_.motor_type  = HdMotor;
        target_yt_.yt_cmd      = YtAngleCmd;
        break;
    case CameraZero:
        target_yt_.hd_angle    = 0;
        target_yt_.motor_type  = HdMotor;
        target_yt_.yt_cmd      = YtAngleCmd;
        break;
        
    default:
        break;
    }

    setSpeedLimited();
}

void RobotViewerStick::setSpeedSmooth()
{
    // driver
    ControlCommon::calculateAcc<double>(curr_speed_.vx, target_speed_.vx, acc_max_v_);
    ControlCommon::calculateAcc<double>(curr_speed_.vy, target_speed_.vy, acc_max_v_);
    
    // yuntai
    //ControlCommon::calculateAcc<double>(curr_speed_.w,  target_speed_.w,  acc_max_w_);
}

void RobotViewerStick::setSpeedLimited()
{
    ControlCommon::calculateLimited<double>(target_speed_.vx,    SPEED_MAX_VX);
    ControlCommon::calculateLimited<double>(target_speed_.vy,    SPEED_MAX_VY);
    ControlCommon::calculateLimited<double>(target_yt_.hd_angle, ANGLE_MAX_HD);

    target_yt_.yt_angle = (long)target_yt_.yt_angle%360;
    if (target_yt_.speed > 10) {
        target_yt_.speed = 10;
    } else if (target_yt_.speed < 2) {
        target_yt_.speed = 2;
    }
}
