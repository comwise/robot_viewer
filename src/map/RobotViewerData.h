#ifndef  __COMWISE__ROBOT_VIEWER__DATA__H__
#define  __COMWISE__ROBOT_VIEWER__DATA__H__

#include <string>
#include <vector>
#include <QVariant>
#include <QPolygonF>
#include "common/msg.h"

struct UPL {
   int start_id;
   int end_id;
   double percent;
   int forward;
};

struct Pose {
   double x;
   double y;

   double getX() { return x; }
   double getX() const { return x; }
   double getY() { return y; }
   double getY() const { return y; }
   void setX(double _x) { x = _x; }
   void setY(double _y) { y = _y; }

   Pose(double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}
   Pose(const Pose &pos): x(pos.x), y(pos.y) { }
};

struct OrientPos {
   double x{0.0};
   double y{0.0};
   double angle{0.0};

   OrientPos(double _x = 0.0, double _y = 0.0, double _angle = 0.0)
    : x(_x), y(_y), angle(_angle) { }
};

typedef Pose VecPosition;

class SpeedControl
{
public:
    enum control_mode {
        kControlNone,
        kControlJoyStick,
        kControlSpeed,
        kControlPosition,
    };
    int mode{kControlSpeed};
    double vx{0.0};
    double vy{0.0};
    double w{0.0};
};

class GraphicsItemType : public QVariant {
public:
    enum ITEM_TYPE {
        ITEM_NONE = UserType,
        ITEM_NODE,
        ITEM_EDGE,
        ITEM_CONTROLPOINT,
        ITEM_LINE,
        ITEM_SITE,
        ITEM_MAP,
        ITEM_REFLECTOR,
        ITEM_AGVPOS,
        ITEM_INITPOS,
        ITEM_ARAE,
        ITEM_ARAECORNER,
        ITEM_ARAEDEVICE,
        ITEM_POLYGON,
        ITEM_EMAP,
    };
};

enum Spin {
    NO_SPIN,
    SPIN_ALLOWED
};

enum Tshaped {
    NO_TSHAPED,
    TSHAPED
};

enum EdgeDir {
    POSITIVE_WAY,
    NEGTIVE_WAY,
    BOTH_WAY
};

enum RobotBackable {
    FORWARD_ONLY,
    REVERSE_ALLOWED
};

enum SiteDir {
    POSITIVE_DIR,
    NEGTIVE_DIR
};

enum SiteFuncCharacter {
    INSPECTION,
    CHARGE,
    CHARHE_PREPARE,
    STRART_OUT
};

enum PTZBack {
    ARSE_IN_FRONT,
    HEAD_IN_FRONT
};

enum StopWay {
    STOP_NORMAL,
    STOP_PRECISE
};

enum ActionMode {
    ACTION_SELECT,
    ACTION_MOVE,
    ACTION_HANDDRAG,
    ADD_NODE,
    ADD_EDGE,
    ADD_SITE,
    ADD_AREA,
    ADD_POLYGON,
    SET_LOCROBOT,
    SET_NAVTARGET,
    REMOTE_CONTROL
};

struct Pos {
    double x;
    double y;

    Pos() : x(0.0), y(0.0) { }
    Pos(double _x, double _y) : x(_x),y(_y) {}
};

struct NodeData {
    int id;
    Pos pos;
    Spin spin;
    Tshaped tshaped;

    NodeData() 
        : id(0)
        , spin(NO_SPIN)
        , tshaped(NO_TSHAPED) {
    }
};

struct SensorConfig {
    std::string sensor_name;
    bool sensor_enable;
    int edge_enable;
    double slow_distance;
    double stop_distance;

    SensorConfig()
        : sensor_enable(true)
        , edge_enable(POSITIVE_WAY)
        , slow_distance(0.0)
        , stop_distance(0.0) {

    }
};
typedef std::map<std::string, SensorConfig> SensorMap;

struct EdgeData {
    int id;
    int startid;
    int endid;
    Pos ctrlpos1;
    Pos ctrlpos2;
    EdgeDir direction;
    double weight;
    RobotBackable backward;
    Pos termstpos;
    Pos termendpos;
    double termstper;
    double termendper;
    int edgepriority;
    int sz_laser_bank;
    int tim310_laser_bank;
    double edge_speed;
    int reference_mode;
    bool global_enable;
    double global_slow;
    double global_stop;
    SensorMap sensor_config;

