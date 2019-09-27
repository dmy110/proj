#include "TickThread.h"
#include "ServerCfg.h"

TickThread::TickThread()
{
	_rcv_queue = new RingBuffer<socket_data_t*>(ServerCfg::get_cfg<int>("command_queue_size"));
	_snd_queue = new RingBuffer<socket_data_t*>(ServerCfg::get_cfg<int>("command_queue_size"));
	_buffer_idx = 0;
}

void TickThread::start_thread()
{
	tick = new std::thread([this](){
		_start_thread();
	});
}

void TickThread::_start_thread()
{
	int num = _rcv_queue.pop(_rcv_buffer, 128);
	if (num) {
		
	}
}
