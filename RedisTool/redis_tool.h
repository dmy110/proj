#ifndef _DMY_REDIS_TOOL_H_
#define _DMY_REDIS_TOOL_H_
#include <list>
#include <stdio.h>
#include <string>
#include <stdint.h>
#include "hiredis/adapters/libevent.h"
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "event2/event.h"
#include <unordered_map>
#include <functional>
#define DMY_CHECK_REDIS_ERROR(c, reply) \
	do { \
		if (!reply) { \
			if (c->err) { \
				LOG_ERROR("sub:%s\n", c->errstr); \
				return ; \
			} \
		} \
		if (reply->type == REDIS_REPLY_ERROR) { \
			LOG_ERROR("reply_error:%s", reply->str); \
			return; \
		} \
	} while (0);

namespace dmy_redis_tool
{

struct sub_callback_t
{
	std::string channel;
	std::function<void(redisReply*)> cb;
};

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
	static void handle_sub(redisAsyncContext *c, void *reply, void *privdata);

	redisAsyncContext *cmd_rac = nullptr;
    struct event_base *cmd_base = nullptr;
    void cmd_connect_to_redis();
	static void handle_cmd_connected(const redisAsyncContext *c, int status);
	static void handle_cmd_disconnected(const redisAsyncContext *c, int status);
	static bool cmd_connect_failure;
	// static void handle_cmd(redisAsyncContext *c, void *reply, void *privdata);	
public:
	//cannot unsubscribe
	void exec_subscribe_cmd(const std::string& channel, std::function<void(redisReply*)> cb);
	void exec_subscribe_cmd(const std::string& channel, std::function<void(std::string)> cb);
private:
	std::unordered_map<std::string, sub_callback_t*> sub_callback;

public:
	void exec_cmd(const std::string& cmd);
	void exec_cmd(const std::string& cmd, std::function<void(redisReply*)> cb);
	void exec_get(const std::string& cmd, std::function<void(std::string)> cb);
private:
	static void handle_cmd_callback(redisAsyncContext *c, void *reply, void *privdata);
};



}

#endif