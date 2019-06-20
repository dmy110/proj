#include "distmemory.h"
#include <chrono>
#include <cstring>
using namespace dmy_dist_memory;
using namespace std::chrono_literals;
using namespace std::placeholders;
// std::thread DistMemoryRedisTool::req_data_thread;

DistMemoryRedisToolThread DistMemoryRedisTool::tool_thread;

void DistMemoryRedisTool::init_tool(const std::string ip, uint32_t port, std::string passwd)
{
	redis_connect_data = {
		ip,
		port,
		passwd
	};

	tool_thread.start_thread();
}

void DistMemoryRedisToolThread::handle_sub(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	if (!reply) {
		if (c->err) {
			LOG_ERROR("sub:%s\n", c->errstr);
			return ;
		}
	}
	if (!privdata) return;
	if (reply->type == REDIS_REPLY_ERROR) {
		LOG_ERROR("reply_error:%s", reply->str);
		return;
	}

	if (reply->type == REDIS_REPLY_ARRAY) {
		std::unique_lock wlock(DistMemoryRedisTool::tool_thread.cmd_list.command_lock);
        for (int j = 0; j < reply->elements; j++) {
        	if (j%3 == 0) {
        		if (std::strncmp(reply->element[j]->str, "message", sizeof("message")) != 0) break;
        	} 
        	if (j%3 == 1) {
        	}
        	if (j%3 == 2) {
				redis_tool_key_t* get_key = new redis_tool_key_t(*(redis_tool_key_t*)privdata);
				DistMemoryRedisTool::tool_thread.cmd_list.command.push_back({get_key, rtct_t::GET});
				break;
        	}
        }
    } 
}

void DistMemoryRedisToolThread::handle_get(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	if (!reply) {
		if (c->err) {
			printf("get:%s\n", c->errstr);
			return ;
		}
	}

	if (!privdata) return;
	
	std::unique_lock wlock(DistMemoryRedisTool::tool_thread.cmd_list.reply_lock);
	if (reply->type == REDIS_REPLY_STRING) {
		std::string* value = new std::string(reply->str, reply->len);
		DistMemoryRedisTool::tool_thread.cmd_list.reply.push_back({(redis_tool_key_t*)privdata, value});
    }
}

std::unordered_map<std::string, 
		std::unordered_map<uint64_t, 
		std::function<void(std::string*)>>> DistMemoryRedisTool::parse_func_map;

void DistMemoryRedisTool::reg_data(std::string& key, uint64_t id, 
		std::function<void(std::string*)> parse_func)
{
	redis_tool_key_t* rtk = new redis_tool_key_t(key, id);
	parse_func_map[key][id] = parse_func;
	std::unique_lock wlock(tool_thread.sub_list.command_lock);
	tool_thread.sub_list.command.push_back({rtk, rtct_t::SUB});
}

void DistMemoryRedisTool::tick()
{
	std::list<redis_tool_value_t> backup;
	{
		std::shared_lock rlock(tool_thread.cmd_list.reply_lock);
		backup = tool_thread.cmd_list.reply;
		tool_thread.cmd_list.reply.clear();
	}
	for (auto& it : backup) {
		auto key_data = it.key;
		auto value = it.value;
		if (parse_func_map.count(key_data->key) && parse_func_map[key_data->key].count(key_data->id)
			&& parse_func_map[key_data->key][key_data->id]) {
			parse_func_map[key_data->key][key_data->id](value);
			delete value;
			delete key_data;
		}
	}

}

std::string DistMemoryRedisTool::get_key(redis_tool_key_t& data)
{
	return std::string(data.key.data(), data.key.length()) + 
	    				":" + std::to_string(data.id);
}

void DistMemoryRedisTool::set_data(std::string& key_str, uint64_t id, std::string value)
{
	redis_tool_command_t cmd;
	cmd.key = new redis_tool_key_t({key_str, id});
	cmd.command_type = rtct_t::PUB;
	cmd.value = new std::string(value);
	std::unique_lock wlock(tool_thread.cmd_list.command_lock);
	tool_thread.cmd_list.command.push_back(cmd);	
}

void DistMemoryRedisTool::unreg_data(std::string& key_str, uint64_t id)
{
	if (parse_func_map.count(key_str)) {
		parse_func_map[key_str].erase(id);
	}
	redis_tool_command_t cmd;
	cmd.key = new redis_tool_key_t({key_str, id});
	cmd.command_type = rtct_t::UNSUB;
	std::unique_lock wlock(tool_thread.cmd_list.command_lock);
	tool_thread.cmd_list.command.push_back(cmd);	
}

