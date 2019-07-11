#include "service_group.h"
#include "service_base.h"
#include "dmy_common.h"
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

void ServiceGroup::start_service(std::unordered_map<uint32_t, service_data_t> service_map,
		int pipe_read)
{
	auto service_group = new ServiceGroup(service_map, pipe_read);
	service_group->run();
	
}

ServiceGroup::ServiceGroup(std::unordered_map<uint32_t, service_data_t> service_cfg_map,
		int pipe_read):service_cfg_map(service_cfg_map),pipe_read(pipe_read)
{
}

void ServiceGroup::run()
{
	//init data
	rt.init_redis_tool("127.0.0.1", 6379, "");

	//load dl
	for (auto& [service_id, data] : service_cfg_map) {
		char *error;
		auto& service = service_map[service_id];

		service.dl = dlopen(data.so_file_path.c_str(), RTLD_NOW|RTLD_LOCAL);
		if (service.dl == nullptr) {
			LOG("%s\n", dlerror());
			return;
		}
		dlerror();

		auto get_func = (void*(*)())dlsym(service.dl, data.service_name.c_str());
		error = dlerror();
		if (error != nullptr) {
			LOG("%s\n", dlerror());
			return;
		}
		service.service_base = (ServiceBase*)get_func();
		service.service_base->init_service_group(this);
		// func.service_end = (void(*)())dlsym(func.dl, data.end_func_name.c_str());
		// error = dlerror();
		// if (error != nullptr) {
		// 	std::cerr << error << std::endl;
		// 	// exit(EXIT_FAILURE);
		// 	return;
		// }

		// func.service_init = (void(*)())dlsym(func.dl, data.init_func_name.c_str());
		// error = dlerror();
		// if (error != nullptr) {
		// 	std::cerr << error << std::endl;
		// 	// exit(EXIT_FAILURE);
		// 	return;
		// }
	}

	//init service
	for (auto& [service_id, service] : service_map) {
		service.service_base->service_init();
	}

	//psubscribe channel
	// for (auto& [service_id, cfg] : service_cfg_map) {
	// 	auto service = service_map[service_id];
	// 	rt.psubscribe(cfg.service_name, std::bind(&ServiceBase::base_service_process_cmd, service.service_base, _1, _2));
	// }
	

	Timer _5_s = get_timer(5s);
	auto tp = std::chrono::steady_clock().now();
	while(!service_stop) {
		// sys_tp = std::chrono::system_clock().now();
		steady_tp = std::chrono::steady_clock().now();
		// frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(steady_tp - tp).count();

		for (auto& [service_id, service] : service_map) {
			service.service_base->service_tick();
		}

		rt.redis_tool_tick();

		tp = steady_tp;
		std::this_thread::sleep_for(1ms);

		if (_5_s()) {
			//read from pipe
			handle_service_cmd();
		}
	}
	for (auto& [service_id, service] : service_map) {
		service.service_base->service_end();
	}
}

Timer ServiceGroup::get_timer(std::chrono::duration<int> duration)
{
	return Timer(duration, steady_tp);	
}

void ServiceGroup::handle_service_cmd()
{
	service_cmd_t cmd;
	ssize_t ret = read(pipe_read, &cmd, sizeof(cmd));
	if (ret == -1 && errno != EAGAIN && errno != EINTR) {
		//something wrong happened,close the process
		LOG_ERROR("pipe error:%u", errno);
		perror("pipe error");
		service_stop = true;
		return;	
	} else {
		return;//do nothing
	}
	if (ret == 0) {
		//pipe close, maybe parent shutdown
		LOG_ERROR("pipe shutdown");
		service_stop = true;
		return;
	}
	if (ret != sizeof(cmd)) {
		LOG_ERROR("pipe read size error:%u", ret);
		service_stop = true;
		return;
	}

	if (cmd.cmd_type == SERVICE_CMD_STOP) {
		service_stop = true;
		return;
	}
}


RedisTool& ServiceGroup::get_rt()
{
	return rt;
}
