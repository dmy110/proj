#include "service_main.h"
#include "service_group.h"
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
using namespace dmy_multiprocess_serivce;
using namespace std::literals::chrono_literals;
bool ServiceMain::stop_service = false;
void ServiceMain::handle_service_shutdown(int signum)
{
	stop_service = true;
}

void ServiceMain::init_serive_cfg()
{
	auto& service = service_group_map[1].service_cfg_map[1];
	service = {
		"./services/test_service.so",
		"test_service",
		1,
		1,
		1
	};
}

void ServiceMain::start_main_service()
{
	auto service_main = new ServiceMain();
	service_main->init_serive_cfg();	
	//监听信号，重启，重载配表，关闭
	for (auto& [service_group_id, data] : service_main->service_group_map) {
		int pipe[2];
		if (pipe2(pipe, O_DIRECT|O_NONBLOCK)) {
			perror("pipe create error");
			exit(EXIT_FAILURE);
		}
		data.pipe_write = pipe[1]; 

		int pid = fork();

		if (pid == 0) {
			//child process
			auto service_map = data.service_cfg_map;
			delete service_main;
			close(pipe[1]);
			ServiceGroup::start_service(service_map, pipe[0]);
			return;
		} else {
			//parent process
			close(pipe[0]);
			data.pid = pid;
		}
	}

	service_main->run();
	return ;
}


void ServiceMain::run()
{
	//init signal handler
	struct sigaction interupt_action;
	interupt_action.sa_handler = ServiceMain::handle_service_shutdown;
	sigaction(SIGINT, &interupt_action, nullptr);

	while (!stop_service) {
		//check cfg change
		std::this_thread::sleep_for(5s);
	}

	for (auto& [service_group_id, data] : service_group_map) {
		waitpid(data.pid, nullptr, 0);
	}
}

