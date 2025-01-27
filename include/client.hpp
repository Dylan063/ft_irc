#pragma once

#include <string>
#include <netinet/in.h>

class Client {
private:
    int _socket_fd;
    std::string _nickname;
    bool _authenticated;
    struct sockaddr_in _address;

public:
    Client(int socket, struct sockaddr_in address);
    ~Client();
    int getSocketFd() const;
    void setSocketFd(int fd);
    const std::string& getNickname() const;
    void setNickname(const std::string& nickname);
    bool isAuthenticated() const;
    void setAuthenticated(bool state);
    const struct sockaddr_in& getAddress() const;
};