#ifndef _DMY_REDIS_TOOL_H_
#define _DMY_REDIS_TOOL_H_
#include <stdio.h>
#include <string>
#include <stdint.h>
#include "hiredis/adapters/libevent.h"
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "event2/event.h"
#include <unordered_map>
#include <functional>

namespace dmy_redis_tool
{

class RedisTool
{
public:
	RedisTool();
	~RedisTool();
	void init_redis_tool(const std::string& ip, uint32_t port, const std::string& passwd);
	void redis_tool_tick();
private:
	struct redis_connect_data_t
	{
		std::string ip;
		uint32_t port;
		std::string passwd;
	} redis_connect_data;	
	static void handle_auth(redisAsyncContext *c, void *reply, void *privdata);
	bool auth_failure;

	redisAsyncContext *sub_rac = nullptr;
    struct event_base *sub_base = nullptr;
    void sub_connect_to_redis();
	static void handle_sub_connected(const redisAsyncContext *c, int status);
	static void handle_sub_disconnected(const redisAsyncContext *c, int status);
	static bool sub_connect_failure;
	static void handle_psub(redisAsyncContext *c, void *reply, void *privdata);

	redisAsyncContext *cmd_rac = nullptr;
    struct event_base *cmd_base = nullptr;
    void cmd_connect_to_redis();
	static void handle_cmd_connected(const redisAsyncContext *c, int status);
	static void handle_cmd_disconnected(const redisAsyncContext *c, int status);
	static bool cmd_connect_failure;
	// static void handle_cmd(redisAsyncContext *c, void *reply, void *privdata);	

	//pub/sub
public:
	void psubscribe(const std::string& channel, std::function<void(std::string&, std::string&)> callback);
	void publish(const std::string& channel, std::string& data);
private:
	// std::unordered_map<std::string, std::function<void(std::string&, std::string&)>> callback_list;

};



}

#endif