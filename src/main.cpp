#include "../include/server.hpp"

int main(int ac, char **av){
    if (ac){
        (void)av;
        try{
            Server test;
            test.run();
            test.connectServer();
        } catch (std::exception &e){
            std::cout << "programme fail because " << e.what() << std::endl;
        }
    }
    else{
        std::cout << "number args" << std::endl;
    }
}