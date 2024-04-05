#include "json_readerwriter.h"
#include <iostream>
#include <fstream>
#include "common/linux_chcwd.h"
#include "common/time.h"

#define LOG_DEBUG  printf 
#define LOGP_WARN  printf


bool JsonReaderWriter::read_json(const std::string &file, json_value_t &value)
{
    bool ret = true;
    try {
        Json::String err;
        Json::CharReaderBuilder reader;
        reader["collectComments"] = true;
        reader["allowComments"] = true;

        std::ifstream ifs;
        ifs.open(file, std::ios::binary);

        if(!Json::parseFromStream(reader, ifs, &value, &err)) {
            LOG_DEBUG("read json file error, file = %s\n", file.c_str());
            ret = false;
        }
        ifs.close();
    } catch (std::exception &e) {
        LOG_DEBUG("read json file exception, %s\n", e.what());
        ret = false;
    }
    return ret;
}

bool JsonReaderWriter::write_json(const std::string &file, const json_value_t &value)
{
    bool ret = true;

    try {
        //! 1\ back old file
        std::string file_name = common::get_file_name(file);
        std::string dir = common::get_file_path(file) + "/bak/";
        if(!common::is_dir_exist(dir)) {
            if(!common::create_dir(dir)) {
                LOGP_WARN("create directory failed, dir = %s", dir.c_str());
            }
        }
        std::string back_name = dir + file_name + "_" + common::get_current_systime() + ".bak";
        if (std::rename(file.c_str(), back_name.c_str())) {
            LOGP_WARN("backup config file failed, file = %s\n", back_name.c_str());
        }

        //! 2\ write new config
        Json::StreamWriterBuilder builder;
        builder["emitUTF8"]     = true;
        builder["indentation"]  = "  ";
        builder["precision"]    = 9;
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        std::ofstream ofs;
        ofs.open(file);
        writer->write(value, &ofs);
        ofs.close();
    } catch(std::exception &e) {
        LOG_DEBUG(" write file exception, file = %s\n", file.c_str());
        ret = false;
    }

    return ret;
}
