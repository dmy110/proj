#include "PacketHeader.h"

size_t packet_header_t::get_packet_size(char* data, size_t data_len)
{
	if (data_len < sizeof(packet_header_t)) return 0;
	size_t* ret = static_cast<size_t*>(data);
	return *ret;
}
