#include "IOThread.h"
#include "ThreadDefine.h"
#include <unistd.h>
IOThread::IOThread():DmyThread(IOThread.c_str())
{
}

void IOThread::ign_pipe()
{
	struct sigaction ign_act;
	ign_act.sa_handler = SIG_IGN;
	sigaction(SIG_PIPE, &ign_act, nullptr);
}

void IOThread::init_server_sock()
{
	_listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int bool_true = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &bool_true, sizeof(bool_true));
	int rcv_buf = ServerCfg::get_cfg<int>("rcv_buffer");
	setsockopt(listen_sock, SOL_SOCKET, SO_RCVBUF, &rcv_buf, sizeof(rcv_buf));

	int snd_buf = ServerCfg::get_cfg<int>("snd_buffer");
	setsockopt(listen_sock, SOL_SOCKET, SO_SNDBUF, &snd_buf, sizeof(snd_buf));
	
	struct sockaddr server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(ServerCfg::get_cfg<int>("server_port"));
    inet_pton(AF_INET, ServerCfg::get_cfg<std::string>("server_ip").c_str(), &(server_addr.sin_addr));
    bind(listen_sock, &server_addr, sizeof(server_addr));
    listen(listen_sock, 128);
}

void IOThread::init_epoll()
{
	_epollfd = epoll_create(1);
	if (epollfd == -1) {
	    perror("epoll_create1");
	    exit(EXIT_FAILURE);
	}
	_ev.events = EPOLLIN;
	_ev.data.fd = listen_sock;
	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listen_sock, &_ev) == -1) {
	    perror("epoll_ctl: listen_sock");
	    exit(EXIT_FAILURE);
	}
}

void IOThread::start_epoll_loop()
{
	_tick_queue = DmyThread::req_queue(TickThreadName.c_str());
	int conn_sock;
	for (;;) {
		nfds = TEMP_FAILURE_RETRY(epoll_wait(epollfd, events, 128, 1));
		if (nfds == -1) {
		    perror("epoll_wait");
		    exit(EXIT_FAILURE);
		}
		for (n = 0; n < nfds; ++n) {
		    if (events[n].data.fd == _listen_sock) {
		        conn_sock = accept4(_listen_sock,
		                           (struct sockaddr *) &addr, &addrlen, SOCK_NONBLOCK);
		        if (conn_sock == -1) {
		            perror("accept");
		            // exit(EXIT_FAILURE);
		            continue;
		        }
				int nodelay = 1;
				setsockopt(conn_sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

		        ev.events = EPOLLIN;
		        ev.data.fd = conn_sock;
		        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
		                    &ev) == -1) {
		            perror("epoll_ctl: conn_sock");
		            exit(EXIT_FAILURE);
		        }

		       	reg_fd(conn_sock);
		    } else {
		    	if (events[n].data.events & EPOLLIN) {
		    		handle_epoll_in(events[n].data.fd);
		    	}
		    }
		}
	}

}

void IOThread::close_server()
{
	spdlog::debug("close server");
}

void IOThread::_start_thread() 
{
	ign_pipe();
	init_server_sock();
	init_epoll();
	start_epoll_loop();
	close_server();	
}

void IOThread::reg_fd(int fd)
{
	if (_io_data.count(fd)) {
		spdlog::error("dulplicate fd:%d", fd);
		free(_io_data[fd].snd_data);
		_io_data.erase(fd);
	}
	auto& data = _io_data[fd];
	data.validate = false;
	// data.snd_data = (char*)malloc(4096);
	// data.size = 4096;
}

void IOThread::unreg_fd(int fd)
{
	free(_io_data[fd].snd_data);
	_io_data.erase(fd);	
}

void IOThread::handle_epoll_in(int fd)
{
	
}
