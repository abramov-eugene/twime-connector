#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::address;
using boost::asio::ip::tcp;

namespace twime
{
enum Status {DISCONNECTED=0, CONNECTING=1, CONNECTED=2, AUTHORIZED=3, DISCONNECTING=4};

class Session{
   Status status;
   //Timer timer;
   boost::asio::io_service service;
   tcp::socket sock;
   
   public:
   Session() 
   : status(Status::DISCONNECTED)
   //, timer(100)
   , service()
   , sock(service){
       service.run();
   }
   
   ~Session(){
       service.stop();
   }
   
   int connect(const std::string& ip, const unsigned int& port){
      if (status == Status::DISCONNECTED){
         status = Status::CONNECTING;
         tcp::endpoint endpoint(address::from_string(ip),port);
         sock.connect(endpoint);
      }
      return 0;
   }
   
   int disconnect(){
      if (status == Status::AUTHORIZED){
         status = Status::DISCONNECTING;
         //CommandTerminate term;
         //sock.sendCommand(term);
         sock.close();
         status = Status::DISCONNECTED;
      }
      return 0;
   }   
   
   bool isRunning(){
       return status != Status::DISCONNECTED;
   }
   
   //int sendCommand(const Command& command){
   //   if (status == Status""AUTHORIZED){
   //       boost::array buff = parser(command)
   //       return sock.send(buff);         
   //   }
   //   return -1;
   //}
   
};
};
