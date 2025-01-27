#include "../include/client.hpp"

Client::Client(int socket, struct sockaddr_in address) : _socket_fd(socket), _nickname(""), _authenticated(false), _address(address) {}

Client::~Client(){}

int Client::getSocketFd() const {
    return _socket_fd;
}
void Client::setSocketFd(int fd) {
    _socket_fd = fd;
}

const std::string& Client::getNickname() const {
    return _nickname;
}
void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

bool Client::isAuthenticated() const {
    return _authenticated;
}
void Client::setAuthenticated(bool state) {
    _authenticated = state;
}

const struct sockaddr_in& Client::getAddress() const {
    return _address;
}
