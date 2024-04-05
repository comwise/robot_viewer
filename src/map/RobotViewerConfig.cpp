#include "RobotViewerConfig.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <QFile>
#include <QTextStream>
#include <QPointF>
#include "common/json_readerwriter.h"

RobotViewerMapConfig::RobotViewerMapConfig()
{
}

bool RobotViewerMapConfig::LoadMapData(const std::string &openmap_name, MapData &map_data)
{
    std::ifstream ifs;
    ifs.open(openmap_name.c_str());
    if (!ifs.is_open())
        return false;

    std::string str;
    bool findHeadInfo = false;
    int head_info_counter = 0;

    while (ifs >> str)
    {
        head_info_counter++;
        if (str == "MinPos:")
        {
            findHeadInfo = true;
            break;
        }
        if (head_info_counter > 3)
            return false;
    }

    double min_x, min_y, max_x, max_y;
    long num_points;
    double resolution;
    ifs >> min_x >> min_y;
    ifs >> str; // MaxPos:
    ifs >> max_x >> max_y;
    ifs >> str; // NumPoints:
    ifs >> num_points;
    ifs >> str; // PointsAreSorted:
    ifs >> str; // true
    ifs >> str; // Resolution:
    ifs >> resolution;

    bool findData = false;
    while (ifs >> str)
    {
        if (str == "DATA")
        {
            findData = true;
            break;
        }
    }
    if (findData == false)
        return false;

    double x, y;
    MapPos map_pos;
    std::vector<MapPos> pos_datas;
    long cnt = num_points;
    while (ifs >> x)
    {
        ifs >> y;
        pos_datas.push_back(MapPos(x, y));
        if (--cnt <= 0)
        {
            break;
        }
    }

    str = "";
    findData = false;
    long num_traces_ = 0;
    size_t trace_count = 3;
    std::vector<MapPos> trace_datas;
    while (ifs >> str)
    {
        if (str == "NumPointsPath:")
        {
            ifs >> num_traces_;
            ifs >> str;
            if (str == "PATH")
            {
                findData = true;
                break;
            }
        }
        if (--trace_count <= 0)
        {
            break;
        }
    }

    if (findData)
    {
        while (ifs >> x)
        {
            ifs >> y;
            trace_datas.push_back(MapPos(x, y));
        }
    }

    map_data.min_pos_       = MapPos(min_x, min_y);
    map_data.max_pos_       = MapPos(max_x, max_y);
    map_data.num_points_    = num_points;
    map_data.resolution_    = resolution;
    map_data.pos_datas_     = pos_datas;
    map_data.num_traces_    = num_traces_;
    map_data.trace_datas_   = trace_datas;

    ifs.close();

    return true;
}

void RobotViewerMapConfig::LoadData(const QString &file_name)
{
    heads_.clear();
    points_.clear();
    contents_.clear();
    file_name_ = file_name;
    QFile file(file_name);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    
    QTextStream in(&file);
    bool data_flag = true;
    bool head_flag = true;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList str_list = line.split(":", QString::SkipEmptyParts);
        if (!str_list.isEmpty()) {
            continue;
        }
        if (data_flag && (
            str_list.first() == "FreeLaserRegion" || 
            str_list.first() == "FreeObserverRegion" || 
            str_list.first() == "StrongObserverRegion")) {
            head_flag = false;
            if (str_list.size() > 1) {
                contents_ << line;
            }
        } else if (str_list.first() == "DATA") {
            data_flag = false;
            head_flag = false;
            continue;
        } else {
            if (head_flag) {
                heads_ << line;
            } else {
                points_ << line;
            }
        }
    }
    file.close();
}

void RobotViewerMapConfig::ReplaceContent(const QList<QString> &contents)
{
    contents_ = contents;
}

QList<QString> &RobotViewerMapConfig::GetContents()
{
    return contents_;
}

QList<QString> &RobotViewerMapConfig::GetPoints()
{
    return points_;
}

