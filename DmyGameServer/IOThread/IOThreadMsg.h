#ifndef _IO_THREAD_MSG_H_
#define _IO_THREAD_MSG_H_

enum io_msg_type
{
	IO_SOC_DATA = 1,
	IO_CONTROL = 2,
};

struct io_data_t
{
	size_t data_size;
	char* data;
};

struct io_msg_t
{
	int fd;
	int msg_type;
}





#endif