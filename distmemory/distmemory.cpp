#include "distmemory.h"
#include <chrono>
#include <cstring>
using namespace dmy_dist_memory;

std::thread DistMemoryRedisTool::sub_thread;
std::thread DistMemoryRedisTool::req_data_thread;
void DistMemoryRedisTool::init_tool()
{
	using namespace std::chrono_literals;
	sub_thread = std::thread([](){
	    signal(SIGPIPE, SIG_IGN);
	    struct event_base *base = event_base_new();

	    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
	    if (c->err) {
	        printf("error: %s\n", c->errstr);
	        return;
	    }

	    redisLibeventAttach(c, base);
	    // redisAsyncCommand(c, onMessage, NULL, "SUBSCRIBE testtopic");
	    while (true) {
	    	auto ret = event_base_loop(base, EVLOOP_NONBLOCK);
	    	if (ret == -1) {
	    		printf("event error");
	    		break;
	    	}
	    	if (ret == 1) {
	    		std::this_thread::sleep_for(1ms);
	    	}
       		std::shared_lock rlock(DistMemoryRedisTool::sub_list.command_lock);
    		while (DistMemoryRedisTool::sub_list.command.size()) {
    			auto data = DistMemoryRedisTool::sub_list.command.front().key;
    			std::string cmd = "SUBSCRIBE " + DistMemoryRedisTool::get_key(*data);
    			redisAsyncCommand(c, DistMemoryRedisTool::handle_pub, data, cmd.c_str());
    			DistMemoryRedisTool::sub_list.command.pop_front();
    			printf("SUBSCRIBE\n");
    		}
	    }	
	   
	});		

	req_data_thread = std::thread([](){
	    signal(SIGPIPE, SIG_IGN);
	    struct event_base *base = event_base_new();

	    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
	    if (c->err) {
	        printf("error: %s\n", c->errstr);
	        return;
	    }

	    redisLibeventAttach(c, base);
	    // redisAsyncCommand(c, onMessage, NULL, "SUBSCRIBE testtopic");
	    while (true) {
	    	auto ret = event_base_loop(base, EVLOOP_NONBLOCK);
	    	if (ret == -1) {
	    		printf("event error");
	    		break;
	    	}
	    	if (ret == 1) {
	    		std::this_thread::sleep_for(1ms);
	    	}

       		std::shared_lock rlock(DistMemoryRedisTool::req_data_list.command_lock);
    		while (DistMemoryRedisTool::req_data_list.command.size()) {
    			auto cmd = DistMemoryRedisTool::req_data_list.command.front();
    			if (cmd.command_type == rtct_t::GET) {
    				auto data = cmd.key;
	    			std::string cmd = "GET " + DistMemoryRedisTool::get_key(*data);
	    			redisAsyncCommand(c, DistMemoryRedisTool::handle_req, data, cmd.c_str());
	    			DistMemoryRedisTool::req_data_list.command.pop_front();
	    			printf("GET\n");
	    		} 
	  			else if (cmd.command_type == rtct_t::PUB) {
    				auto data = cmd.key;

    				std::string set_cmd = "SET " + DistMemoryRedisTool::get_key(*data) + " " + *(cmd.value);
    				redisAsyncCommand(c, DistMemoryRedisTool::handle_req, nullptr, set_cmd.c_str());

	    			std::string pub_cmd = "PUBLISH " + DistMemoryRedisTool::get_key(*data) + " ' '";
	    			redisAsyncCommand(c, DistMemoryRedisTool::handle_req, nullptr, pub_cmd.c_str());
	    			
	    			delete cmd.value;
	    			delete cmd.key;

	    			DistMemoryRedisTool::req_data_list.command.pop_front();	  
	    			printf("SET && PUBLISH\n");				
	  			}
    		}
	    }	
	});	
}

void DistMemoryRedisTool::handle_pub(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	if (!reply) {
		if (c->err) {
			printf("%s\n", c->errstr);
			return ;
		}
	}
	if (!privdata) return;
	req_data_list.command.push_back({(redis_tool_key_t*)privdata, rtct_t::GET});
	// if (reply->type == REDIS_REPLY_ARRAY) {
	// 	// std::unique_lock(redis_command_list.reply_lock);
	// 	// std::string key
	// 	std::unique_lock wlock(req_data_list.command_lock);
 //        for (int j = 0; j < reply->elements; j++) {
 //        	if (j%3 == 0) {
 //        		if (std::strncmp(reply->element[j]->str, "message", sizeof("message")) != 0) continue;
 //        	} 
 //        	if (j%3 == 1) {
 //        	}
 //        	if (j%3 == 2) {
 //        		req_data_list.command.push_back({(redis_tool_key_t*)privdata, rtct_t::GET});
 //        	}
 //        }
 //    } 
}

void DistMemoryRedisTool::handle_req(redisAsyncContext *c, void *reply_ptr, void *privdata)
{
	redisReply* reply = static_cast<redisReply*>(reply_ptr);
	if (!reply) {
		if (c->err) {
			printf("%s\n", c->errstr);
			return ;
		}
	}

	if (!privdata) return;
	
	std::unique_lock wlock(req_data_list.reply_lock);
	if (reply->type == REDIS_REPLY_STRING) {
		std::string* value = new std::string(reply->str, reply->len);
		req_data_list.reply.push_back({(redis_tool_key_t*)privdata, value});
    }
}
std::unordered_map<std::string, 
		std::unordered_map<uint64_t, 
		std::function<void(std::string*)>>> DistMemoryRedisTool::parse_func_map;

redis_tool_list_t DistMemoryRedisTool::sub_list;
redis_tool_list_t DistMemoryRedisTool::req_data_list;

void DistMemoryRedisTool::reg_data(std::string& key, uint64_t id, 
		std::function<void(std::string*)> parse_func)
{
	redis_tool_key_t* rtk = new redis_tool_key_t({key, id});
	parse_func_map[key][id] = parse_func;
	std::unique_lock wlock(sub_list.command_lock);
	sub_list.command.push_back({rtk, rtct_t::SUB});
}

void DistMemoryRedisTool::tick()
{
	std::list<redis_tool_value_t> backup;
	{
		std::shared_lock rlock(req_data_list.reply_lock);
		backup = req_data_list.reply;
		req_data_list.reply.clear();
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
	std::unique_lock wlock(req_data_list.command_lock);
	req_data_list.command.push_back(cmd);	
}

void DistMemoryRedisTool::unreg_data(std::string& key_str, uint64_t id)
{
	if (parse_func_map.count(key_str)) {
		parse_func_map[key_str].erase(id);
	}
}
