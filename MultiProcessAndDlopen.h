#ifndef _M_P_A_D_H_
#define _M_P_A_D_H_

namespace MPADH
{

struct child_service_data_t
{
	uint32_t version = 0;
	uint32_t service_id = 0;
	std::string so_file_path;
	
	uint32_t pid = 0;
	int pipe[2] = {};
};



std::unorderd_map<uint32_t, child_service_data_t> service_map;

void start_mpadh();

void service_process(int pipeid);

bool terminate;

void mgr_tick();
}












#endif