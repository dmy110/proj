#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_
#include "socket_data.h"
class SocketClient
{
public:
private:
	int _fd;
	// std::vector<char*> _rcv_buffer;
	char* _rcv_buffer;
	size_t _current_idx;
	size_t _max_capacity;
public:
	static void push_rcv_data(socket_data_t& socket_data);
private:	
	static std::unordered_map<int, SocketClient*> _soc_map;
	SocketClient(int fd);
};


#endif