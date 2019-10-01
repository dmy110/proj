#ifndef _IO_THRAD_H_
#define _IO_THRAD_H_

#include "Common/DmyThread"

struct io_data_t
{
	int fd = 0;
	char* snd_data = nullptr;
	int size = 0;
	int idx = 0;
	// bool validated = false;
	// bool req_validate = false;
	// int validate_data_size = 0;
};

class IOThread : public DmyThread
{
public:
	IOThread();
	// ~IOThread();
protected:
	void _start_thread() override;
private:
	int _listen_sock;
	int _epollfd;
	struct epoll_event _ev;
	struct epoll_event _events[128];
	WorkQueue* _tick_queue;
private:
	void ign_pipe();
	void init_server_sock();
	void init_epoll();
	void start_epoll_loop();
	void close_server();	

private:
	void reg_fd(int fd);
	void unreg_fd(int fd);

	void handle_epoll_in(int fd);
private:	
	std::unordered_map<int, io_data_t> _io_data;
};






#endif