void RobotViewerMapConfig::SaveData(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;

    QTextStream out(&file);
    for (int i = 0; i < heads_.size(); ++i) {
        out << heads_[i] << "\n";
    }
    for (int i = 0; i < contents_.size(); ++i) {
        out << contents_[i] << "\n";
    }
    out << "DATA\n";
    for (int i = 0; i < points_.size(); ++i) {
        out << points_[i] << "\n";
    }

    file.close();
}

bool RobotViewerConfig::loadData(const std::string &filename, ProjectInfo &data)
{
    std::locale loc = std::locale::global(std::locale(""));

    bool ret = true;
    try {
        json_value_t value;
        if(!JsonReaderWriter::read_json(filename, value)) {
            std::cout << "read json file error, file = " << filename << std::endl;
            ret = false;
        }

        if (value.isMember("Node")) {
            loadData(value["Node"], data.node);
        }

        if (value.isMember("Edge")) {
            loadData(value["Edge"], data.edge);
        }

        if (value.isMember("Site")) {
            loadData(value["Site"], data.site);
        }

        if (value.isMember("Area")) {
            loadData(value["Area"], data.area);
        }

    } catch (std::exception &e) {
        std::cout << "read json file exception, " << e.what() << std::endl;
        ret = false;
    }

    std::locale::global(loc);
    
    return ret;
}

bool RobotViewerConfig::loadData(const std::string &filename, std::vector<AreaData> &data)
{
    std::locale loc = std::locale::global(std::locale(""));

    bool ret = true;
    try {
        json_value_t value;
        if(!JsonReaderWriter::read_json(filename, value)) {
            std::cout << "read json file error, file = " << filename << std::endl;
            ret = false;
        }

        ret = loadData(value, data);

    } catch (std::exception &e) {
        std::cout << "read json file exception, " << e.what() << std::endl;
        ret = false;
    }

    std::locale::global(loc);

    return ret;
}

bool RobotViewerConfig::loadData(const json_value_t &value, std::vector<NodeData> &data)
{
    data.clear();
    for (uint32_t i = 0; i < value.size() ; ++i) {
        const json_value_t& obj = value[i];

        bool repeat = false;

        NodeData node;
        node.id         = obj["id"].asUInt();
        node.spin       = (Spin)obj["spin"].asUInt();
        node.tshaped    = (Tshaped)obj["tshaped"].asUInt();
        if (obj.isMember("pos")) {
            node.pos.x  = CoordSystemTransf::mapWorld2Local(obj["pos"]["x"].asDouble());
            node.pos.y  = CoordSystemTransf::mapWorld2Local(obj["pos"]["y"].asDouble());
        }
        for (std::size_t j = 0; j < data.size(); j++) {
            if (data[j].id == node.id) {
                repeat = true;
                break;
            }
        }
        if (repeat) {
            continue;
        }
        data.push_back(node);
    }
    return true;
}

bool RobotViewerConfig::loadData(const json_value_t &value, std::vector<EdgeData> &data)
{
    data.clear();
    for (uint32_t i = 0; i < value.size() ; ++i) {
        const json_value_t& obj = value[i];

        bool repeat = false;

        EdgeData edgedata;
        edgedata.id             = obj["id"].asUInt();
        edgedata.startid        = obj["startid"].asUInt();
        edgedata.endid          = obj["endid"].asUInt();
        edgedata.ctrlpos1.x     = CoordSystemTransf::mapWorld2Local(obj["controlpos1x"].asDouble());
        edgedata.ctrlpos1.y     = CoordSystemTransf::mapWorld2Local(obj["controlpos1y"].asDouble());
        edgedata.ctrlpos2.x     = CoordSystemTransf::mapWorld2Local(obj["controlpos2x"].asDouble());
        edgedata.ctrlpos2.y     = CoordSystemTransf::mapWorld2Local(obj["controlpos2y"].asDouble());
        edgedata.direction      = (EdgeDir)obj["direction"].asInt();
        edgedata.weight         = CoordSystemTransf::mapWorld2Local(obj["weight"].asDouble());
        edgedata.backward       = (RobotBackable)obj["backward"].asInt();
        edgedata.termstpos.x    = CoordSystemTransf::mapWorld2Local(obj["terminalstx"].asDouble());
        edgedata.termstpos.y    = CoordSystemTransf::mapWorld2Local(obj["terminalsty"].asDouble());
        edgedata.termendpos.x   = CoordSystemTransf::mapWorld2Local(obj["terminalendx"].asDouble());
        edgedata.termendpos.y   = CoordSystemTransf::mapWorld2Local(obj["terminalendy"].asDouble());
        edgedata.termstper      = obj["termstper"].asDouble();
        edgedata.termendper     = obj["termendper"].asDouble();
        edgedata.edgepriority   = obj["edgepriority"].asInt();
        edgedata.reference_mode = obj["reference_mode"].asInt();
        edgedata.edge_speed     = obj["edge_speed"].asDouble();
        edgedata.tim310_laser_bank = obj["tim310_laser_bank"].asUInt();
        edgedata.sz_laser_bank  = obj["sz_laser_bank"].asUInt();

        for (std::size_t j = 0; j < data.size(); j++ ) {
            if (data[j].id == edgedata.id) {
                repeat = true;
                break;
            }
        }
        if (repeat) {
            continue;
        }

        data.push_back(edgedata);
    }
    return true;
}


