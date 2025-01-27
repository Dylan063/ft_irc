#include "../include/server.hpp"
#include "../include/client.hpp"

Server::Server() {}

Server::~Server(){
}

void Server::init(){
    struct sockaddr_in info;
    struct pollfd thePoll;
    int en = 1;

    info.sin_addr.s_addr = INADDR_ANY;
    info.sin_port = htons(this->_port); // converti le port 
    info.sin_family = AF_INET; // mettre l'adresse en ipv4
    this->fd_serv = socket(AF_INET, SOCK_STREAM, 0); // creation du socket
    if (this->fd_serv == -1) // voir si le socket est bien creer
        throw (std::runtime_error("Erreur de la creation du socket"));
    if (setsockopt(this->fd_serv, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) // définir l'option socket (SO_REUSEADDR) pour réutiliser l'adresse
        throw (std::runtime_error("Erreur du setSocket"));
    if (fcntl(this->fd_serv, F_SETFL, O_NONBLOCK)) // defini l'option O_NONBLOCK pour rendre le socket non blonquant, il sera accessiblr pour recevoir accept ou send
        throw(std::runtime_error("Erreur, socket blocked"));
    if (bind(this->fd_serv, (struct sockaddr*)&info, sizeof(info)) < 0) // lier le socket a l'adresse 
        throw std::runtime_error("Erreur lors du bind.");
    if (listen(this->fd_serv, 0)) // permet de rendre le socket passif en attendant une connexion
        throw std::runtime_error("Erreur lors du listen.");
    
    thePoll.fd = this->fd_serv; // assigner le pollfd au socket du serveur, 
    thePoll.events = POLLIN; // permet d'etre notifier lorsque des données sont disponibles en lectures
    thePoll.revents = 0; // permet de confirmer les evenements arrivées dans events
    // std::cout << "Server ON" << std::endl;
}

void Server::connectServer(){
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_fd = accept(this->fd_serv, (struct sockaddr*)&client_address, &client_len);
    if (client_fd == -1) {
        std::cerr << "Erreur : Impossible d'accepter la connexion." << std::endl;
        return;
    }
    Client new_client(client_fd, client_address);
    this->_client.push_back(new_client);
    std::cout << "Nouvelle connexion acceptée : " << inet_ntoa(client_address.sin_addr) << " : " << ntohs(client_address.sin_port) << std::endl;
}

void Server::run(){
    this->_port = 6667;
    init();
    std::cout << "Server <" << this->fd_serv << "> Connected" << std::endl;
}