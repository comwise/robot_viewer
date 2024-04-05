#ifndef __COMWISE__ROBOT_VIEWER__LOG_XLOG_LAYOUT__H__
#define __COMWISE__ROBOT_VIEWER__LOG_XLOG_LAYOUT__H__

#include <cstdint>
#include "xlog_var.h"

namespace comwise {
namespace log {

/**
 * Consumes a log entry to output it somewhere.
 */
class log_layout
{
public:
    virtual ~log_layout() {};
    virtual void consume(const xlog_entry_t&) = 0;
};

} // namespace log
} // namespace comwise

#endif // __COMWISE__ROBOT_VIEWER__LOG_XLOG_LAYOUT__H__