bool RobotViewerConfig::loadData(const json_value_t &value, std::vector<SiteData> &data) 
{
    data.clear();
    for (uint32_t i = 0; i < value.size() ; ++i) {
        const json_value_t& obj = value[i];

        bool repeat = false;

        SiteData sitedata;
        sitedata.id             = obj["id"].asUInt();
        sitedata.pos.x          = CoordSystemTransf::mapWorld2Local(obj["posx"].asDouble());
        sitedata.pos.y          = CoordSystemTransf::mapWorld2Local(obj["posy"].asDouble());
        sitedata.direction      = (SiteDir)obj["direction"].asInt();
        sitedata.precisestop    = (StopWay)obj["precisestop"].asInt();
        sitedata.startid        = obj["startnodeid"].asUInt();
        sitedata.endid          = obj["endnodeid"].asUInt();
        sitedata.percent        = obj["percent"].asDouble();
        sitedata.parentedgeid   = obj["parentedgeid"].asUInt();
        sitedata.priority       = obj["priority"].asDouble();
        sitedata.yaanback       = (PTZBack)obj["yaanback"].asInt();
        sitedata.func_character = (SiteFuncCharacter)obj["func_character"].asInt();
        sitedata.waittime       = obj["waittime"].asInt();
        for (std::size_t j = 0; j < data.size(); j++) {
            if (data[j].id == sitedata.id) {
                repeat = true;
                break;
            }
        }
        if (repeat) {
            continue;
        }
        data.push_back(sitedata);
    }
    return true;
}

bool RobotViewerConfig::loadData(const json_value_t &value, std::vector<AreaData> &datas)
{
#if 0
    MapArea area;
    area.map_id = value["map_id"].asUInt();
    area.section_id = value["section_id"].asUInt();
    area.resolution = value["resolution"].asDouble();

    if (value.isMember("origin")) {
        area.origin.x = value["resolution"]["x"].asDouble();
        area.origin.y = value["resolution"]["y"].asDouble();
    }
#endif

    if (value.isMember("area")) {
        for(uint32_t i = 0; i < value["area"].size(); ++i) {
            const json_value_t& area_val = value["area"][i];
            AreaData data;
            data.id                 = area_val["id"].asUInt();
            data.desc               = area_val["desc"].asString();
            data.feature            = area_val["feature"].asInt();
            data.is_update          = area_val["is_update"].asBool();
            data.rect_data.corner_x = area_val["rect"]["x"].asDouble();
            data.rect_data.corner_y = area_val["rect"]["y"].asDouble();
            data.rect_data.width    = area_val["rect"]["w"].asDouble();
            data.rect_data.height   = area_val["rect"]["h"].asDouble();
            datas.push_back(data);
        }
    }
    return true;
}

