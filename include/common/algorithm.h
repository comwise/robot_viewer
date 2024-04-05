#ifndef __COMWISE__ROBOT_VIEWER__ROBOT_ALGORITHM__H__
#define __COMWISE__ROBOT_VIEWER__ROBOT_ALGORITHM__H__

#include <cmath>

namespace algo {

//#define M_PI 3.1415

struct RobotPose
{
    double x{0.0};
    double y{0.0};
    union {
        double theta{0.0};
        double angle;
    };

    RobotPose(double _x = 0, double _y = 0, double _z = 0)
        : x(_x), y(_y), angle(_z) { }
};

struct RobotSpeed
{
    double vx{0.0};
    double vy{0.0};
    double w{0.0};

    RobotSpeed(double _x = 0, double _y = 0, double _z = 0)
        : vx(_x), vy(_y), w(_z) { }
};

//角度转弧度
inline double deg2rad(double x)
{
    return M_PI*x/180.0;
}

//弧度转角度
inline double rad2deg(double x )
{
    return 180.0*x/M_PI;
}

inline double getAngle(float x1, float y1, float x2, float y2)
{
    float angle_temp = 0.0;
    float xx = 0.0, yy = 0.0;
    xx = x2 - x1;
    yy = y2 - y1;
    if (xx == 0.0)
        angle_temp = M_PI / 2.0;
    else
        angle_temp = atan(fabs(yy / xx));
    if ((xx < 0.0) && (yy >= 0.0))
        angle_temp = M_PI - angle_temp;
    else if ((xx < 0.0) && (yy < 0.0))
        angle_temp = M_PI + angle_temp;
    else if ((xx >= 0.0) && (yy < 0.0))
        angle_temp = M_PI * 2.0 - angle_temp;
    return (angle_temp);
}

} // namespace algo 

#endif // __COMWISE__ROBOT_VIEWER__ROBOT_ALGORITHM__H__
