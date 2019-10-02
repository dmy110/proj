#include <iostream>
#include <sys/types.h>  
#include <sys/socket.h>
#include <epoll.h>
#include <signal.h>
#include "TickThread.h"


// std::string ip_str;
// int port;
// void init_cfg()
// {
// 	ip_str = "192.168.56.10";
// 	port = 1234;
// }


// void ign_pipe()
// {
// 	struct sigaction ign_act;
// 	ign_act.sa_handler = SIG_IGN;
// 	sigaction(SIG_PIPE, &ign_act, nullptr);
// }

// void init_server_sock()
// {
// 	listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 	int bool_true = 1;
// 	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &bool_true, sizeof(bool_true));
// 	struct sockaddr server_addr;
// 	memset(&server_addr, 0, sizeof(server_addr));
// 	server_addr.sin_family = AF_INET;
//     // server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(ServerCfg::get_cfg<int>("server_port"));
//     inet_pton(AF_INET, ServerCfg::get_cfg<std::string>("server_ip").c_str(), &(server_addr.sin_addr));
//     bind(listen_sock, &server_addr, sizeof(server_addr));
//     listen(listen_sock, 128);
// }

// void init_epoll()
// {
// 	/* Code to set up listening socket, 'listen_sock',
// 	   (socket(), bind(), listen()) omitted */
// 	epollfd = epoll_create(1);
// 	if (epollfd == -1) {
// 	    perror("epoll_create1");
// 	    exit(EXIT_FAILURE);
// 	}
// 	ev.events = EPOLLIN;
// 	ev.data.fd = listen_sock;
// 	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
// 	    perror("epoll_ctl: listen_sock");
// 	    exit(EXIT_FAILURE);
// 	}
// }



int main()
{
	DmyGameServer server;
	server.start_server();
	// init_cfg();
	// ign_pipe();
	// init_server_sock();
	// init_epoll();
   
	// TickThread tt;
	// tt.start_thread();
}