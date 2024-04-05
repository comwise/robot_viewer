#ifndef __COMWISE__ROBOT_VIEWER__STRING_H__
#define __COMWISE__ROBOT_VIEWER__STRING_H__

#include <fstream>
#include <sstream>
#include <string>

namespace common {

class string final
{
public:
  static std::string load_string_from_file(const std::string &file_path)
  {
    std::ifstream ifs(file_path);
    if (!ifs.is_open()) {
      ifs.close();
      return std::string();
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    return ss.str();
  }

  static bool save_string_to_file(const std::string &file_path,
                               const std::string &data)
  {
    std::ofstream ofs(file_path);
    if (!ofs.is_open()) {
      ofs.close();
      return false;
    }
    ofs << data;
    ofs.close();
    return true;
  }
};

} // namespace common

#endif // __COMWISE__ROBOT_VIEWER__STRING_H__
