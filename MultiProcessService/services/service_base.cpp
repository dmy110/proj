// #include "service_base.h"
// #include "redis_tool.h"
// #include "service_group.h"
// #include <stdio.h>
// using namespace dmy_multiprocess_serivce;
// using namespace std::placeholders;
// using namespace std::literals::chrono_literals;
// void ServiceBase::service_init()
// {
// 	printf("init\n");
// }

// void ServiceBase::service_tick()
// {
// 	Timer _5_s = get_timer(5s);
// 	if (_5_s()) {
// 		printf("tick\n");
// 	}
// }

// // ServiceGroup* ServiceBase::get_service_group()
// // {
// // 	return _service_group;
// // }

// void ServiceBase::service_end()
// {
// 	printf("end\n");
// }

// void ServiceBase::service_process_cmd(std::string& cmd_name, std::string& cmd_data)
// {
// 	printf("cmd_name:%s, cmd_data_len:%u\n", cmd_name);
// }

// void ServiceBase::base_service_process_cmd(std::string& cmd_name, std::string& cmd_data)
// {
// 	service_process_cmd(cmd_name, cmd_data);
// }


// // void ServiceBase::init_service_group(ServiceGroup* service_group)
// // {
// 	// _service_group = service_group;
// 	// _service_group->get_rt().exec_subscribe_cmd(service_get_name(), [this](std::string& data){
// 	// 	auto pos = data.find("|");
// 	// 	if (pos == std::string::npos) {
// 	// 		LOG_ERROR("data error:%s", data.c_str());
// 	// 		return;
// 	// 	}
// 	// 	std::string msg_name = data.substr(0, pos);
// 	// 	std::string msg_data = data.substr(pos, data.size());
// 	// 	// auto dp = DescriptoPool::generated_pool();
// 	// 	// auto descriptor= dp->FindMessageTypeByName(msg_name);
// 	// 	// auto msg = MessageFactory::generated_factory()->GetPrototype(descriptor)->New();
// 	// 	// msg.ParsePartialFromString(msg_data);
// 	// 	service_process_cmd(msg_name, msg_data);	
// 	// });
// 	// _service_group->get_rt().subscribe(service_get_name(), std::bind(&ServiceBase::base_service_process_cmd, this, _1, _2));
	
// // }

// ServiceBase::ServiceBase(){}
// ServiceBase::~ServiceBase(){}

// void ServiceBase::send_cmd(const std::string& cmd_name, std::string& cmd_data)
// {
// 	// _service_group->get_rt().publish(cmd_name, cmd_data);
// 	std::stringstream ss;
// 	ss << "PUBLISH " << service_get_name();
// 	ss << cmd_name << "|" << cmd_data;
// 	_service_group->get_rt().exec_cmd(ss.str());
// }

// std::string ServiceBase::service_get_name()
// {
// 	return std::string("test_service");
// }
