#include "SocketClient.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

SocketClient::SocketClient(int fd):_fd(fd)
{
	// _rcv_buffer.reserve(ServerCfg::get_cfg<int>("rcv_buffer_section"));
	size_t capacity = ServerCfg::get_cfg<int>("rcv_buffer_section");
	_rcv_buffer = (char*)malloc(capacity);
	_current_idx = 0;
	_max_capacity = capacity;
}

void SocketClient::push_rcv_data(socket_data_t& socket_data)
{
	auto soc = _soc_map[socket_data.fd];
	if (!soc) {
		soc = new SocketClient(socket_data.fd);
		_soc_map[socket_data.fd] = soc;
	}
	auto rcv_buffer = soc->_rcv_buffer;
	size_t& current_idx = soc->_current_idx; 
	size_t& max_capacity = soc->_max_capacity;
	if (current_idx + socket_data.size() > max_capacity || 
		current_idx + socket_data.size() + ServerCfg::get_cfg<int>("rcv_buffer_section") < max_capacity) {
		max_capacity = current_idx + socket_data.size() + ServerCfg::get_cfg<int>("rcv_buffer_section");
		_rcv_buffer = realloc(max_capacity);
	} 
	
	memcpy(_rcv_buffer + current_idx, socket_data.data, socket_data.size());
	size_t packet_size = packet_header_t::get_packet_size(_rcv_buffer, current_idx);
	size_t unpack_size = 0;
	while (packet_size && packet_size <= _current_idx - unpack_size) {
		//unpack
		std::string s(_rcv_buffer + sizeof(packet_header_t), packet_size);
		spdlog::debug("rcv:%s", s.c_str());
		unpack_size += sizeof(packet_header_t) + packet_size;
		packet_size = packet_header_t::get_packet_size(_rcv_buffer + unpack_size, current_idx - unpack_size);
	}

	current_idx -= unpack_size;
	memmove(_rcv_buffer, _rcv_buffer + unpack_size, current_idx);
}
