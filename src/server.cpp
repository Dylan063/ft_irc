/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dravaono <dravaono@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/10 11:32:15 by afont             #+#    #+#             */
/*   Updated: 2025/03/11 11:19:48 by dravaono         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/all.hpp"


bool Server::_signal = false;
Server::Server()
{
}

Server::~Server()
{
}

void	Server::signalHandler(int signum)
{
	(void)signum;
	Server::_signal = true;
}

void	Server::closeFd()
{
	size_t	i;

	i = 0;
	while (i < this->_clients.size())
	{
		std::cout << "Client " << this->_clients[i]._ip << " disconnected" << std::endl;
		close(this->_clients[i++]._fd);
	}
	if (_socketFd != - 1)
	{
		std::cout << "Server closed" << std::endl;
		if (close(_socketFd) == -1)
			std::cout << "close() failed" << std::endl;
	}
}

void	Server::removeClient(int fd)
{
	size_t	i;

	i = 0;
	while (i < this->_pfds.size())
	{
		// std::cout << "\\" << std::endl;
		if (this->_pfds[i].fd == fd)
		{
			this->_pfds.erase(this->_pfds.begin() + i);
			break;
		}
		i++;
	}
	i = 0;
	while (i < this->_clients.size())
	{
		// std::cout << "/" << std::endl;
		if (this->_clients[i]._fd == fd)
		{
			this->_clients.erase(this->_clients.begin() + i);
			break;
		}
		i++;
	}
	// std::cout << ":" << std::endl;
}

void	Server::newClient()
{
	Client				cli;
	struct sockaddr_in	cli_addr;
	struct pollfd		pfd;
	socklen_t			len;
	int					cli_fd;
	int					status;

	len = sizeof(cli_addr);
	cli_fd = accept(_socketFd, (struct sockaddr *)&cli_addr, &len);
	if (cli_fd == -1)
	{
		std::cout << "accept() client failed" << std::endl;
		return;
	}
	status = getnameinfo((struct sockaddr *)&cli_addr, len, NULL, 0, cli._service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
	if (status != 0)
	{
		std::cout << "getnameinfo() failed" << std::endl;
		return;
	}
	status = fcntl(cli_fd, F_SETFL, O_NONBLOCK);
	if (status == -1)
	{
		std::cout << "fcntl() client failed" << std::endl;
		return;
	}
	pfd.fd = cli_fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	cli._fd = cli_fd;
	cli._nbMess = 0;
	cli._nickname = "Unknown";
	cli._username = "Unknown";
	cli._ip = inet_ntoa(cli_addr.sin_addr);
	this->_clients.push_back(cli);
	this->_pfds.push_back(pfd);
	// cli.sendWelcome();
}

void	Server::initSocket()
{
	int		status;
	int		opt = 1;
	struct	sockaddr_in	serv_addr;
	struct	pollfd		pfd;

	serv_addr.sin_family = AF_INET;												//IPv4
	serv_addr.sin_addr.s_addr = INADDR_ANY;										//accept only local connections 127.0.0.1
	serv_addr.sin_port = htons(_port);											//set port
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);								//SOCK_STREAM for TCP, 0 for ???
	if (_socketFd == -1)
		throw std::runtime_error("Error: socket creation failed");
	status = setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//set socket options to reuse address
	if (status == -1)
		throw std::runtime_error("Error: setsockopt failed");
	status = bind(_socketFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));	//bind socket to address
	if (status == -1)
		throw std::runtime_error("Error: bind failed");
	status = fcntl(_socketFd, F_SETFL, O_NONBLOCK);								//set socket to non-blocking
	if (status == -1)
		throw std::runtime_error("Error: fcntl failed");
	status = listen(_socketFd, SOMAXCONN);										//listen for connections, 4096 clients max
	if (status == -1)
		throw std::runtime_error("Error: listen failed");
	pfd.fd = _socketFd;
	pfd.events = POLLIN;														// to read data
	pfd.revents = 0;															// no return events
	this->_pfds.push_back(pfd);
}

int	Server::getClientIndex(int fd)
{
	size_t	i;

	i = 0;
	while (i < this->_clients.size())
	{
		if (this->_clients[i]._fd == fd)
			return (i);
		i++;
	}
	return (-1);
}

void	Server::processData(t_cmd *dataCmd, int fd)
{
	std::string buf(1, 0);
	size_t	bytes;
	size_t	i;	

	i = getClientIndex(fd);
	bytes = recv(fd, &buf[0], 1, 0);
	if (bytes <= 0)
	{
		std::cout << "recv() failed / client disconnected " << std::endl;
		removeClient(fd);
		close(fd);
	}
	else
	{
		// std::cout << "Data received: [" << buf << "]" << std::endl;
		// std::cout << "i: " << i << std::endl;
		// _clients[i]._nickname = "Alice";
		if (parserCmd(dataCmd, buf) == 1) //si c'est la fin du message
		{
			std::cout << "Message: [" << dataCmd->_message << "]" << std::endl;
			// printvector(dataCmd->_cmd);
			checkCmd(&_clients[i], dataCmd->_cmd, this);
			// appel aux commandes ici
			dataCmd->_message.clear();
		}
	}
}

void	Server::initServer(t_cmd *dataCmd)
{
	int	status;
	size_t	i;

	initSocket();
	std::cout << "Server started on port " << _port << std::endl;
	while (!this->_signal)
	{
		status = poll(&this->_pfds[0], this->_pfds.size(), -1);
		if (status == -1 && !this->_signal)
			throw std::runtime_error("Error: poll failed");
		i = 0;
		while (i < this->_pfds.size())
		{
			// std::cout << i << std::endl;
			if (this->_pfds[i].revents & POLLIN)	// Vérifie si des données sont disponibles en lecture
			{
				if (this->_pfds[i].fd == _socketFd)
				{
					newClient();
					std::cout << "New client connected" << std::endl;
				}
				else
				{
					// std::cout << "Data received from client" << std::endl;
					processData(dataCmd, this->_pfds[i].fd);
				}
			}
			i++;
		}
	}
	closeFd();
}
