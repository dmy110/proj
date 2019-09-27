#ifndef _PACKET_HEADER_H_
#define _PACKET_HEADER_H_

struct packet_header_t
{
	size_t packet_size;
	static size_t get_packet_size(char* data, size_t data_len);
};










#endif