#ifndef _DMY_MEMORY_TEMPLATE_H_
#define _DMY_MEMORY_TEMPLATE_H_
#include "distmemory.h"
#include <functional>
namespace dmy_dist_memory
{
using namespace std::placeholders;
struct test_data
{
	test_data(const std::string& type_str, uint64_t id):type_str(type_str), id(id){}
	std::string type_str;
	uint64_t id;
	void parse(std::string* str)
	{
	}
	std::string serialize()
	{
		return "1";
	}
	std::string extra;
};

template <typename DATA>
class DistMemory;

template <typename DATA>
class MutableData
{
public:
	MutableData(DATA& data):_data(data){}
	DATA* operator->()
	{
		return &_data;
	}
	~MutableData()
	{
		DistMemoryRedisTool::set_data(_data.type_str, _data.id, _data.serialize());
	}
private:
	DATA& _data;
};

template <typename DATA>
class DistMemory
{
public:
	DistMemory(const std::string& type_str, uint64_t id):_data(type_str, id)
	{
		DistMemoryRedisTool::reg_data(_data.type_str, _data.id, std::bind(&DATA::parse, &_data, _1));	
		DistMemoryRedisTool::get_data(_data.type_str, _data.id);
	}

	template <typename... Args>
	DistMemory(const std::string& type_str, uint64_t id, Args... args):_data(type_str, id, args...)
	{
		DistMemoryRedisTool::reg_data(_data.type_str, _data.id, std::bind(&DATA::parse, &_data, _1));	
		DistMemoryRedisTool::set_data(_data.type_str, _data.id, _data.serialize());
	}

	~DistMemory()
	{
		DistMemoryRedisTool::unreg_data(_data.type_str, _data.id);
	}

	const DATA& data()
	{
		return _data;
	}

	MutableData<DATA> mutable_data()
	{
		return MutableData<DATA>(_data);
	}

private:
	DATA _data;
};

}







#endif