bool RobotViewerConfig::loadData(const json_value_t &value, std::vector<MapArea> &data)
{
    data.clear();
    for (uint32_t i = 0 ; i < value.size() ; ++i) {
        MapArea area;
        const json_value_t& area_val = value[i];
        area.map_id = area_val["map_id"].asUInt();
        area.section_id = area_val["section_id"].asUInt();
        area.resolution = area_val["resolution"].asDouble();

        if (area_val.isMember("origin")) {
            area.origin.x = area_val["origin"]["x"].asDouble();
            area.origin.y = area_val["origin"]["y"].asDouble();
        }

        if (area_val.isMember("polygon")) {
            for(uint32_t j = 0; j < area_val["polygon"].size(); ++j) {
                std::vector<point_t> poly;
                const json_value_t& poly_val = area_val["polygon"][i];
                for(uint32_t k = 0; k < poly_val.size(); ++k) {
                    const json_value_t& pt_val = poly_val[k];
                    point_t pt;
                    pt.x = pt_val["x"].asDouble();
                    pt.y = pt_val["y"].asDouble();
                    pt.z = pt_val["z"].asDouble();
                    poly.push_back(pt);
                }
                area.polygon.push_back(poly);
            }
        }
        data.push_back(area);
    }
    return true;
}

bool RobotViewerConfig::saveData(const std::string &filename, const ProjectInfo &data)
{
    std::locale loc = std::locale::global(std::locale(""));

    json_value_t root;
    saveData(data.node,     root);
    saveData(data.edge,     root);
    saveData(data.site,     root);
    saveData(data.area,     root);
    if(!JsonReaderWriter::write_json(filename, root)) {
        std::cout << "write json to file error, file = " << filename;
        return false;
    }

    std::locale::global(loc);

    return true;
}

bool RobotViewerConfig::saveData(const std::string &filename, const std::vector<AreaData> &data)
{
    std::locale loc = std::locale::global(std::locale(""));

    json_value_t root;
    saveData(data,  root);
    if(!JsonReaderWriter::write_json(filename, root)) {
        std::cout << "write json to file error, file = " << filename;
        return false;
    }

    std::locale::global(loc);

    return true;
}

bool RobotViewerConfig::saveData(const std::string &filename, const MapArea &data)
{
    std::locale loc = std::locale::global(std::locale(""));

    json_value_t root;
    saveData(data, root);
    if(!JsonReaderWriter::write_json(filename, root)) {
        std::cout << "write json to file error, file = " << filename;
        return false;
    }

    std::locale::global(loc);

    return true;
}

bool RobotViewerConfig::saveData(const std::vector<NodeData> &datas, json_value_t &value)
{
    for (std::size_t i = 0; i < datas.size(); i++) {
        const NodeData &data = datas[i];
        json_value_t val;
        val["id"]       = data.id;
        val["spin"]     = data.spin;
        val["tshaped"]  = data.tshaped;
        val["pos"]["x"] = data.pos.x;
        val["pos"]["y"] = data.pos.y;
        value["Node"].append(val);
    }
    return true;
}

bool RobotViewerConfig::saveData(const std::vector<EdgeData> &datas, json_value_t &value)
{
    for (std::size_t i = 0; i < datas.size(); i++) {
        const EdgeData &data = datas[i];
        json_value_t obj;
        obj["id"]                   = data.id;
        obj["startid"]              = data.startid;
        obj["endid"]                = data.endid;
        obj["controlpos1x"]         = CoordSystemTransf::mapLocal2World(data.ctrlpos1.x);
        obj["controlpos1y"]         = CoordSystemTransf::mapLocal2World(data.ctrlpos1.y);
        obj["controlpos2x"]         = CoordSystemTransf::mapLocal2World(data.ctrlpos2.x);
        obj["controlpos2y"]         = CoordSystemTransf::mapLocal2World(data.ctrlpos2.y);
        obj["direction"]            = data.direction;
        obj["weight"]               = CoordSystemTransf::mapLocal2World(data.weight);
        obj["backward"]             = data.backward;
        obj["terminalstx"]          = CoordSystemTransf::mapLocal2World(data.termstpos.x);
        obj["terminalsty"]          = CoordSystemTransf::mapLocal2World(data.termstpos.y);
        obj["terminalendx"]         = CoordSystemTransf::mapLocal2World(data.termendpos.x);
        obj["terminalendy"]         = CoordSystemTransf::mapLocal2World(data.termendpos.y);
        obj["termstper"]            = data.termstper;
        obj["termendper"]           = data.termendper;
        obj["edgepriority"]         = data.edgepriority;
        obj["edge_speed"]           = data.edge_speed;
        obj["reference_mode"]       = data.reference_mode;
        obj["tim310_laser_bank"]    = data.tim310_laser_bank;
        obj["sz_laser_bank"]        = data.sz_laser_bank;
        value["Edge"].append(obj);
    }
    return true;
}

