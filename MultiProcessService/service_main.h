// #ifndef _DMY_SERVICE_MAIN_H_
// #define _DMY_SERVICE_MAIN_H_

// #include <unordered_map>
// #include <chrono>
// #include <functional>
// #include "timer.h"
// namespace dmy_multiprocess_serivce 
// {
// // void start_service();

// // extern "C" {
// // 	void* dl = nullptr;
// // 	char *error;
// // 	void (*service_tick)();
// // 	void (*service_init)();
// // 	void (*service_end)();
// // }

// // class ServiceMain
// // {
// // public:
	
// // };
// enum service_cmd_type_t
// {
// 	SERVICE_CMD_STOP = 1,
// };
// struct service_cmd_t
// {
// 	uint32_t cmd_type;
// };

// struct service_data_t
// {
// 	std::string so_file_path;
// 	std::string service_name;	
// 	int service_id = 0;
// 	int config_version = 0;
// 	int service_version = 0;
// };

// struct service_group_data_t
// {
// 	int pid = 0;
// 	int pipe_write;
// 	bool need_restart = true;
// 	// int service_group_id = 0;
// 	std::unordered_map<uint32_t, service_data_t> service_cfg_map;
// };


// class ServiceMain
// {
// public:
// 	static void start_main_service();
// 	static void handle_service_shutdown(int signum);
// 	static bool stop_service;
// private:
// 	void init_serive_cfg();
// 	std::unordered_map<uint32_t, service_group_data_t> service_group_map;
// 	void run();
// };












// }
// #endif