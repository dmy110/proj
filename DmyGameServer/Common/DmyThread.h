#ifndef _DMY_THREAD_H_
#define _DMY_THREAD_H_

class DmyThread
{
public:
	DmyThread(const char* thread_name);
	virtual ~DmyThread();

	void start_thread();
	std::thread::id get_thread_id();
	std::string get_thread_name();
protected:
	virtual void _start_thread() = 0;
// private:
	std::thread* _thread;
	std::string _thread_name;
	bool is_shutdown;
public:
	static void reg_thread(DmyThread* thread);
	static void unreg_thread(DmyThread* thread);
	static WorkQueue* req_queue(const char* thread_name);
	static std::string get_self_thread_name();
private:
	static std::unordered_map<std::string, DmyThread*> _thread_pool_name;
	static std::unordered_map<std::thread::id, DmyThread*> _thread_pool_id;
	static std::shared_mutex _mutex;
	static std::unordered_map<std::string, std::unordered_map<std::string, WorkQueue*>> _work_queue; 
public:

};











#endif