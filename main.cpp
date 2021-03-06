#include <iostream>
#include <cstdio>
#include <csignal>
#include "twime.hpp"

twime::TwimeConnector connector;

void handler(int){
    if (connector.isRunning()){
        cerr << "Connector is stopping!!!" << endl;
        connector.disconnect();
    }    
}

int main(int argc, char **argv) {
    signal(SIGKILL, handler);
    signal(SIGINT, handler);
    if (argc != 4){
        cerr << "Usage:" << argv[0] << "<pass> <ip> <port>" << endl;
        return -1;
    }
    try{
        
        connector.setUser(string(argv[1]));
        connector.connect(string(argv[2]), atoi(argv[3]));
        while(connector.isRunning())
            sleep(1);
    }
    catch(std::exception& ex){
        cerr << "Exception:" << ex.what();
        connector.disconnect();
    }
    return 0;
}
