#ifndef _SERVER_CFG_H_
#define _SERVER_CFG_H_

#include <unordered_map>
class ServerCfg
{
public:
	template <typename T>
	const T& get_cfg(const char* cfg_name);
	
	static void set_cfg(const char* cfg_name, const char* cfg_data);
private:
	std::unordered_map<std::string, std::string> _cfg; 
	std::unordered_map<std::string, int> _int_cfg; 
	ServerCfg();
	static ServerCfg _cfg_obj;
};

template <>
const std::string& ServerCfg::get_cfg(const char* cfg_name)
{
	return _cfg[std::string(cfg_name)];
}

template <>
const int& ServerCfg::get_cfg(const char* cfg_name)
{
	std::string key(cfg_name);
	if (_int_cfg.count(key)) {
		return _int_cfg[key];
	}
	_int_cfg[key] = std::atoi(_cfg[key]);
	return _int_cfg[key];
}


#endif