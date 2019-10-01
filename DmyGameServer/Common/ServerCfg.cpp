#include "ServerCfg.h"

void ServerCfg::set_cfg(const char* cfg_name, const char* cfg_data)
{
	_cfg[std::string(cfg_name)] = std::string(cfg_data);
}