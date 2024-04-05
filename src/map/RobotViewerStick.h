#ifndef __COMWISE__ROBOT_VIEWER__STICK__H__
#define __COMWISE__ROBOT_VIEWER__STICK__H__

#include <memory>
#include <mutex>
#include <condition_variable>
#include "RobotViewerData.h"

namespace std {
class thread;
}

class ControlCommon {
public:
    template<typename T>
    static void calculateAcc(T& curr, const T& tar, const T& min) 
    {
        T acc = tar - curr;
        if (acc > min) {
            acc = min;
        } else if (acc < -1*min) {
            acc = -1*min;
        } else {
            // do nothing
        }
        curr += acc;
    }

    template<typename T>
    static void calculateLimited(T& curr, const T& limit)
    {
        if (curr > limit) {
            curr = limit;
        } else if (curr < -1*limit) {
            curr = -1 * limit;
        } else {
            //do nothing
        }
    }
};

struct RobotSpeed {
    double vx;
    double vy;
    double w;

    RobotSpeed() : vx(0.0), vy(0.0), w(0.0) { }
};

struct PtzCtrl {
    int    motor_type; //电机类型
    int    yt_cmd;     //命令类型
    double yt_angle;   //云台角度
    double hd_angle;   //相机角度
    int    speed;      //云台转速

    PtzCtrl()
        : motor_type(0)
        , yt_cmd(0)
        , yt_angle(0.0)
        , hd_angle(0.0)
        , speed(7) { }
};

class RobotViewerStick
{
public:
    RobotViewerStick();
    ~RobotViewerStick();

    void setSpeed(MessageCommand cmd);

    void startThread();
    void pauseThread();

private:
    void init();
    void workThread();

    void setSpeedSmooth();
    void setSpeedLimited();

private:
    //加速度
    double acc_max_v_;
    double acc_max_yt_;

    //增量
    double delta_vx_;
    double delta_theta_;
    double delta_yt_angle_;
    double delta_hd_angle_;
    double delta_yt_speed_;

    //速度
    RobotSpeed target_speed_;
    RobotSpeed curr_speed_;
    PtzCtrl target_yt_;
    PtzCtrl curr_yt_;

    bool emergency_stop_enable_;

    std::mutex wait_thread_mtx_;
    std::condition_variable wait_thread_cond_;
    std::mutex thr_lck_t;
    bool is_pause_thread_;
    std::shared_ptr<std::thread> work_thr_;

private:
    enum PtzCmd {
        YtStopCmd  = 0, //stop
        YtLoopCmd  = 1, //loop
        YtAngleCmd = 2, //step
    };

    enum MotorType {
        YtMotor = 0,  //yuntai
        HdMotor = 1,  //camera
    };
};

#endif // __COMWISE__ROBOT_VIEWER__STICK__H__