bool RobotViewerConfig::saveData(const std::vector<SiteData> &datas, json_value_t &value)
{
    for (std::size_t i = 0; i < datas.size(); i++) {
        const SiteData &data = datas[i];
        json_value_t obj;
        obj["id"]               = data.id;
        obj["posx"]             = CoordSystemTransf::mapLocal2World(data.pos.x);
        obj["posy"]             = CoordSystemTransf::mapLocal2World(data.pos.y);
        obj["direction"]        = data.direction;
        obj["precisestop"]      = data.precisestop;
        obj["startnodeid"]      = data.startid;
        obj["endnodeid"]        = data.endid;
        obj["percent"]          = data.percent;
        obj["parentedgeid"]     = data.parentedgeid;
        obj["priority"]         = data.priority;
        obj["yaanback"]         = data.yaanback;
        obj["func_character"]   = data.func_character;
        obj["waittime"]         = data.waittime;
        value["Site"].append(obj);
    }
    return true;
}

bool RobotViewerConfig::saveData(const std::vector<AreaData> &datas,  json_value_t &value)
{
    for (std::size_t i = 0; i < datas.size(); i++) {
        const AreaData &data = datas[i];
        json_value_t obj;
        obj["id"]               = data.id;
        obj["desc"]             = data.desc;
        obj["feature"]          = data.feature;
        obj["is_update"]        = data.is_update;
        obj["rect"]["x"]        = data.rect_data.corner_x;
        obj["rect"]["y"]        = data.rect_data.corner_y;
        obj["rect"]["w"]        = data.rect_data.width;
        obj["rect"]["h"]        = data.rect_data.height;
        value["area"].append(obj);
    }
    return true;
}

bool RobotViewerConfig::saveData(const std::vector<MapArea> &datas, json_value_t &value)
{
    for (std::size_t i = 0; i < datas.size(); i++) {
        const MapArea &data = datas[i];
        json_value_t obj;
        obj["map_id"]       = data.map_id;
        obj["section_id"]   = data.section_id;
        obj["resolution"]   = data.resolution;
        obj["width"]        = data.width;
        obj["height"]       = data.height;

        obj["origin"]["x"] = data.origin.x;
        obj["origin"]["y"] = data.origin.y;

        for(auto &poly : data.polygon) {
            json_value_t poly_obj;
            for(auto &pt : poly) {
                json_value_t pt_obj;
                pt_obj["x"] = pt.x;
                pt_obj["y"] = pt.y;
                pt_obj["z"] = pt.z;
                poly_obj.append(pt_obj);
            }
            obj["polygon"].append(poly_obj);
        }
        value["Area"].append(obj);
    }
    return true;
}

bool RobotViewerConfig::saveData(const MapArea &data, json_value_t &obj)
{
    for (int i = 0; i < data.polygon.size(); i++) {
        auto &poly = data.polygon[i];
        json_value_t poly_obj;
        poly_obj["id"] = i + 1;
        for(auto &pt : poly) {
            json_value_t pt_obj;
            pt_obj["x"] = pt.x;
            pt_obj["y"] = pt.y;
            poly_obj["polygon"].append(pt_obj);
        }
        obj["strong_area"].append(poly_obj);
    }
    return true;
}

