#ifndef __COMWISE__ROBOT_VIEWER__CONFIG__H__
#define __COMWISE__ROBOT_VIEWER__CONFIG__H__

#include <string>
#include <vector>
#include <map>
#include <QString>
#include <QStringList>
#include "common/singleton.h"
#include "RobotViewerData.h"

namespace Json {
    class Value;
}
using json_value_t = Json::Value;

class RobotViewerMapConfig
{
public:
    RobotViewerMapConfig();

    bool LoadMapData(const std::string &openmap_name, MapData &map_data);
    void LoadData(const QString &file_name);
    void SaveData(const QString &file_name);

    QList<QString>& GetPoints();
    QList<QString>& GetContents();
    
    void ReplaceContent(const QList<QString> &contents);

private:
    QString file_name_;
    QList<QString> points_;
    QList<QString> heads_;
    QList<QString> contents_;
};

class RobotViewerConfig : public singleton<RobotViewerConfig>
{
public:
    //route config
    bool loadData(const std::string &filename, ProjectInfo &data);
    bool saveData(const std::string &filename, const ProjectInfo &data);

    // area config
    bool loadData(const std::string &filename, std::vector<AreaData> &data);
    bool saveData(const std::string &filename, const std::vector<AreaData> &data);
    bool saveData(const std::string &filename, const MapArea &data);

    // protect config
    bool loadProtectData(const std::string &file, std::vector<EdgeData> &edgedatas);
    bool saveProtectData(const std::string &file, const std::vector<EdgeData> &EdgeData);

private:
    bool loadData(const json_value_t &value, std::vector<NodeData> &data);
    bool loadData(const json_value_t &value, std::vector<EdgeData> &data);
    bool loadData(const json_value_t &value, std::vector<SiteData> &data);
    bool loadData(const json_value_t &value, std::vector<AreaData> &data);
    bool loadData(const json_value_t &value, std::vector<MapArea> &data);

    bool saveData(const std::vector<NodeData>   &data,  json_value_t &value);
    bool saveData(const std::vector<EdgeData>   &data,  json_value_t &value);
    bool saveData(const std::vector<SiteData>   &data,  json_value_t &value);
    bool saveData(const std::vector<AreaData>   &data,  json_value_t &value);
    bool saveData(const std::vector<MapArea>    &data,  json_value_t &value);
    bool saveData(const MapArea                 &data,  json_value_t &value);
};

#define PRJ_CFG RobotViewerConfig::instance()

class RobotViewerProtectConfig : public singleton<RobotViewerProtectConfig>
{
public:
    typedef std::string SensorName;
    typedef std::string NvgUpl;
    typedef std::map<NvgUpl, SensorMap> SafeConfigVec;

public:
    RobotViewerProtectConfig();
    ~RobotViewerProtectConfig();

    bool readJsonFromFile(const char *filename);
    bool WriteJsonData(const char *filename, const SafeConfigVec &datas);
    bool WriteJsonData(const char *filename, const std::vector<EdgeData> &datas);

    bool getGlobalEnable();
    double getGlobalSlow();
    double getGlobalStop();

    bool getSensorEnable(const NvgUpl &upl, const SensorName &sensor);
    double getSensorSlow(const NvgUpl &upl, const SensorName &sensor);
    double getSensorStop(const NvgUpl &upl, const SensorName &sensor);
    bool getSensorByUPL(const NvgUpl &upl, SensorMap &sensors);

private:
    bool global_enable_;
    double global_slow_;
    double global_stop_;

    SafeConfigVec safe_config_;
};

#define FPRW RobotViewerProtectConfig::instance()

#endif // __COMWISE__ROBOT_VIEWER__CONFIG__H__
