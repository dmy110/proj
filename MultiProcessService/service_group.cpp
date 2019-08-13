#include "service_group.h"
#include "dmy_common/dmy_common.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <dlfcn.h>
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include <signal.h>
#include <functional>
using namespace dmy_multiprocess_serivce;
using namespace std::literals::chrono_literals;
using namespace std::placeholders;
using namespace dmy_common;


ServiceGroup* ServiceGroup::_sg = nullptr;
void dmy_multiprocess_serivce::send_cmd(const std::string& channel, const std::string& msg_name, std::string& msg_data)
{
	std::stringstream ss;
	ss << "PUBLISH " << channel << " ";
	ss << msg_name << "|" << msg_data;
	ServiceGroup::get_service_group()->get_rt().exec_cmd(ss.str());
	// get_service_group()->get_rt().exec_cmd()
}

Timer dmy_multiprocess_serivce::get_timer(std::chrono::duration<int> duration)
{
	return 	ServiceGroup::get_service_group()->get_timer(duration);
}

bool ServiceGroup::service_stop = false;
void ServiceGroup::handle_service_shutdown(int signum)
{
	service_stop = true;
}

ServiceGroup* ServiceGroup::get_service_group()
{
	return _sg;
}

void ServiceGroup::start_service()
{
	_sg = new ServiceGroup();
	_sg->run();
}

ServiceGroup::ServiceGroup():dpm(rt)
{
}

void ServiceGroup::init_service_cfg()
{
	service_cfg_map[1] = {
		"./services/test.so",
		"test",
		1,
		1,
		1,
		nullptr,
		nullptr
	};
}

void ServiceGroup::run()
{
	init_service_cfg();

	struct sigaction interupt_action;
	interupt_action.sa_handler = ServiceGroup::handle_service_shutdown;
	sigaction(SIGINT, &interupt_action, nullptr);

	//init data
	rt.init_redis_tool("127.0.0.1", 6379, "");

	//load dl
	for (auto& [service_id, service] : service_cfg_map) {
		char *error;
		// auto& service = service_map[service_id];

		service.dl = dlopen(service.so_file_path.c_str(), RTLD_NOW|RTLD_LOCAL);
		if (service.dl == nullptr) {
			LOG("%s\n", dlerror());
			return;
		}
		dlerror();
		
		// auto get_func = (void*(*)())dlsym(service.dl, service.service_name.c_str());
		// error = dlerror();
		// if (error != nullptr) {
		// 	LOG("%s\n", dlerror());
		// 	return;
		// }
		std::string init_func_name = service.service_name + "_init_func";
		service.service_init = (void(*)())dlsym(service.dl, init_func_name.c_str());
		error = dlerror();
		if (error != nullptr) {
			LOG("%s\n", dlerror());
			return;
		}

		std::string tick_func_name = service.service_name + "_tick_func";
		service.service_tick = (void(*)())dlsym(service.dl, tick_func_name.c_str());
		error = dlerror();
		if (error != nullptr) {
			LOG("%s\n", dlerror());
			return;
		}

		std::string end_func_name = service.service_name + "_end_func";
		service.service_end = (void(*)())dlsym(service.dl, end_func_name.c_str());
		error = dlerror();
		if (error != nullptr) {
			LOG("%s\n", dlerror());
			return;
		}

		std::string process_cmd_func_name = service.service_name + "_process_cmd";
		service.process_cmd = (void(*)(std::string& cmd_name, std::string& cmd_data))dlsym(service.dl, process_cmd_func_name.c_str());
		error = dlerror();
		if (error != nullptr) {
			LOG("%s\n", dlerror());
			return;
		}
		// service.service_base = (ServiceBase*)get_func();
		// service.service_base->init_service_group(this);
		get_rt().exec_subscribe_cmd(service.service_name, [&service](std::string data){
			auto pos = data.find("|");
			if (pos == std::string::npos) {
				LOG_ERROR("data error:%s", data.c_str());
				return;
			}
			std::string msg_name = data.substr(0, pos);
			std::string msg_data = data.substr(pos + 1, data.size());
			service.process_cmd(msg_name, msg_data);				
		});
	}

	//init service
	for (auto& [service_id, service] : service_cfg_map) {
		service.service_init();
	}

	//psubscribe channel
	// for (auto& [service_id, cfg] : service_cfg_map) {
	// 	auto service = service_map[service_id];
	// 	rt.psubscribe(cfg.service_name, std::bind(&ServiceBase::base_service_process_cmd, service.service_base, _1, _2));
	// }
	

	// Timer _5_s = get_timer(5s);
	auto tp = std::chrono::steady_clock().now();
	while(!service_stop) {
		// sys_tp = std::chrono::system_clock().now();
		steady_tp = std::chrono::steady_clock().now();
		// frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(steady_tp - tp).count();

		for (auto& [service_id, service] : service_cfg_map) {
			service.service_tick();
		}

		
		rt.redis_tool_tick();

		tp = steady_tp;
		std::this_thread::sleep_for(1ms);

		// if (_5_s()) {
		// 	//read from pipe
		// 	handle_service_cmd();
		// }
	}
	for (auto& [service_id, service] : service_cfg_map) {
		service.service_end();
	}
}

Timer ServiceGroup::get_timer(std::chrono::duration<int> duration)
{
	return Timer(duration, steady_tp);	
}

// void ServiceGroup::handle_service_cmd()
// {
// 	service_cmd_t cmd;
// 	ssize_t ret = read(pipe_read, &cmd, sizeof(cmd));
// 	if (ret == -1 && errno != EAGAIN && errno != EINTR) {
// 		//something wrong happened,close the process
// 		LOG_ERROR("pipe error:%u", errno);
// 		perror("pipe error");
// 		service_stop = true;
// 		return;	
// 	} else {
// 		return;//do nothing
// 	}
// 	if (ret == 0) {
// 		//pipe close, maybe parent shutdown
// 		LOG_ERROR("pipe shutdown");
// 		service_stop = true;
// 		return;
// 	}
// 	if (ret != sizeof(cmd)) {
// 		LOG_ERROR("pipe read size error:%u", ret);
// 		service_stop = true;
// 		return;
// 	}

// 	if (cmd.cmd_type == SERVICE_CMD_STOP) {
// 		service_stop = true;
// 		return;
// 	}
// }


RedisTool& ServiceGroup::get_rt()
{
	return rt;
}
