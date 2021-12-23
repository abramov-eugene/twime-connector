#include <iostream>
#include <cstdio>
#include <csignal>
#include "messages.hpp"
#include "twime.hpp"

using namespace twime;

shared_ptr<TwimeConnector> connector;
shared_ptr<Logger> logger;

void handler(int) {
    if (connector->isRunning()){
        logger->log("Connector is stopping!!!");
        connector->disconnect();
    }
}

int main(int argc, char **argv) {
    signal(SIGKILL, handler);
    signal(SIGINT, handler);
    if (argc != 4) {
        cerr << "Usage:" << argv[0] << "<pass> <ip> <port>" << endl;
        return -1;
    }

    logger = std::make_shared<Logger>("twime.log");
    connector = std::make_shared<TwimeConnector>(*logger);

    try {
        connector->setUser(string(argv[1]));
        connector->connect(string(argv[2]), atoi(argv[3]));
        while(connector->isRunning()) {
            sleep(1); //delay for NOS-request
            /*if (connector->getStatus() == Status::AUTHORIZED) {
               NewOrderSingle nos;

               nos.setClOrdId(24)
                  .setClOrdLinkId(1)
                  .setSecurityId(12345)
                  .setQty(1)
                  .setPrice(100.0)
                  .setSide(Side::BUY)
                  .setTimeInForce(TimeInForce::IOC)
                  .setAccount("ABCDEFG");

               connector->send(nos);
            }*/
        }
    }
    catch(std::exception& ex){
        std::cerr << "Exception:" << ex.what() << std::endl;
        connector->disconnect();
    }
    return 0;
}
