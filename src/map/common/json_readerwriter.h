#ifndef __COMWISE__ROBOT_VIEWER__JOSN_READER_WRITER__H__
#define __COMWISE__ROBOT_VIEWER__JOSN_READER_WRITER__H__

#include <string>
#include "json/json.h"

class JsonReaderWriter
{
public:
    using json_value_t = Json::Value;

    static bool read_json(const std::string &file, json_value_t &value);
    static bool write_json(const std::string &file, const json_value_t &value);
};

#endif // __COMWISE__ROBOT_VIEWER__JOSN_READER_WRITER__H__
