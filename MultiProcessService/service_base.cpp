#include "service_base.h"
#include "redis_tool.h"
#include "service_group.h"
#include <stdio.h>
using namespace dmy_multiprocess_serivce;
using namespace std::placeholders;
void ServiceBase::service_init()
{
	printf("init\n");
}

void ServiceBase::service_tick()
{
	printf("tick\n");
}

void ServiceBase::service_end()
{
	printf("end\n");
}

void ServiceBase::service_process_cmd(std::string& cmd_name, std::string& cmd_data)
{
	printf("cmd_name:%s, cmd_data_len:%u\n", cmd_name);
}

void ServiceBase::base_service_process_cmd(std::string& cmd_name, std::string& cmd_data)
{
	service_process_cmd(cmd_name, cmd_data);
}


void ServiceBase::init_service_group(ServiceGroup* service_group)
{
	_service_group = service_group;
	_service_group->get_rt().psubscribe(service_get_name(), std::bind(&ServiceBase::base_service_process_cmd, this, _1, _2));
}

ServiceBase::ServiceBase(){}
ServiceBase::~ServiceBase(){}

void ServiceBase::send_cmd(const std::string& cmd_name, std::string& cmd_data)
{
	_service_group->get_rt().publish(cmd_name, cmd_data);
}

std::string ServiceBase::service_get_name()
{
	return std::string("test_service");
}
