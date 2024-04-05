#ifndef __COMWISE__ROBOT_VIEWER__SIMULATOR__H__
#define __COMWISE__ROBOT_VIEWER__SIMULATOR__H__

#include <memory>
#include <functional>
#include "RobotViewerData.h"

namespace std {
    class thread;
}

class RobotViewerSimulator
{
public:
    using speed_cmd_t = SpeedControl;
    using speed_cb_t = std::function<void(speed_cmd_t &)>;
public:
    RobotViewerSimulator();
    ~RobotViewerSimulator();

    void setSpeedCallback(const speed_cb_t &cb);

    void setPeriod(int period);

    void setInitPos(double x, double y, double angle);
    void setSpeed(const speed_cmd_t &speed);

    void startTimer();
    void stopTimer();

private:
    void init();
    void deinit();

    void timerThread();

    void OnTimeCaller(double time);

private:
    speed_cb_t cb_{nullptr};
    int chassis_period_{100};

    double vx_{0.0};
    double vy_{0.0};
    double vtheta_{0.0};
    double vw_{0.0};

    double speed_v_;     //合成速度
    double speed_theta_; //转角
    double speed_w_;     //合成角速度

    double odo_x_;     //x里程
    double odo_y_;     //y里程
    double odo_theta_; //角度里程，弧度

    bool is_loop_{false};
    bool is_start_{false};
    std::shared_ptr<std::thread> thr_;
};

#endif // __COMWISE__ROBOT_VIEWER__SIMULATOR__H__
