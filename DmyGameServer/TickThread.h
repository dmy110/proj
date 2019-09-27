#ifndef _TICK_THREAD_H_
#define _TICK_THREAD_H_
#include "socket_data.h"
class TickThread
{
public:
	TickThread();
	~TickThread();
	void start_thread();
private:
	void _start_thread();
	std::thread* tick;
	RingBuffer<socket_data_t*>* _rcv_queue;
	RingBuffer<socket_data_t*>* _snd_queue;

	socket_data_t _rcv_buffer[128];
};



#endif