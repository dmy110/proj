#include "RingBuffer.h"

RingBuffer::RingBuffer(std::size_t size):_queue(size)
{

}
// ~RingBuffer();
void RingBuffer::push(cmd_t& t)
{
	_push_buffer();
	if (_buffer.size()) {
		_buffer.push_back(t);
		return;
	}

	if (!_queue.push(t)) {
		_buffer.push_back(t);
		return;
	}
}

void RingBuffer::pop(cmd_t* t, std::size_t size)
{
	return _queue.pop(t, size);
}

void RingBuffer::tick()
{
	_push_buffer();
}

void RingBuffer::_push_buffer()
{
	while (_buffer.size() && _queue.push(_buffer.front())) {
		_buffer.pop_front();
	}		
}

void WorkQueue::push(cmd_t& t)
{
	auto& thread_id = std::this_thead::get_id();
	_queue_push[thread_id]->push(t);
}

void WorkQueue::pop(cmd_t* t, std::size_t size)
{
	auto& thread_id = std::this_thead::get_id();
	_queue_pop[thread_id]->pop(t, size);
}

void WorkQueue::tick()
{
	auto& thread_id = std::this_thead::get_id();
	_queue_push[thread_id].tick();
}

WorkQueue::WorkQueue():WorkQueue(128)
{
}

WorkQueue::WorkQueue(std::size_t queue_size)
{
	thread_1_id = std::this_thead::get_id();
	_queue_push[thread_1_id] = new RingBuffer(queue_size);
	_queue_pop[thread_1_id] = new RingBuffer(queue_size);
}

void WorkQueue::hook()
{
	auto& thread_id = std::this_thead::get_id();
	_queue_push[thread_id] = _queue_pop[thread_1_id];
	_queue_pop[thread_id] = _queue_push[thread_1_id];
}
