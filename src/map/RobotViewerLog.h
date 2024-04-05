/***
* @brief    : comwise robot viewer log
* @author   : comwise
* @version  : V1.0
*/
#ifndef __COMWISE__ROBOT_VIEWER__LOG__H__
#define __COMWISE__ROBOT_VIEWER__LOG__H__

#include <string>
#include <QDebug>

#define LOG_NAME    "RobotViewer"
#define THREAD_ID   0

#define DEBUG_LOG() qDebug() << LOG_NAME << THREAD_ID << ":" << __FUNCTION__ << "() "
#define INFO_LOG()  qDebug() << LOG_NAME << THREAD_ID << ":" << __FUNCTION__ << "() "
#define WARN_LOG()  qDebug() << LOG_NAME << THREAD_ID << ":" << __FUNCTION__ << "() "
#define ERROR_LOG() qDebug() << LOG_NAME << THREAD_ID << ":" << __FUNCTION__ << "() "

inline std::string Trace(const char* fmt, ...)
{
    std::string str;
    try {
        char buff[2048] = { 0 };
        va_list arg;
        va_start(arg, fmt);
        if( -1 == vsprintf(buff, fmt, arg))
            return "";
        va_end(arg);
        printf("%s", buff);

        str = std::string(buff);
        std::size_t pos = str.find_last_of('\n');
        if (pos < str.length())
            str = str.substr(0, pos);
    } catch(std::exception& e) {
        printf("%s\n", e.what());
        str = "";
    } catch(...) {
        printf("unknown exception printf");
        str = "";
    }
    return str;
}

inline std::string Trace(char* fmt, ...)
{
    std::string str;
    try {
        char buff[2048] = { 0 };
        va_list arg;
        va_start(arg, fmt);
        if( -1 == vsprintf(buff, fmt, arg))
            return "";
        va_end(arg);
        printf("%s", buff);

        str = std::string(buff);
        std::size_t pos = str.find_last_of('\n');
        if (pos < str.length())
            str = str.substr(0, pos);
    } catch(std::exception& e) {
        printf("%s\n", e.what());
        str = "";
    } catch(...) {
        printf("unknown exception printf");
        str = "";
    }
    return str;
}

#define TRACE                 Trace
#define TRACE_DEBUG(fmt, ...) DEBUG_LOG() << TRACE(fmt, __VA_ARGS__).c_str()
#define TRACE_INFO(fmt, ...)  INFO_LOG()  << TRACE(fmt, __VA_ARGS__).c_str()
#define TRACE_WARN(fmt, ...)  WARN_LOG()  << TRACE(fmt, __VA_ARGS__).c_str()
#define TRACE_ERROR(fmt, ...) ERROR_LOG() << TRACE(fmt, __VA_ARGS__).c_str()

#endif // __COMWISE__ROBOT_VIEWER__LOG__H__
