#ifndef __COMWISE__ROBOT_VIEWER__CAMERA_DATA__H__
#define __COMWISE__ROBOT_VIEWER__CAMERA_DATA__H__

#include <memory>
#include <string>

struct CameraData {
    int type{0};
    int cmd{0};
    int value{0};

    std::string topic;

    bool is_login{false};
    bool is_play{false};
    bool is_record{false};

    double hd_angle{0.0};
    double yt_angle{0.0};
    double focus_val{0.0};
    double zoom_val{0.0};
};

#endif // __COMWISE__ROBOT_VIEWER__CAMERA_DATA__H__
