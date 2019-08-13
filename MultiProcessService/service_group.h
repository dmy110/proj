#include <unordered_map>
#include <chrono>
#include <functional>
#include "dmy_common/timer.h"
#include "redis_tool.h"
#include "dist_protobuf_mgr.h"
using namespace dmy_redis_tool;
using namespace dmy_dist_protobuf;

namespace dmy_multiprocess_serivce 
{

void send_cmd(const std::string& channel ,const std::string& msg_name, std::string& msg_data);
Timer get_timer(std::chrono::duration<int> duration);


struct service_data_t
{
	std::string so_file_path;
	std::string service_name;	
	int service_id = 0;
	int config_version = 0;
	int service_version = 0;
	void* dl;
	// ServiceBase* service_base;
	void (*service_init)();
	void (*service_tick)();
	void (*service_end)();
	void (*process_cmd)(std::string& cmd_name, std::string& cmd_data);
};



class ServiceBase;
class ServiceGroup
{
public:
	static void start_service();
	ServiceGroup();
private:
	void init_service_cfg();

	std::unordered_map<uint32_t, service_data_t> service_cfg_map;
	void run();

	static bool service_stop;
	static void handle_service_shutdown(int signum);
	std::chrono::time_point<std::chrono::steady_clock> steady_tp;

	// void handle_service_cmd();
	RedisTool rt;
	DistProtobufMgr dpm;
public:	
	Timer get_timer(std::chrono::duration<int> duration);
	RedisTool& get_rt();
private:
	static ServiceGroup* _sg;
public:
	static ServiceGroup* get_service_group();
};
}