bool RobotViewerConfig::loadProtectData(const std::string& file, std::vector<EdgeData> &datas)
{
    RobotViewerProtectConfig fp;
    if (file.empty() || !fp.readJsonFromFile(file.c_str())) {
        return false;
    }

    std::size_t edge_count = datas.size();
    for (std::size_t i = 0; i < edge_count; i++) {

        char dst1[32] = { 0 }, dst2[32] = { 0 };
        sprintf(dst1, "%d-%d", datas[i].startid, datas[i].endid);
        sprintf(dst2, "%d-%d", datas[i].endid, datas[i].startid);

        bool flag = false;
        SensorMap smap;

        datas[i].global_enable = fp.getGlobalEnable();
        datas[i].global_slow   = fp.getGlobalSlow();
        datas[i].global_stop   = fp.getGlobalStop();
        if (fp.getSensorByUPL(dst1, smap) || fp.getSensorByUPL(dst2, smap)) {
            datas[i].sensor_config = smap;
        }
    }
    return true;
}

static const std::string GlobalSensor = "前超声|后超声|左前超声|左后超声|右前超声|右后超声|前光电";

bool RobotViewerConfig::saveProtectData(const std::string& file, const std::vector<EdgeData> &datas)
{
    return FPRW->WriteJsonData(file.c_str(), datas);
}

RobotViewerProtectConfig::RobotViewerProtectConfig()
: global_enable_(false)
, global_slow_(0.0)
, global_stop_(0.0)
{

}

RobotViewerProtectConfig::~RobotViewerProtectConfig()
{

}

bool RobotViewerProtectConfig::readJsonFromFile(const char* filename)
{
    Json::String err;
    Json::CharReaderBuilder reader;
    Json::Value root;
    reader["collectComments"] = true;
    reader["allowComments"] = true;

    std::ifstream ifs;
    ifs.open(filename, std::ios::binary);
    if (!Json::parseFromStream(reader, ifs, &root, &err)) {
        std::cout<< "parse file error" << std::endl;
        return false;
    }

    global_enable_ = root["global_enable"].asInt() != 0;
    global_slow_   = root["global_slow"].asDouble();
    global_stop_   = root["global_stop"].asDouble();

    int pos_size = root["safe_config"].size();
    for(int i = 0; i < pos_size; ++i) {
        Json::Value pos_val = root["safe_config"][i];
        std::string edge_id  = pos_val["edge_id"].asString();
        int edge_direction   = pos_val["edge_direction"].asInt();

        SensorMap scv;
        int sensor_size      = pos_val["edge_config"].size();
        for(int j = 0; j < sensor_size; ++j) {
            SensorConfig sc;
            Json::Value sensor_val = pos_val["edge_config"][j];

            sc.sensor_name   = sensor_val["sensor_name"].asString();
            sc.sensor_enable = sensor_val["sensor_enable"].asInt() != 0;
            sc.slow_distance = sensor_val["slow_distance"].asDouble();
            sc.stop_distance = sensor_val["stop_distance"].asDouble();
            sc.edge_enable   = sensor_val["edge_enable"].asInt();

            if (!sc.sensor_name.empty()) {
                scv[sc.sensor_name] = sc;
            }
        }
        if (!edge_id.empty()) {
            safe_config_[edge_id] = scv;
        }
    }
    ifs.close();

    return true;
}

bool RobotViewerProtectConfig::WriteJsonData(const char* filename, const SafeConfigVec& lst)
{
    Json::Value root;
    root["global_enable"] = (int)global_enable_;
    root["global_slow"] = global_slow_;
    root["global_stop"] = global_slow_;
    root["global_sensor"] = GlobalSensor;

    std::size_t pos_size = lst.size();
    SafeConfigVec::const_iterator cit = lst.begin();
    for (std::size_t i = 0; i < pos_size && cit != lst.end(); i++, cit++) {
        Json::Value pos;
        SensorMap smap(cit->second);
        SensorMap::const_iterator scit = smap.begin();
        for ( ; scit != smap.end(); scit++) {
            Json::Value sensor;
            sensor["sensor_name"] = scit->second.sensor_name;
            sensor["sensor_enable"] = scit->second.sensor_enable;
            sensor["edge_enable"] = scit->second.edge_enable;
            sensor["slow_distance"] = scit->second.slow_distance;
            sensor["stop_distance"] = scit->second.stop_distance;
            pos["edge_config"].append(sensor);
        }
        pos["edge_id"] = cit->first;
        pos["edge_direction"] = 0;
        root["safe_config"].append(pos);
    }
    std::ofstream ofs;
    ofs.open(filename);
    if(!ofs.is_open()) {
        return false;
    }
    ofs << root.toStyledString();
    ofs.close();

    return true;
}

