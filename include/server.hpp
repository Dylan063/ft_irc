#pragma once 

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include "client.hpp"


class Server{
	private  :
	std::vector<Client> _client;
	int fd_serv;
	std::string _ip;
	int _port;
	std::vector<struct pollfd> fdvec;

	public :
	Server();
	~Server();
	void init();
	void run();
	void connectServer();
};