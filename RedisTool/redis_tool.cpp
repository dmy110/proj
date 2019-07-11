#include "redis_tool.h"
#include "dmy_common.h"
#include <signal.h>
#include <cstring>
using namespace dmy_redis_tool;
using namespace dmy_common;
void RedisTool::init_redis_tool(const std::string& ip, uint32_t port, const std::string& passwd)
{
	redis_connect_data = {
		ip,
		port,
		passwd
	};

	// signal(SIGPIPE, SIG_IGN);
	struct sigaction pipe_action;
	pipe_action.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &pipe_action, nullptr);

	sub_connect_to_redis();
	cmd_connect_to_redis();
}


void RedisTool::handle_sub_connected(const redisAsyncContext *c, int status)
{
	LOG("redis connect:%d\n", status);
	if (status == REDIS_ERR) {
		// std::this_thread::sleep_for(1s);
		sub_connect_failure = true;
	}
}

void RedisTool::handle_sub_disconnected(const redisAsyncContext *c, int status)
{
	LOG_ERROR("disconnected:%s\n", c->errstr);	
	sub_connect_failure = true;	
}

void RedisTool::sub_connect_to_redis()
{
    auto ip = redis_connect_data.ip;
    auto port = redis_connect_data.port;
    auto passwd = redis_connect_data.passwd;
   	 
    if (sub_base) {
    	event_base_free(sub_base);
    }

	sub_base = event_base_new();
    sub_rac = redisAsyncConnect(ip.c_str(), port);
    if (sub_rac->err) {
        LOG_ERROR("error: %s\n", sub_rac->errstr);
        return;
    }
    redisAsyncSetConnectCallback(sub_rac, handle_sub_connected);
    redisAsyncSetDisconnectCallback(sub_rac, handle_sub_disconnected);
    redisLibeventAttach(sub_rac, sub_base);
    if (passwd.size()) {
    	std::string auth_cmd = "AUTH " + passwd;
    	redisAsyncCommand(sub_rac, 
    		handle_auth,
    		this, passwd.c_str());
    }
}

void RedisTool::psubscribe(const std::string& channel, std::function<void(std::string&, std::string&)> callback)
{
	// callback_list[channel] = callback;
	std::string cmd = "PSUBSCRIBE " + channel;
	auto callback_ptr = new std::function<void(std::string&, std::string&)>(callback);
	redisAsyncCommand(sub_rac, handle_psub, callback_ptr, cmd.c_str());
}

void RedisTool::handle_psub(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	if (!reply) {
		if (c->err) {
			LOG_ERROR("sub:%s\n", c->errstr);
			return ;
		}
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		LOG_ERROR("reply_error:%s", reply->str);
		return;
	}

	// RedisTool* rt = (RedisTool*)privdata;
	std::function<void(std::string&, std::string&)>* callback = (std::function<void(std::string&, std::string&)>*)privdata;
	if (reply->type == REDIS_REPLY_ARRAY) {
		std::string channel;
		std::string data;
        for (int j = 0; j < reply->elements; j++) {
        	if (j%3 == 0 && reply->element[j]->type == REDIS_REPLY_STRING) {
        		if (std::strncmp(reply->element[j]->str, "message", sizeof("message")) != 0) {
        			j += 2;
        			continue;
        		}
        	} 
        	if (j%3 == 1 && reply->element[j]->type == REDIS_REPLY_STRING) {
        		channel = std::string(reply->element[j]->str, reply->element[j]->len);
        	}
        	if (j%3 == 2 && reply->element[j]->type == REDIS_REPLY_STRING) {
				// break;
				data = std::string(reply->element[j]->str, reply->element[j]->len);
				(*callback)(channel, data);
				// if (rt->callback_list[channel]) {
				// 	rt->callback_list[channel](data);
				// }
        	}
        }
    } 	
}

void RedisTool::handle_auth(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	RedisTool* rt = (RedisTool*)privdata;
	if (!rt) return;
	if (!reply) {
		if (c->err) {
			rt->auth_failure = true;
			LOG_ERROR("auth:%s\n", c->errstr);
			return;
		}
	}

	if (reply->type == REDIS_REPLY_ERROR) {
		rt->auth_failure = true;
		LOG_ERROR("auth:%s\n", reply->str);
		return;
	}	
}

void RedisTool::redis_tool_tick()
{
	event_base_loop(sub_base, EVLOOP_NONBLOCK);
	event_base_loop(cmd_base, EVLOOP_NONBLOCK);
	if (sub_connect_failure) {
		// std::this_thread::sleep_for(1s);
		// event_base_free(sub_base);
		// sub_base = event_base_new();
		sub_connect_to_redis();
		sub_connect_failure = false;
	}

	if (cmd_connect_failure) {
		// std::this_thread::sleep_for(1s);
		// event_base_free(cmd_base);
		// cmd_base = event_base_new();
		cmd_connect_to_redis();		
		cmd_connect_failure = false;
	}	
}

void RedisTool::cmd_connect_to_redis()
{
    auto ip = redis_connect_data.ip;
    auto port = redis_connect_data.port;
    auto passwd = redis_connect_data.passwd;
   	 
    if (cmd_base) {
    	event_base_free(cmd_base);
    }

	cmd_base = event_base_new();
    cmd_rac = redisAsyncConnect(ip.c_str(), port);
    if (cmd_rac->err) {
        LOG_ERROR("error: %s\n", cmd_rac->errstr);
        return;
    }
    redisAsyncSetConnectCallback(cmd_rac, handle_cmd_connected);
    redisAsyncSetDisconnectCallback(cmd_rac, handle_cmd_disconnected);
    redisLibeventAttach(cmd_rac, cmd_base);
    if (passwd.size()) {
    	std::string auth_cmd = "AUTH " + passwd;
    	redisAsyncCommand(cmd_rac, 
    		handle_auth,
    		this, passwd.c_str());
    }	
}

void RedisTool::handle_cmd_connected(const redisAsyncContext *c, int status)
{
	LOG("redis connect:%d\n", status);
	if (status == REDIS_ERR) {
		// std::this_thread::sleep_for(1s);
		cmd_connect_failure = true;
	}
}

void RedisTool::handle_cmd_disconnected(const redisAsyncContext *c, int status)
{
	LOG_ERROR("disconnected:%s\n", c->errstr);	
	cmd_connect_failure = true;
}

void RedisTool::publish(const std::string& channel, std::string& data)
{
	std::string pub_cmd = "PUBLISH " + channel + " %b";
	redisAsyncCommand(cmd_rac, nullptr, nullptr, pub_cmd.c_str(), data.data(), data.size());	
}

bool RedisTool::sub_connect_failure = false;
bool RedisTool::cmd_connect_failure = false;
// bool RedisTool::auth_failure = false;
RedisTool::RedisTool()
{}

RedisTool::~RedisTool()
{}
