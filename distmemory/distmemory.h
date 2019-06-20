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
#include <atomic>
namespace dmy_dist_memory
{


#define LOG printf
#define LOG_ERROR printf

struct redis_tool_key_t
{
	redis_tool_key_t(std::string key, uint64_t id)
	{
		this->key = key;
		this->id = id;
	}
	redis_tool_key_t(const redis_tool_key_t& cp_key)
	{
		key = cp_key.key;
		id = cp_key.id;
	}
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
	UNSUB = 4,
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



class DistMemoryRedisToolThread
{
public:
	DistMemoryRedisToolThread();
	void start_thread();

	void connect_to_redis();
	void sub_connect_to_redis();
	void cmd_connect_to_redis();

	void handle_process();
	void cleanup();
	void check();
	
	redis_tool_list_t sub_list;
	// bool need_reconnect = false;
	bool auth_failure = false;
	bool sub_connect_failure = false;
	bool cmd_connect_failure = false;
	redis_tool_list_t cmd_list;

	void sub_process();
	void cmd_process();

	static void handle_get(redisAsyncContext *c, void *reply, void *privdata);
	static void handle_sub(redisAsyncContext *c, void *reply, void *privdata);

	static void handle_sub_connected(const redisAsyncContext *c, int status);
	static void handle_cmd_connected(const redisAsyncContext *c, int status);
	static void handle_sub_disconnected(const redisAsyncContext *c, int status);
	static void handle_cmd_disconnected(const redisAsyncContext *c, int status);
	static void handle_auth(redisAsyncContext *c, void *reply, void *privdata);
	std::atomic<bool> stop_thread;
private:
	std::thread thread_obj;
	redisAsyncContext *sub_rac = nullptr;
	redisAsyncContext *cmd_rac = nullptr;
    // struct event_base *base = event_base_new();
    struct event_base *sub_base = nullptr;
    struct event_base *cmd_base = nullptr;
};

class DistMemoryRedisTool
{
public:
	static void init_tool(std::string ip, uint32_t port, std::string passwd);
	static void reg_data(std::string& key_str, uint64_t id, 
		std::function<void(std::string*)> parse_func);
	static void set_data(std::string& key_str, uint64_t id, 
		std::string value);
	static void get_data(std::string& key_str, uint64_t id);
	static void unreg_data(std::string& key_str, uint64_t id);
	static std::string get_key(redis_tool_key_t& key);
	static void tick();
private:
	static std::unordered_map<std::string, 
		std::unordered_map<uint64_t, std::function<void(std::string*)>>> parse_func_map;

public:
	struct redis_connect_data_t
	{
		std::string ip;
		uint32_t port;
		std::string passwd;
	};
	static redis_connect_data_t redis_connect_data;
public:
	static DistMemoryRedisToolThread tool_thread;

};

}
#endif



