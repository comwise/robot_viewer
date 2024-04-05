#ifndef __COMWISE__ROBOT_VIEWER__MESSAGE__H__
#define __COMWISE__ROBOT_VIEWER__MESSAGE__H__

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "algorithm.h"

enum robot_status_t
{
    kStatusNone,
    kStatusNormal,
    kStatusWarn,
    kStatusError,
    kStatusFatal,
};

enum sensor_type_t 
{
    kSensorNone,
    kSensorLaser,
    kSensorCamera,
    kSensorMap,
    kSensorPathPlanner,
};

struct header_t
{
    uint64_t seq{0};
    uint64_t stamp{0};
    std::string frame_id;
};

struct point_t
{
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

using polygon_t = std::vector<point_t>;

struct quaternion_t
{
    double x{0.0};
    double y{0.0};
    double z{0.0};
    double w{0.0};
};

struct pose_t 
{
    point_t position;
    quaternion_t orientation;
};

struct robot_pos
{
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

struct velocity_t 
{
    double v {0.0};
    union {
        double angle{0.0};
        double theta;
    };
    union {
        double w{0.0};
        double omega;
    };
};

using vel_cmd_t = velocity_t;
using vel_feedback_t = velocity_t;

struct position_t
{
    double x{0.0};
    double y{0.0};
    union {
        double z{0.0};
        double yaw;
        double angle;
        double theta;
    };
};

struct battery_state_t 
{
    double timestamp{0};

    uint8_t battery_type{0};

    double current{0.0};
    double voltage{0.0};
    double charge{0.0};
    double capacity{0.0};
    double design_capacity{0.0};
    double percentage{0.0};
    uint8_t power_supply_status{0};
    uint8_t power_supply_health{0};
    uint8_t power_supply_technology{0};
    bool present{false};
    std::vector<double> cell_voltage;
    std::string location;
    std::string serial_number;

    bool charge_enable{false};
};

struct camera_sub_t
{
    std::string topic;
    int frame{0};
    bool subscribe{false};

    camera_sub_t(const std::string &_topic = "", int _frame = 0, bool _sub = false) 
        : topic(_topic), frame(_frame), subscribe(_sub) {}
};

struct laser_scan_t
{
    uint32_t seq{0};
    double timestamp{0.0};
    std::string frame_id;
    double angle_min{0};
    double angle_max{0};
    double angle_increment{0.0};
    double time_increment{0.0};
    double scan_time{0.0};
    double range_min{0.0};
    double range_max{0.0};
    std::vector<float> ranges;
    std::vector<float> intensities;
};

struct occupancy_grid_t : public header_t
{
    int64_t map_load_time;
    double resolution;
    uint32_t width;
    uint32_t height;
    pose_t origin;
    std::vector<int8_t> data;
};

struct map_area_t {
    int32_t map_id{0};
    int32_t section_id{0};
    uint32_t width{0};
    uint32_t height{0};
    point_t origin;
    float resolution{0.0};
    std::vector<polygon_t> polygon;
};


#endif // __COMWISE__ROBOT_VIEWER__MESSAGE__H__
