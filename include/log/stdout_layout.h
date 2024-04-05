#ifndef __COMWISE__ROBOT_VIEWER__LOG_STDOUT_LAYOUT__H__
#define __COMWISE__ROBOT_VIEWER__LOG_STDOUT_LAYOUT__H__

#include "xlog_layout.h"

namespace comwise {
namespace log {

class stdout_layout: public log_layout
{
public:
   virtual void consume(const xlog_entry_t&);

private:
   void print_header(const xlog_entry_t&) const;
   void print_context(const xlog_entry_t&) const;
};

} // namespace log
} // namespace comwise

#endif
