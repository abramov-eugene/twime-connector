#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

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
   boost::thread serviceThread;
   static const size_t MAX_BUFF_SIZE = 2048;
   char buff[MAX_BUFF_SIZE];   
   
   public:
   Session() 
   : status(Status::DISCONNECTED)
   //, timer(100)
   , service()
   , sock(service)
   , serviceThread(boost::bind(&boost::asio::io_service::run, &service)){       
   }
   
   ~Session(){
       serviceThread.join();
   }
   
   int connect(const std::string& ip, const unsigned int& port){
      if (status == Status::DISCONNECTED){
         status = Status::CONNECTING;
         tcp::endpoint endpoint(address::from_string(ip),port);
         sock.connect(endpoint);
         status = Status::CONNECTED;
         std::cout << "connected " << std::endl;
         init_read();
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

   int send(char *buff, const int len){
       boost::asio::write(sock, boost::asio::buffer(buff, len));
       return 0;
   }
   
   //int sendCommand(const Command& command){
   //   if (status == Status""AUTHORIZED){
   //       boost::array buff = parser(command)
   //       return sock.send(buff);         
   //   }
   //   return -1;
   //}
private:
    void onRead(const boost::system::error_code& error, size_t bytes){
        std::cout << "onRead:" << error << "" << bytes << std::endl;
        if (!error){
           //use bytes of buff
            if (bytes < MAX_BUFF_SIZE){
                //parser.parse(buff, bytes);
                init_read();
            }
                
        } else {
            //todo log(error);
        }
       
    }
    
    void init_read(){
        sock.async_read_some(boost::asio::buffer(buff, MAX_BUFF_SIZE), boost::bind(&Session::onRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }   
};
};
