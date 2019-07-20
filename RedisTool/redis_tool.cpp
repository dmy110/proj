#include "redis_tool.h"
#include "dmy_common.h"
#include <signal.h>
#include <cstring>
#include <sstream>
using namespace dmy_redis_tool;
using namespace dmy_common;
void RedisTool::init_redis_tool(const std::string& ip, uint32_t port, const std::string& passwd)
{
	redis_connect_data = {
		ip,
		port,
		passwd
	};

	// signal(SIGPIPE, SIG_IGN)
	struct sigaction pipe_action;
	pipe_action.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &pipe_action, nullptr);

	sub_connect_to_redis();
	cmd_connect_to_redis();
}


void RedisTool::handle_sub_connected(const redisAsyncContext *c, int status)
{
	LOG("sub redis connect:%d\n", status);
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
    for (auto& it : sub_callback) {
    	std::string cmd = "SUBSCRIBE " + it.second->channel;
    	redisAsyncCommand(sub_rac, handle_sub, it.second, cmd.c_str());
    }
    sub_callback.clear();
}

void RedisTool::exec_subscribe_cmd(const std::string& channel, std::function<void(redisReply*)> cb)
{
	std::string exec_cmd = "SUBSCRIBE " + channel;
	auto cb_obj = new sub_callback_t({channel, cb});
	sub_callback[channel] = cb_obj;

	redisAsyncCommand(sub_rac, handle_sub, cb_obj, exec_cmd.c_str());
}

void RedisTool::exec_subscribe_cmd(const std::string& channel, std::function<void(std::string)> cb)
{
	exec_subscribe_cmd(channel, [cb](redisReply* reply){
		for (int j = 0; j < reply->elements;) {
    		if (std::strncmp(reply->element[j]->str, "message", sizeof("message")) == 0) {
    			if (j + 2 < reply->elements) {
	    			cb(std::string(reply->element[j + 2]->str, reply->element[j + 2]->len));
    			}
    		}
			j += 3;
        }
	});
}

void RedisTool::handle_sub(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	DMY_CHECK_REDIS_ERROR(c, reply);

	// std::function<void(std::string&, std::string&)>* callback = (std::function<void(std::string&, std::string&)>*)privdata;
	sub_callback_t* callback_obj = (sub_callback_t*)privdata;
	
    callback_obj->cb(reply);
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



void RedisTool::handle_cmd_connected(const redisAsyncContext *c, int status)
{
	LOG("cmd redis connect:%d\n", status);
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

void RedisTool::exec_get(const std::string& key, std::function<void(std::string)> cb)
{
	std::stringstream ss;
	ss<<"GET "<<key;
	exec_cmd(ss.str(), [cb](redisReply* reply){
		if (reply->type == REDIS_REPLY_INTEGER) {
			cb(std::to_string(reply->integer));
		} else if (reply->type == REDIS_REPLY_STRING) {
			cb(std::string(reply->str, reply->len));
		}
	});
}

void RedisTool::exec_cmd(const std::string& cmd, std::function<void(redisReply*)> cb)
{
	std::function<void(redisReply*)>* cb_ptr = new std::function<void(redisReply*)>(cb);
	redisAsyncCommand(cmd_rac, handle_cmd_callback, cb_ptr, cmd.c_str());
}

void RedisTool::handle_cmd_callback(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	DMY_CHECK_REDIS_ERROR(c, reply);

	// std::function<void(std::string&, std::string&)>* callback = (std::function<void(std::string&, std::string&)>*)privdata;
	// sub_callback_t* callback_obj = (sub_callback_t*)privdata;
	std::function<void(redisReply*)>* cb_ptr = (std::function<void(redisReply*)>*)privdata;
	(*cb_ptr)(reply);
	delete cb_ptr;
    // callback_obj->cb(reply);
}

void RedisTool::exec_cmd(const std::string& cmd)
{
	std::string* cmd_backup = new std::string(cmd);
	redisAsyncCommand(cmd_rac, handle_check_error_callback, cmd_backup, cmd.c_str());
}

// void RedisTool::publish(const std::string& channel, std::string& data)
// {
	// std::string pub_cmd = "PUBLISH " + channel + " %b";
	// redisAsyncCommand(cmd_rac, nullptr, nullptr, pub_cmd.c_str(), data.data(), data.size());	
// }

// void RedisTool::exec_command(const std::string& cmd)
// {
	// redisAsyncCommand(cmd_rac, nullptr, nullptr, "%b", cmd.data(), cmd.size());
// }

bool RedisTool::sub_connect_failure = false;
bool RedisTool::cmd_connect_failure = false;
// bool RedisTool::auth_failure = false;
RedisTool::RedisTool()
{}

RedisTool::~RedisTool()
{}

void RedisTool::handle_check_error_callback(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	std::string* str;
	if (privdata) {
		str = (std::string*)privdata;
	} else {
		str = new std::string("fcmd");
	}
	if (!reply) { 
		if (c->err) { 
			LOG_ERROR("cmd_error:%s %s\n", c->errstr, str->c_str());
			delete str; 
			return ; 
		} 
	} 
	if (reply->type == REDIS_REPLY_ERROR) { 
		LOG_ERROR("reply_error:%s %s", reply->str, str->c_str());
		delete str; 
		return; 
	} 
	delete str;
}
// void RedisTool::exec_command(const std::string& cmd, void* privdata, void(*callback)(redisAsyncContext *c, void *reply, void *privdata))
// {
// 	redisAsyncCommand(cmd_rac, callback, privdata, "%b", cmd.data(), cmd.size());
// }

void RedisTool::exec_cmdf(const char *format, ...)
{
	va_list ap;
    va_start(ap,format);
    redisvAsyncCommand(cmd_rac,handle_check_error_callback,nullptr,format,ap);
    va_end(ap);
}

void RedisTool::exec_hmset(const std::string& key, std::vector<std::tuple<std::string, std::string>>& fv)
{
	std::stringstream ss;
	ss << "HMSET " << key;
	for (auto& it : fv) {
		ss << " " << std::get<0>(it) << " " << std::get<1>(it);
	}
	exec_cmd(ss.str());
}

void RedisTool::exec_hmget(const std::string& key, std::vector<std::string> field, 
		std::function<void(std::unordered_map<std::string, std::string>&)> cb)
{
	std::stringstream ss;
	ss << "HMGET " << key;
	for (auto& it : field) {
		ss << " " << it;
	}
	std::function<void(redisReply*)> dec_cb = [cb, field](redisReply* reply){
		// std::vector<std::tuple<std::string, std::string>> fv;
		std::unordered_map<std::string, std::string> fv_list;
		if (reply->elements != field.size()) {
			LOG_ERROR("exec_hmget return size error:%u, %u", reply->elements, field.size());
			return;
		}

		for (int j = 0; j < reply->elements; ++j) {
			if (reply->element[j]->type == REDIS_REPLY_STRING) {
				fv_list[field[j]] = std::string(reply->element[j]->str, reply->element[j]->len);
				// fv.push_back({field[j], reply->element[j]->str});
			} else if (reply->element[j]->type == REDIS_REPLY_INTEGER) {
				fv_list[field[j]] = std::to_string(reply->element[j]->integer);
				// fv.push_back({field[j], std::to_string(reply->element[j]->integer)});
			} else {
				LOG_ERROR("exec_hmget return type error:%u\n", reply->element[j]->type);
				return;
			}
        }
        cb(fv_list);
	};
	exec_cmd(ss.str(), dec_cb);
}