    EdgeData():
      id(0),
      startid(0),
      endid(0),
      direction(POSITIVE_WAY),
      weight(0),
      backward(FORWARD_ONLY),
      termstper(0),
      termendper(1),
      edgepriority(0),
      sz_laser_bank(0),
      tim310_laser_bank(0),
      edge_speed(0.0f),
      reference_mode(0),
      global_enable(true),
      global_slow(0.0),
      global_stop(0.0)
    {

    }
};

struct SaveHeight {
    int id;
    int layer_num;
    std::string floor;
    std::string storage;
    int up_dest_height;
    int up_pre_height;
    int down_dest_height;
    int down_pre_height;
    int dest_start_id;
    int dest_end_id;
    float dest_percent;
    double dest_x;
    double dest_y;
    double pre_x;
    double pre_y;
    int pre_start_id;
    int pre_end_id;
    float pre_percent;

    SaveHeight()
        : id(0)
        , layer_num(0)
        , up_dest_height(0)
        , up_pre_height(0)
        , down_dest_height(0)
        , down_pre_height(0)
        , dest_start_id(0)
        , dest_end_id(0)
        , dest_percent(0.0)
        , dest_x(0.0)
        , dest_y(0.0)
        , pre_x(0.0)
        , pre_y(0.0)
        , pre_start_id(0)
        , pre_end_id(0)
        , pre_percent(0.0)
    {

    }
};

struct SiteData {
    int id;
    Pos pos;
    int startid;
    int endid;
    double percent;
    int parentedgeid;
    SiteDir direction;
    double priority;
    PTZBack yaanback;
    StopWay precisestop;
    std::vector<int> holdids;
    SiteFuncCharacter func_character;
    int waittime;

    SiteData():
        id(0),
        startid(0),
        endid(0),
        percent(0),
        parentedgeid(0),
        direction(POSITIVE_DIR),
        priority(0),
        yaanback(ARSE_IN_FRONT),
        precisestop(STOP_NORMAL),
        func_character(INSPECTION),
        waittime(0) {
    }
};

struct RectData {
    double  corner_x; //左下角x
    double  corner_y; //左下角y
    double  width;
    double  height;

    RectData() :
        corner_x(0.0),
        corner_y(0.0),
        width(0.0),
        height(0.0) {
    }
};

struct AreaData {
    uint32_t id;
    std::string desc;
    RectData rect_data;
    uint16_t feature;
    bool is_update;

    AreaData()
        : id(0)
        , desc("")
        , feature(0)
        , is_update(false){
    }
};

struct PolygonData {
    uint32_t id;
    std::string desc;
    uint32_t area_id;
    uint32_t storage_id;
    uint32_t feature; //0 正常 1 删除数据
    QPolygonF data;

    PolygonData()
        : id(0)
        , desc("")
        , area_id(0)
        , storage_id(0)
        , feature(0) {
    }
};

typedef map_area_t MapArea;


class CoordSystemTransf {
public:
    static double mapLocal2World(double loc){
        return loc/100;
    };
    static double mapWorld2Local(double world){
        return world*100;
    };
};

typedef std::pair<double, double> MapPos;//地图位置坐标x, y
struct MapData {
    MapPos min_pos_; //最小位置坐标
    MapPos max_pos_; //最大位置坐标
    double resolution_;//分辨率
    long num_points_; //物体轮廓点点数
    std::vector<MapPos> pos_datas_;//物体轮廓点数据
    long num_traces_; //车体行走轨迹点数
    std::vector<MapPos> trace_datas_; //车体行走轨迹数据
    std::string file_;
    bool is_file_;

    MapData()
        : resolution_(0)
        , num_points_(0)
        , num_traces_(0)
        , is_file_(false) {

    }
};

struct ProjectInfo {
    std::vector<NodeData> node;
    std::vector<EdgeData> edge;
    std::vector<SiteData> site;
    std::vector<MapArea> area;
};

enum MessageCommand {
    BodyUp,
    BodyDown,
    BodyLeft,
    BodyRight,
    BodyZero,
    BodyStop,

    CameraUp,
    CameraDown,
    CameraZero,
    CameraZoomIn,
    CameraZoomOut,
};

#endif // __COMWISE__ROBOT_VIEWER__DATA__H__
