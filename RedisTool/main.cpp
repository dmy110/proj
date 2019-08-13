#include <iostream>
#include <stdio.h>
#include "redis_tool.h"
#include <thread>
using namespace std::literals::chrono_literals;
using namespace dmy_redis_tool;
int main()
{
	// printf("hello world\n");
	RedisTool rt;
	rt.init_redis_tool("127.0.0.1", 6379, "");
	// rt.redis_tool_tick();
	rt.exec_cmd("MULTI");
	rt.exec_cmd("SET test_redis_tool 123");
	// rt.exec_subscribe_cmd("test_topic", [](std::string string){
	// 	printf("%s\n", string.c_str());
	// });
	rt.exec_cmd("PUBLISH test_topic 1234");
	// rt.exec_get("test_redis_tool", [](std::string s){
	// 	// printf("%s\n", s.c_str());
	// });
	
	rt.exec_cmd("PUBLISH 'test.test_reflection' '0|str|integer'");
	std::vector<std::tuple<std::string, std::string>> hmset = {
		{"1", "1"},
		{"2", "2"}, 
		{"3", "3"}
	};
	rt.exec_hmset("test_hm", hmset);
	rt.exec_cmd("EXEC");
	// std::vector<std::string> field = {"1", "2", "3"};
	// rt.exec_hmget("test_hm", field, [](std::vector<std::tuple<std::string, std::string>>& vec){
	// 	for (auto& it : vec) {
	// 		std::cout<<std::get<0>(it)<<";"<<std::get<1>(it)<<std::endl;
	// 	}
	// });
	while (true) {
		rt.redis_tool_tick();
		std::this_thread::sleep_for(1s);
	}
	return 0;
}