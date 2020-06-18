#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "messages.hpp"

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
   static const size_t MAX_BUFF_SIZE = 8 * 2048;
   static const size_t MAX_RECV_SIZE = 2048;
   
   char sendBuff[MAX_BUFF_SIZE];
   char recvBuff[MAX_RECV_SIZE];
   std::string userName;
   unsigned int keepAlive;
   std::string ip;
   unsigned int port;
   long lastConnecting;
   long lastEstablish;
   long lastSended;
   unsigned long nextSeqNum;
   long lastRecv;
   unsigned int keepAliveServer;
   public:
   Session(std::string mUserName, std::string mIP, unsigned int mPort) 
   : status(Status::DISCONNECTED)
   //, timer(100)
   , service()
   , sock(service)
   , serviceThread(boost::bind(&boost::asio::io_service::run, &service))
   , userName(mUserName)
   , keepAlive(30)
   , ip(mIP)
   , port(mPort)
   , lastConnecting(0)
   , lastEstablish(0)
   , lastSended(0)
   , nextSeqNum(0)
   , lastRecv(0)
   , keepAliveServer(0)/*get from Establish Ack*/{
       memset(sendBuff, 0, MAX_BUFF_SIZE);
       memset(recvBuff, 0, MAX_RECV_SIZE);
   }
   
   ~Session(){
       serviceThread.join();
   }
   
   void onTimer(){
        switch (status){
            case Status::CONNECTING:{
                if (lastConnecting > DateTimeUtils::now(-60)){
                    connect(ip, port);
                    lastConnecting = DateTimeUtils::now();
                }
                break;
            }
            case Status::CONNECTED:{
                if (lastEstablish > DateTimeUtils::now(-60)){
                    Establish est(keepAlive, userName);
                    sendCommand(&est);
                    lastEstablish = DateTimeUtils::now();
                }
                break;
            }
            case Status::AUTHORIZED:{
                if (lastSended > DateTimeUtils::now(-keepAlive)){
                    Sequence seq(nextSeqNum);
                    sendCommand(&seq);
                    lastSended = DateTimeUtils::now();
                }
                if (lastRecv > DateTimeUtils::now(-keepAliveServer)){
                    //log(ServerDown);
                    disconnect();
                }
                break;
            }
            case Status::DISCONNECTING:{
                break;
            }
            case Status::DISCONNECTED:{
                break;
            }
        }        
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
         Terminate term(0);
         sendCommand(&term);
         sock.close();
         status = Status::DISCONNECTED;
      }
      return 0;
   }
   
   bool isRunning(){
       return status != Status::DISCONNECTED;
   }

   int send(const char *buff, const int len){
       boost::asio::write(sock, boost::asio::buffer(buff, len));
       lastSended = DateTimeUtils::now();
       return 0;
   }
   
   int sendCommand(FixMessage* command){
      if (status == Status::AUTHORIZED){  
          int len = command->encode(sendBuff, MAX_BUFF_SIZE);
          if (len > 0)
             return send(sendBuff, len);
          //else
             //log("Can't encode command:", command->str())
      }
      return -1;
   }
   
private:
    void onRead(const boost::system::error_code& error, size_t bytes){
        lastRecv = DateTimeUtils::now();
        std::cout << "onRead:" << error << "" << bytes << std::endl;
        if (!error){
           //use bytes of buff
            if (bytes < MAX_BUFF_SIZE){
                //parser.parse(revBuff, bytes);
                init_read();
            }
        } else {
            //todo log(error);
        }       
    }
    
    void init_read(){
        sock.async_read_some(boost::asio::buffer(recvBuff, MAX_RECV_SIZE), boost::bind(&Session::onRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }   
};
};
