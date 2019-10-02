#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <boost/lockfree/spsc_queue.hpp>
//warp boost spsc_queue
// struct cmd_t
// {
	// int cmd_id;
	// void* cmd_data;
// };

class RingBuffer
{
public:
	RingBuffer(std::size_t size);
	// ~RingBuffer();
	void push(void* t);

	void pop(void** t, std::size_t size);

	void tick();
private:
	void _push_buffer();

	boost::lockfree::spsc_queue<void*> _queue;
	std::list<void*> _buffer;
	std::size_t _queue_size;
};

class WorkQueue
{
public:
	WorkQueue();

	WorkQueue(std::size_t queue_size);
	
public:
	void push(void* t);

	void pop(void** t, std::size_t size);

	void tick();

	void hook();
private:
	std::unordered_map<std::thread::id, RingBuffer<void*>*> _queue_push;
	std::unordered_map<std::thread::id, RingBuffer<void*>*> _queue_pop;	
	std::this_thread::id thread_1_id;
};






#endif