#include "MultiProcessAndDlopen.h"
#include <unistd.h>

using namespace	MPADH;

enum service_state_t
{
	WAIT = 0,
	BEGIN = 1,
}

enum control_type_t
{
	SERVICE_NONE_OP = 0,
	SERVICE_BEGIN = 1,
	SERVICE_END = 2,
};

struct control_msg_t
{
	int type = 0;
	uint32_t service_id = 0;
};

void service_process(int pipeid)
{
	int service_state = WAIT;
	void* dl = nullptr;
	char *error;
	void (*service_tick)();
	void (*service_init)();
	void (*service_end)();

	while (!terminate) {
		control_msg_t control;
		if (read(pipeid, &control, sizeof(control)) == -1) {
			perror("pipe close");
			exit(EXIT_FAILURE);
		}	

		if (service_state == WAIT && control.type == SERVICE_BEGIN) {
			uint32_t service_id = control.service_id;
			std::string so_file = service_map[service_id].so_file_path;
			dl = dlopen(so_file_path.c_str(), RTLD_NOW|RTLD_LOCAL);
			if (dl == nullptr) {
				std::cerr<<dlerror()<<std::endl;
				exit(EXIT_FAILURE);
			}

			dlerror();
			service_tick = dlsym(dl, "service_tick");
			error = dlerror();
			if (error != nullptr) {
				std::cerr << error << std::endl;
				exit(EXIT_FAILURE);
			}

			service_end = dlsym(dl, "service_end");
			error = dlerror();
			if (error != nullptr) {
				std::cerr << error << std::endl;
				exit(EXIT_FAILURE);
			}

			service_init = dlsym(dl, "service_init");
			error = dlerror();
			if (error != nullptr) {
				std::cerr << error << std::endl;
				exit(EXIT_FAILURE);
			}
			service_state = BEGIN;
			service_init();
			continue;
		}

		if (service_state == BEGIN) {
			service_tick();
			if (control.type == SERVICE_END) {
				service_end();
				break;
			}
		}
	}
}

void start_mpadh()
{
	terminate = false;
	//读取配表
	service_map[1].version = 1;
	service_map[1].service_id = 1;
	service_map[1].so_file_path = "/home/dengmuyang/proj/test.so";

	bool child = false;

	//初始化所有服务
	for (auto& [service_id, service_data] : service_map) {
		if (pipe2(service_data.pipe, O_DIRECT|O_NONBLOCK) == -1) {
			perror("pipe generated failed");
			exit(EXIT_FAILURE);
		}

		int pid = fork();
		if (pid == 0) {
			//child
			child = true;
			service_process(service_data.pipe[0]);
		} else {
			service_data.pid = pid;		
		}
	}
	
	//service进程直接退出
	if (child) return;

	//
	for (auto& [service_id, service_data] : service_map) {
		//
		int pipe = service_data.pipe[1];
		control_msg_t control = {SERVICE_BEGIN, service_id};
		if (write(pipe, &control, sizeof(control)) == -1) {
			perror("pipe write error");
			exit(EXIT_FAILURE);
		}
	}

	while(!terminate) {
		mgr_tick();
		sleep(1);
	}
}

void mgr_tick()
{
	
}
