#include <unordered_map>
#include <chrono>
#include <functional>
#include "timer.h"
#include "service_main.h"
#include "redis_tool.h"
using namespace dmy_redis_tool;

namespace dmy_multiprocess_serivce 
{

class ServiceBase;
class ServiceGroup
{
public:
	struct service_t
	{
		void* dl;
		ServiceBase* service_base;
	};
public:
	static void start_service(std::unordered_map<uint32_t, service_data_t> service_map,
		int pipe_read);
	ServiceGroup(std::unordered_map<uint32_t, service_data_t> service_map,
		int pipe_read);
private:
	int pipe_read = 0;
	std::unordered_map<uint32_t, service_data_t> service_cfg_map;
	void run();

	// time_t now_sec = 0;
	std::unordered_map<uint32_t, service_t> service_map;
	
	bool service_stop = false;
	// time_t now_sec = 0;
	// uint32_t duration_ms = 0;
	// std::chrono::duration<int> frame_duration;

	// std::chrono::time_point<std::chrono::system_clock> sys_tp;
	std::chrono::time_point<std::chrono::steady_clock> steady_tp;

	void handle_service_cmd();
	RedisTool rt;
public:	
	Timer get_timer(std::chrono::duration<int> duration);
	RedisTool& get_rt();
	// void publish(const std::string& channel, std::string& data);
	// void psubscribe()
	// uint32_t get_duration_ms();
	// void channel(const std::string& channel_name);

private:
};
}