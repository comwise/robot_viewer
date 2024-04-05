/*! @file count_time.hpp
 *        计算代码时间的类
 *        用来计算辅助查看代码执行时间的类
 *  @code 使用方法是:
 *        #include <count_time.hpp>
 *        count_time time;
 *        time.begin();
 *        coding .....
 *        time.end();
 *        printf("use time = %f\n",get_time());
 *  @endcode
 *
 */

#ifndef __COMWISE__ROBOT_VIEWER__COUNT_TIME__H__
#define __COMWISE__ROBOT_VIEWER__COUNT_TIME__H__

#include <string.h>
#include <iostream>
#include <stdio.h>
#include <chrono>

class count_time
{
private:
    std::chrono::steady_clock::time_point begin_time_;
    std::chrono::steady_clock::time_point end_time_;

public:
    count_time()
    {
        begin();
    }

    /**
     * 开始计时
     */
    void begin()
    {
        begin_time_ = std::chrono::steady_clock::now();
    }

    /*!
     * 结束计时
     * */
    void end()
    {
        end_time_ = std::chrono::steady_clock::now();
    }

    /**
     * @return 返回begin和end之间的花费的时间，单位是毫秒(ms)
     */
    float get_time() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - begin_time_).count();
        ;
    }

    /**
     * 打印花费的时间
     */
    void print_string()
    {
        printf("@Use Time = %f\n", get_time());
    }
};

#endif //__COMWISE__ROBOT_VIEWER__COUNT_TIME__H__
