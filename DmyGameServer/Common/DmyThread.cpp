#include "DmyThread.h"

DmyThread::DmyThread(const char* thread_name):_thread_name(thread_name)
{
	_thread = nullptr;
	is_shutdown = false;
}

DmyThread::~DmyThread()
{

}

void DmyThread::start_thread()
{
	_thread = new std::thread([this](){
		reg_thread(this);
		_start_thread();
		
	});
}

void DmyThread::join_thread()
{

}

void DmyThread::reg_thread(DmyThread* thread)
{
	std::unique_lock lock(_mutex);
	_thread_pool_name[thread->_thread_name] = thread;
	_thread_pool_id[thread->get_thread_id()] = thread;
	
}

WorkQueue* DmyThread::req_queue(const char* thread_name)
{
	std::unique_lock lock(_mutex);	
	std::string target_key(thread_name);
	std::string self_key = get_self_thread_name();
	if (_work_queue[self_key][target_key]) {
		return _work_queue[self_thread_id][target_key];
	} else if (_work_queue[target_key][self_key]) {
		_work_queue[self_key][target_key] = _work_queue[target_key][self_key];
		_work_queue[self_key][target_key]->hook();
		return _work_queue[self_key][target_key];
	} else {
		_work_queue[self_key][target_key] = new WorkQueue();
		return _work_queue[self_key][target_key];
	}
}

std::thread::id DmyThread::get_thread_id()
{
	if (_thread) {
		return _thread->get_id();
	}
	return std::thread::id;
}

std::string DmyThread::get_self_thread_name()
{
	auto thread_id = std::this_thread::get_id();
	if (_thread_pool_id[thread_id]) {
		_thread_pool_id[thread_id]->get_thread_name();
	}
	return std::string();
}

std::string DmyThread::get_thread_name()
{
	return _thread_name;
}
