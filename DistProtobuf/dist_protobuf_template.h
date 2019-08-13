#ifndef _DMY_DIST_PROTOBUF_TEMPLATE_H_
#define _DMY_DIST_PROTOBUF_TEMPLATE_H_
#include "redis_tool.h"
#include "dmy_common/dmy_common.h"
#include <google/protobuf/message.h>
#include <sstream>
using namespace dmy_redis_tool;
using namespace google;
using namespace protobuf;
using namespace dmy_common;
namespace dmy_dist_protobuf
{
template <typename PROTO>
class DistProtobufTemplate;

template <typename PROTO>
class MutableData
{
public:
	MutableData(DistProtobufTemplate<PROTO>& data):_data(data)
	{

	}

	PROTO& data()
	{
		return _data._mutable_msg;
	}

	~MutableData()
	{
		_data.apply_change();
	}
private:
	DistProtobufTemplate<PROTO>& _data;
};

template <typename PROTO>
class DistProtobufTemplate
{
friend class MutableData<PROTO>;
public:
	DistProtobufTemplate(uint64_t id)
	{
		PROTO msg;
		channel = msg.GetTypeName();
		this->id = id;
	}
	DistProtobufTemplate()
	{
		PROTO msg;
		channel = msg.GetTypeName();
		this->id = 0;
	}
	~DistProtobufTemplate(){}

	bool is_init()
	{
		return _is_init;		
	}

	void init_data(PROTO& msg)
	{
		_is_init = true;
		_mutable_msg.CopyFrom(msg);
		// _msg.CopyFrom(msg);
		// sync_to_redis(true);
		apply_change();
	}

	const PROTO& data()
	{
		return _msg;
	}

	MutableData<PROTO> mutable_data()
	{
		return MutableData<PROTO>(*this);
	}

	void apply_change()
	{
		auto reflection = _mutable_msg.GetReflection();
		std::vector<const FieldDescriptor*> field_list;
		reflection->ListFields(_mutable_msg, &field_list);
		if (field_list.size() == 0) return;
		for (const auto it : field_list) {
			if (it->is_repeated()) {
				//对应处理一下，现在懒得做了
			} else {
				if (it->type() == FieldDescriptor::TYPE_INT64) {
					reflection->SetInt64(&_msg, it, reflection->GetInt64(_mutable_msg, it));
					modified_field[it->name()] = std::to_string(reflection->GetInt64(_mutable_msg, it));
				} else if (it->type() == FieldDescriptor::TYPE_INT32) {
					reflection->SetInt32(&_msg, it, reflection->GetInt32(_mutable_msg, it));
					modified_field[it->name()] = std::to_string(reflection->GetInt32(_mutable_msg, it));
				} else if (it->type() == FieldDescriptor::TYPE_UINT32) {
					reflection->SetUInt32(&_msg, it, reflection->GetUInt32(_mutable_msg, it));
					modified_field[it->name()] = std::to_string(reflection->GetUInt32(_mutable_msg, it));
				} else if (it->type() == FieldDescriptor::TYPE_UINT64) {
					reflection->SetUInt64(&_msg, it, reflection->GetUInt64(_mutable_msg, it));
					modified_field[it->name()] = std::to_string(reflection->GetUInt64(_mutable_msg, it));
				} else if (it->type() == FieldDescriptor::TYPE_BOOL) {
					reflection->SetBool(&_msg, it, reflection->GetBool(_mutable_msg, it));
					modified_field[it->name()] = std::to_string(reflection->GetBool(_mutable_msg, it));
				} else if (it->type() == FieldDescriptor::TYPE_STRING) {
					reflection->SetString(&_msg, it, reflection->GetString(_mutable_msg, it));
					modified_field[it->name()] = reflection->GetString(_mutable_msg, it);
				}
			}
		}
	}

	std::string get_hmset_cmd()
	{
		// std::string ret;
		std::stringstream ss;
		ss<<"HMSET "<<channel<<":"<<std::to_string(id);
		for (auto& [field, data] : modified_field) {
			ss<<" "<<field<<" "<<data;
		}
		return ss.str();
	}

	std::string get_publish_cmd()
	{
		std::stringstream ss;
		ss<<"PUBLISH "<<channel<<" "<<std::to_string(id)<<"|";
		for (auto it = modified_field.begin(); it != modified_field.end();) {
			ss<<it->first;
			++it;
			if (it != modified_field.end()) {
				ss<<"|";
			} else {
				break;
			}
		}
		return ss.str();
	}

	void sync_to_redis(RedisTool& rt)
	{
		// if (!_need_sync) return;
		if (modified_field.size() == 0) return;
		rt.exec_cmd("MULTI");
		rt.exec_cmd(get_hmset_cmd());
		rt.exec_cmd(get_publish_cmd());
		rt.exec_cmd("EXEC");
		modified_field.clear();
	}

	void set_data(std::unordered_map<std::string, std::string>& data_map)
	{
		for (auto& it : modified_field) {
			data_map.erase(it.first);
		}
		// for (auto& [field, value] : data_map) {
			// modified_field.erase(field);
		// }
		auto reflection = _msg.GetReflection();
		// std::vector<const FieldDescriptor*> field_list;
		auto descriptor = _msg.GetDescriptor();

		// reflection->ListFields(_msg, &field_list);
		if (field_list.size() == 0) return;
		for (int i = 0; i < descriptor->field_count(); ++i) {
			auto it = descriptor->field(i);
			if (data_map.count(it->name()) == 0) continue;
			std::string& value = data_map[it->name()];
			if (it->type() == FieldDescriptor::TYPE_INT64) {
				reflection->SetInt64(&_msg, it, std::atoll(value.c_str()));
				// modified_field[it->name()] = std::to_string(it->GetInt64());
			} else if (it->type() == FieldDescriptor::TYPE_INT32) {
				reflection->SetInt32(&_msg, it, std::atoi(value.c_str()));
				// modified_field[it->name()] = std::to_string(it->GetInt32());
			} else if (it->type() == FieldDescriptor::TYPE_UINT32) {
				reflection->SetUInt32(&_msg, it, (uint32_t)std::atoi(value.c_str()));
				// modified_field[it->name()] = std::to_string(it->GetUInt32());
			} else if (it->type() == FieldDescriptor::TYPE_UINT64) {
				reflection->SetUInt64(&_msg, it, (uint64_t)std::atoll(value.c_str()));
				// modified_field[it->name()] = std::to_string(it->GetUInt64());
			} else if (it->type() == FieldDescriptor::TYPE_BOOL) {
				reflection->SetBool(&_msg, it, std::atoi(value.c_str()) == 0 ? false:true);
				// modified_field[it->name()] = std::to_string(it->GetBool());
			} else if (it->type() == FieldDescriptor::TYPE_STRING) {
				reflection->SetString(&_msg, it, value);
				// modified_field[it->name()] = it->GetString();
			}
		}		
	}
private:
	PROTO _msg;
	PROTO _mutable_msg;
	uint64_t id;
	std::string channel;
	bool _is_init;
	std::unordered_map<std::string, std::string> modified_field;
};






}



#endif