#ifndef _DMY_DEIST_MEMORY_H_
#define _DMY_DEIST_MEMORY_H_
#include <stdio.h>
#include <string>
#include <thread>
#include <stdint.h>
// #include "zookeeper.h"
#include "hiredis/adapters/libevent.h"
#include <signal.h>
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "event2/event.h"
#include <chrono>
#include <list>
#include <shared_mutex>
#include <unordered_map>
#include <functional>
#include <map>
namespace dmy_dist_memory
{




struct redis_tool_key_t
{
	std::string key;
	uint64_t id;
};

struct redis_tool_value_t
{
	redis_tool_key_t* key;
	std::string* value;
};

enum class rtct_t
{
	GET = 1,
	PUB = 2,
	SUB = 3,
};

struct redis_tool_command_t
{
	redis_tool_key_t* key;
	rtct_t command_type;
	std::string* value;
};

struct redis_tool_list_t
{
	std::list<redis_tool_command_t> command;
	std::shared_mutex command_lock;

	std::list<redis_tool_value_t> reply;
	std::shared_mutex reply_lock;
};

class DistMemoryRedisTool
{
public:
	static void init_tool();
	static void reg_data(std::string& key_str, uint64_t id, 
		std::function<void(std::string*)> parse_func);

	static void set_data(std::string& key_str, uint64_t id, 
		std::string value);

	static void unreg_data(std::string& key_str, uint64_t id);

	static std::string get_key(redis_tool_key_t& key);

	static void tick();
private:
	// static std::unordered_map<std::string, uint32_t> type_key;
	static std::unordered_map<std::string, 
		std::unordered_map<uint64_t, std::function<void(std::string*)>>> parse_func_map;
	// static std::unordered_map<redis_tool_key_t*, 
		// std::function<void()>> notify_map;
	static redis_tool_list_t sub_list;
	static redis_tool_list_t req_data_list;

	static void handle_pub(redisAsyncContext *c, void *reply, void *privdata);
	static void handle_req(redisAsyncContext *c, void *reply, void *privdata);

	static std::thread sub_thread;
	static std::thread req_data_thread;
};

}
#endif



