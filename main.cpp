#include <iostream>
#include <cstdio>
#include <csignal>
#include "twime.hpp"

twime::TwimeConnector connector;

void handler(int){
    if (connector.isRunning()){
        connector.disconnect();
    }    
}

int main(int argc, char **argv) {
    signal(SIGKILL, handler);
    signal(SIGINT, handler);
    if (argc != 4){
        std::cerr << "Usage:" << argv[0] << "<pass> <ip> <port>" << std::endl;
        return -1;
    }
    connector.setUser(std::string(argv[1]));
    connector.connect(std::string(argv[2]), atoi(argv[3]));
    while(connector.isRunning())
        sleep(1);    
    return 0;
}