void DistMemoryRedisTool::get_data(std::string& key_str, uint64_t id)
{
	redis_tool_command_t cmd;
	cmd.key = new redis_tool_key_t({key_str, id});
	cmd.command_type = rtct_t::GET;
	std::unique_lock wlock(tool_thread.cmd_list.command_lock);
	tool_thread.cmd_list.command.push_back(cmd);
}

DistMemoryRedisTool::redis_connect_data_t DistMemoryRedisTool::redis_connect_data;

void DistMemoryRedisToolThread::handle_sub_connected(const redisAsyncContext *c, int status)
{
	LOG("redis connect:%d\n", status);
	if (status == REDIS_ERR) {
		// std::this_thread::sleep_for(1s);
		DistMemoryRedisTool::tool_thread.sub_connect_failure = true;

	}
}

void DistMemoryRedisToolThread::handle_cmd_connected(const redisAsyncContext *c, int status)
{
	LOG("redis connect:%d\n", status);
	if (status == REDIS_ERR) {
		// std::this_thread::sleep_for(1s);
		DistMemoryRedisTool::tool_thread.cmd_connect_failure = true;
	}
}

void DistMemoryRedisToolThread::cmd_connect_to_redis()
{
    auto ip = DistMemoryRedisTool::redis_connect_data.ip;
    auto port = DistMemoryRedisTool::redis_connect_data.port;
    auto passwd = DistMemoryRedisTool::redis_connect_data.passwd;
   	 
    if (cmd_base) {
    	event_base_free(cmd_base);
    }

	cmd_base = event_base_new();
    cmd_rac = redisAsyncConnect(ip.c_str(), port);
    if (cmd_rac->err) {
        LOG_ERROR("error: %s\n", cmd_rac->errstr);
        stop_thread = true;
        return;
    }
    redisAsyncSetConnectCallback(cmd_rac, &DistMemoryRedisToolThread::handle_cmd_connected);
    redisAsyncSetDisconnectCallback(cmd_rac, &DistMemoryRedisToolThread::handle_cmd_disconnected);
    redisLibeventAttach(cmd_rac, cmd_base);
    if (passwd.size()) {
    	std::string auth_cmd = "AUTH " + passwd;
    	redisAsyncCommand(cmd_rac, 
    		&DistMemoryRedisToolThread::handle_auth,
    		nullptr, passwd.c_str());
    }
}

void DistMemoryRedisToolThread::sub_connect_to_redis()
{
    auto ip = DistMemoryRedisTool::redis_connect_data.ip;
    auto port = DistMemoryRedisTool::redis_connect_data.port;
    auto passwd = DistMemoryRedisTool::redis_connect_data.passwd;
   	if (sub_base) {
   		event_base_free(sub_base);
   	} 
	sub_base = event_base_new();
    sub_rac = redisAsyncConnect(ip.c_str(), port);
    if (sub_rac->err) {
    	// data->need_reconnect = true;
    	stop_thread = true;
        LOG_ERROR("error: %s\n", sub_rac->errstr);
        return;
    }
    redisAsyncSetConnectCallback(sub_rac, &DistMemoryRedisToolThread::handle_sub_connected);
    redisAsyncSetDisconnectCallback(sub_rac, &DistMemoryRedisToolThread::handle_sub_disconnected);
    redisLibeventAttach(sub_rac, sub_base);
    if (passwd.size()) {
    	std::string auth_cmd = "AUTH " + passwd;
    	redisAsyncCommand(sub_rac, 
    		&DistMemoryRedisToolThread::handle_auth,
    		nullptr, passwd.c_str());
    }
}

void DistMemoryRedisToolThread::connect_to_redis()
{
	sub_connect_to_redis();
	cmd_connect_to_redis();
}

void DistMemoryRedisToolThread::handle_sub_disconnected(const redisAsyncContext *c, int status)
{
	LOG_ERROR("disconnected:%s\n", c->errstr);	
	DistMemoryRedisTool::tool_thread.sub_connect_failure = true;
}

void DistMemoryRedisToolThread::handle_cmd_disconnected(const redisAsyncContext *c, int status)
{
	LOG_ERROR("disconnected:%s\n", c->errstr);	
	DistMemoryRedisTool::tool_thread.cmd_connect_failure = true;
}

