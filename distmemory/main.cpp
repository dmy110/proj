#include <stdio.h>
#include <string>
#include <stdint.h>
// #include "zookeeper.h"
#include "hiredis/adapters/libevent.h"
#include <signal.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "event2/event.h"
#include <chrono>
#include "distmemory.h"
#include "memory_template.h"
int main()
{
	using namespace dmy_dist_memory;
	DistMemoryRedisTool::init_tool("127.0.0.1", 6379, "");
	for (int i = 0; i < 100; ++i) {
		DistMemory<test_data> test_data("test_data", i);
		{
			test_data.mutable_data()->extra = "hehesimida";
		}
		getchar();	
	}
	return 0;
}