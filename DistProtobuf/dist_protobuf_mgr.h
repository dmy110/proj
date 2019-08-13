#ifndef _DMY_DIST_PROTOBUF_MGR_H_
#define _DMY_DIST_PROTOBUF_MGR_H_
#include "redis_tool.h"
#include "dist_protobuf_template.h"
#include "dmy_common/dmy_common.h"
#include <regex>
using namespace dmy_common;
using namespace dmy_redis_tool;
namespace dmy_dist_protobuf
{
class DistProtobufMgr
{
public:
	DistProtobufMgr(RedisTool& rt):_rt(rt){}
	~DistProtobufMgr(){}
	void dpm_tick()
	{
		for (auto& it : dist_protobuf_tick) {
			it();
		}
	}
private:	
	RedisTool& _rt;
	
public:
	template <typename PROTO>
	DistProtobufTemplate<PROTO>& get_dist_protobuf(uint64_t id)
	{
		static std::unordered_map<uint64_t, DistProtobufTemplate<PROTO>*> data_map;
		static bool has_init;
		static PROTO msg;			

		if (has_init == false) {
			has_init = true;

			dist_protobuf_tick.push_back([this](){
				for (auto& it : data_map) {
					it.second->sync_to_redis(_rt);
				}
			});
			
			_rt.exec_subscribe_cmd(msg.GetTypeName(), [this](std::string data){
				std::regex regex("\\|");
				std::sregex_token_iterator it{data.begin(), data.end(), regex, -1};
				std::vector<std::string> field_list{it, {}};

				if (field_list.size() <= 1) return;
				uint64_t id = std::atoll(field_list[0].c_str());
				if (data_map.count(id) == 0) return;

				field_list.erase(field_list.begin());
				_rt.exec_hmget(msg.GetTypeName() + ":" + std::to_string(id), field_list, [this, id, field_list]
					(std::unordered_map<std::string, std::string>& fv_list){
					if (data_map.count(id) == 0) return;

					data_map[id]->set_data(fv_list);
				});
			});
		}

		if (data_map.count(id) == 0) {
			data_map[id] = new DistProtobufTemplate<PROTO>();
		}


		return *data_map[id];
	}
private:
	std::list<std::function<void()>> dist_protobuf_tick;

};

// template <typename PROTO>
// std::unordered_map<uint64_t, DistProtobufTemplate<PROTO>*> DistProtobufMgr::DistProtobufTemplate<PROTO>::data_map;

// template <typename PROTO>
// bool DistProtobufMgr::DistProtobufTemplate<PROTO>::has_init;

// template <typename PROTO>
// PROTO DistProtobufMgr::DistProtobufTemplate<PROTO>::msg;	



}

#endif