#include "SocketClient.h"

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
	if (rcv_buffer.size() + socket_data.size() > rcv_buffer.capacity() || 
		rcv_buffer.size() + socket_data.size() + ServerCfg::get_cfg<int>("rcv_buffer_section") < rcv_buffer.capacity()) {
		rcv_buffer.reserve(rcv_buffer.size() + socket_data.size());
	} 
	
	memcpy(rcv_buffer.data() + rcv_buffer.size(), socket_data.data, socket_data.size());
	rcv_buffer.resize(rcv_buffer.size() + socket_data.size());
	// if (packet_header_t::get_packet_size(rcv_buffer.data()))	
	size_t packet_size = packet_header_t::get_packet_size(rcv_buffer.data());

}
