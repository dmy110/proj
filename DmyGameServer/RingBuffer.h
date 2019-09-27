#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <boost/lockfree/spsc_queue.hpp>
//warp boost spsc_queue
template <typename T>
class RingBuffer
{
public:
	RingBuffer(std::size_t size):_queue(size)
	{

	}
	// ~RingBuffer();
	void push(T& t)
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

	void pop(T* t, std::size_t size)
	{
		return _queue.pop(t, size);
	}

	void tick()
	{
		_push_buffer();
	}
private:
	void _push_buffer()
	{
		while (_buffer.size() && _queue.push(_buffer.front())) {
			_buffer.pop_front();
		}		
	}

	boost::lockfree::spsc_queue<T> _queue;
	std::list<T> _buffer;
	std::size_t _queue_size;
};









#endif