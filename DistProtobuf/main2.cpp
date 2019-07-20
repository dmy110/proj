#include <stdio.h>
#include "test.pb.h"
#include <vector>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <regex>
#include <iostream>
#include <thread>
#include "dist_protobuf_mgr.h"
#include "redis_tool.h"

using namespace dmy_dist_protobuf;
using namespace google;
using namespace protobuf;
using namespace dmy_redis_tool;
int main()
{
	// test::test_reflection msg;
	// msg.set_integer(1);
	// std::vector< const FieldDescriptor * > field_list;
	// auto reflection = msg.GetReflection();
	// reflection->ListFields(msg, &field_list);

	// test::test_reflection msg2;
	// for (const auto it : field_list) {
	// 	printf("%s\n", it->name().c_str());
	// 	reflection->SetInt32(&msg2, it, 100);
	// }
	// printf("%s\n", msg2.ShortDebugString().c_str());

	// std::string s("123 456 789");
	// std::regex regex(" ");
	// std::sregex_token_iterator it{s.begin(), s.end(), regex, -1};
	// std::vector<std::string> v{it, {}};
	// for (auto& it : v) {
	// 	std::cout<<it<<std::endl;
	// }
	RedisTool rt;
	rt.init_redis_tool("127.0.0.1", 6379, "");
	DistProtobufMgr dpm(rt);
	DistProtobufTemplate<test::test_reflection>& dpt = dpm.get_dist_protobuf<test::test_reflection>(0);
	test::test_reflection msg;
	int i = 10;
	msg.set_integer(i);
	msg.set_str("test");
	dpt.init_data(msg);
	while (true) {
		rt.redis_tool_tick();
		dpm.dpm_tick();
		std::this_thread::sleep_for(1s);
		printf("%s\n", dpt.data().ShortDebugString().c_str());
	}
	return 0;
}