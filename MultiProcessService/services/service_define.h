#ifndef _DMY_SERVICE_DEFINE_H_
#define _DMY_SERVICE_DEFINE_H_
#include <string>
#include "dmy_common/timer.h"
#define REG_SERVICE(service_name) \
	extern "C" \
	{ \
		void service_name##_init_func(); \
		void service_name##_tick_func(); \
		void service_name##_end_func(); \
		void service_name##_process_cmd(std::string& cmd_name, std::string& cmd_data); \
	}
	
using namespace dmy_common;
namespace dmy_multiprocess_serivce 
{
void send_cmd(const std::string& channel, const std::string& msg_name, std::string& msg_data);
Timer get_timer(std::chrono::duration<int> duration);
}

#endif