void DistMemoryRedisToolThread::handle_process()
{
	event_base_loop(sub_base, EVLOOP_NONBLOCK);
	event_base_loop(cmd_base, EVLOOP_NONBLOCK);
	sub_process();
	cmd_process();
	std::this_thread::sleep_for(1ms);
	if (sub_connect_failure) {
		std::this_thread::sleep_for(1s);
		// event_base_free(sub_base);
		// sub_base = event_base_new();
		sub_connect_to_redis();
		sub_connect_failure = false;
	}

	if (cmd_connect_failure) {
		std::this_thread::sleep_for(1s);
		// event_base_free(cmd_base);
		// cmd_base = event_base_new();
		cmd_connect_to_redis();		
		cmd_connect_failure = false;
	}
}

void DistMemoryRedisToolThread::handle_auth(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	if (!reply) {
		if (c->err) {
			DistMemoryRedisTool::tool_thread.auth_failure = true;
			LOG_ERROR("auth:%s\n", c->errstr);
			return;
		}
	}

	if (reply->type == REDIS_REPLY_ERROR) {
		DistMemoryRedisTool::tool_thread.auth_failure = true;
		LOG_ERROR("auth:%s\n", reply->str);
		return;
	}	
}

void DistMemoryRedisToolThread::start_thread()
{
	DistMemoryRedisToolThread* data = this;
	thread_obj = std::thread([data](){
	    signal(SIGPIPE, SIG_IGN);
	    data->connect_to_redis();
	    while (!data->stop_thread && !data->auth_failure) {
	    	data->handle_process();
	    }
    });
}

void DistMemoryRedisToolThread::sub_process()
{
	redisAsyncContext* c = sub_rac;
	std::shared_lock rlock(DistMemoryRedisTool::tool_thread.sub_list.command_lock);
	while (DistMemoryRedisTool::tool_thread.sub_list.command.size()) {
		auto key = DistMemoryRedisTool::tool_thread.sub_list.command.front().key;
		std::string cmd = "SUBSCRIBE " + DistMemoryRedisTool::get_key(*key);
		redisAsyncCommand(c, DistMemoryRedisToolThread::handle_sub, key, cmd.c_str());
		LOG("SUBSCRIBE\n");
		DistMemoryRedisTool::tool_thread.sub_list.command.pop_front();
	}	
}

DistMemoryRedisToolThread::DistMemoryRedisToolThread()
{
	auth_failure = false;
	stop_thread = false;
    // sub_base = event_base_new();
    // cmd_base = event_base_new();
}


void DistMemoryRedisToolThread::cmd_process()
{
	redisAsyncContext* c = cmd_rac;
   	std::shared_lock rlock(DistMemoryRedisTool::tool_thread.cmd_list.command_lock);
	while (DistMemoryRedisTool::tool_thread.cmd_list.command.size()) {
		auto cmd = DistMemoryRedisTool::tool_thread.cmd_list.command.front();
		if (cmd.command_type == rtct_t::GET) {
			auto data = cmd.key;
			std::string cmd = "GET " + DistMemoryRedisTool::get_key(*data);
			redisAsyncCommand(c, DistMemoryRedisToolThread::handle_get, data, cmd.c_str());
			// DistMemoryRedisTool::tool_thread.cmd_list.command.pop_front();
			LOG("GET\n");
		} 
		else if (cmd.command_type == rtct_t::PUB) {
			auto data = cmd.key;

			std::string set_cmd = "SET " + DistMemoryRedisTool::get_key(*data) + " " + *(cmd.value);
			redisAsyncCommand(c, nullptr, nullptr, set_cmd.c_str());

			std::string pub_cmd = "PUBLISH " + DistMemoryRedisTool::get_key(*data) + " ' '";
			redisAsyncCommand(c, nullptr, nullptr, pub_cmd.c_str());
			
			delete cmd.value;
			delete cmd.key;

			// DistMemoryRedisTool::tool_thread.cmd_list.command.pop_front();	  
			LOG("SET && PUBLISH %s, %s\n", set_cmd.c_str(), pub_cmd.c_str());				
		} 
		else if (cmd.command_type == rtct_t::UNSUB) {
			auto data = cmd.key;
			std::string unsub_cmd = "UNSUBSCRIBE " + DistMemoryRedisTool::get_key(*data);
			redisAsyncCommand(c, nullptr, nullptr, unsub_cmd.c_str());
			delete cmd.key;
		}
		DistMemoryRedisTool::tool_thread.cmd_list.command.pop_front();
	}
}