bool RobotViewerProtectConfig::WriteJsonData(const char* filename, const std::vector<EdgeData> &datas)
{
    if (datas.empty()) {
        return false;
    }

    Json::Value root;
    root["global_enable"] = (int)datas.front().global_enable;
    root["global_slow"]   = datas.front().global_slow;
    root["global_stop"]   = datas.front().global_stop;
    root["global_sensor"] = GlobalSensor;

    std::size_t pos_size = datas.size();
    for (std::size_t i = 0; i < pos_size; i++) {
        Json::Value pos;
        EdgeData ed(datas[i]);

        char edge_id[32] = { 0 };
        sprintf(edge_id, "%d-%d", ed.startid, ed.endid);

        pos["edge_id"]        = edge_id;
        pos["edge_direction"] = ed.direction;

        SensorMap smap(ed.sensor_config);
        SensorMap::const_iterator scit = smap.begin();
        for ( ; scit != smap.end(); scit++) {
            Json::Value sensor;
            sensor["sensor_name"]   = scit->second.sensor_name;
            sensor["sensor_enable"] = (int)scit->second.sensor_enable;
            sensor["edge_enable"]   = scit->second.edge_enable;
            sensor["slow_distance"] = scit->second.slow_distance;
            sensor["stop_distance"] = scit->second.stop_distance;
            pos["edge_config"].append(sensor);
        }
        root["safe_config"].append(pos);
    }

    std::ofstream ofs;
    ofs.open(filename);
    if(!ofs.is_open()) {
        return false;
    }
    ofs << root.toStyledString();
    ofs.close();

    return true;
}

bool RobotViewerProtectConfig::getGlobalEnable()
{
    return global_enable_;
}

double RobotViewerProtectConfig::getGlobalSlow()
{
    return global_slow_;
}

double RobotViewerProtectConfig::getGlobalStop()
{
    return global_stop_;
}

bool RobotViewerProtectConfig::getSensorEnable(const NvgUpl& upl, const SensorName& sensor)
{
    bool enable = global_enable_;
    SafeConfigVec::const_iterator safe_cit = safe_config_.find(upl);
    if (safe_cit != safe_config_.end())
    {
        SensorMap::const_iterator sensor_cit = safe_cit->second.find(sensor);
        if (sensor_cit != safe_cit->second.end())
        {
            enable = sensor_cit->second.sensor_enable;
        }
    }
    return enable;
}

double RobotViewerProtectConfig::getSensorSlow(const NvgUpl& upl, const SensorName& sensor)
{
    double slow_distance = global_slow_;
    SafeConfigVec::const_iterator safe_cit = safe_config_.find(upl);
    if (safe_cit != safe_config_.end())
    {
        SensorMap::const_iterator sensor_cit = safe_cit->second.find(sensor);
        if (sensor_cit != safe_cit->second.end())
        {
            slow_distance = sensor_cit->second.slow_distance;
        }
    }
    return slow_distance;
}

double RobotViewerProtectConfig::getSensorStop(const NvgUpl& upl, const SensorName& sensor)
{
    double stop_distance = global_stop_;
    SafeConfigVec::const_iterator safe_cit = safe_config_.find(upl);
    if (safe_cit != safe_config_.end())
    {
        SensorMap::const_iterator sensor_cit = safe_cit->second.find(sensor);
        if (sensor_cit != safe_cit->second.end())
        {
            stop_distance = sensor_cit->second.stop_distance;
        }
    }
    return stop_distance;
}

bool RobotViewerProtectConfig::getSensorByUPL(const NvgUpl& upl, SensorMap& lst)
{
    bool ret = false;
    SafeConfigVec::const_iterator safe_cit = safe_config_.find(upl);
    if (safe_cit != safe_config_.end())
    {
        lst = safe_cit->second;
        ret = true;
    }
    return ret;
}
