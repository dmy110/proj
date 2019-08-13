#include <cstdio>
#include "service_define.h"
using namespace std::literals::chrono_literals;
using namespace dmy_multiprocess_serivce;
using namespace dmy_common;
REG_SERVICE(test)

void test_init_func()
{
	printf("init\n");
}

void test_tick_func()
{
	static Timer t5s(get_timer(60s));
	if (t5s()) {
		printf("tick\n");
	}
}

void test_end_func()
{
	printf("end\n");
}

void test_process_cmd(std::string& cmd_name, std::string& cmd_data)
{
	printf("%s %s\n", cmd_name.c_str(), cmd_data.c_str());
	send_cmd("gateway", cmd_name, cmd_data);
}



