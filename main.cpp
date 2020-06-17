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
    if (argc != 3){
        std::cerr << "Usage:" << argv[0] << "<ip> <port>" << std::endl;
        return -1;
    }
    connector.connect(std::string(argv[1]), atoi(argv[2]));
    while(connector.isRunning())
        sleep(1);    
    return 0;
}
