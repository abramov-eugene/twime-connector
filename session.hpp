#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "messages.hpp"
#include "parser.hpp"
using boost::asio::ip::address;
using boost::asio::ip::tcp;

namespace twime
{
enum Status {DISCONNECTED=0, CONNECTING=1, CONNECTED=2, AUTHORIZED=3, DISCONNECTING=4};

class Session{
   Status status;
   boost::asio::io_service service;
   boost::asio::deadline_timer timer;
   tcp::socket sock;
   boost::thread serviceThread;
   static const size_t MAX_BUFF_SIZE = 8 * 2048;
   static const size_t MAX_RECV_SIZE = 2048;
   
   char sendBuff[MAX_BUFF_SIZE];
   char recvBuff[MAX_RECV_SIZE];
   std::string userName;
   unsigned int keepAlive;
   long reconnectTimeoutSec;
   long lastConnecting;
   long lastEstablish;
   long lastSended;
   uint64 nextSeqNum;
   uint64 sendSeqNum;
   long lastRecv;
   unsigned int keepAliveServer;
   Parser<Session> parser;
   std::string ip;
   unsigned int port;
   public:
   Session() 
   : status(Status::DISCONNECTED)
   //, timer(100)
   , service()
   , timer(service, boost::posix_time::seconds(2))
   , sock(service)
   , serviceThread(boost::bind(&boost::asio::io_service::run, &service))
   , userName("")
   , keepAlive(10000)
   , reconnectTimeoutSec(60)
   , lastConnecting(DateTimeUtils::now(-reconnectTimeoutSec))
   , lastEstablish(DateTimeUtils::now(-30))
   , lastSended(DateTimeUtils::now(-30))
   , nextSeqNum(0)
   , sendSeqNum(0)
   , lastRecv(DateTimeUtils::now(-30))
   , keepAliveServer(30000)/*get from Establish Ack*/
   , parser(*this)
   , ip("")
   , port(0)
   {
       memset(sendBuff, 0, MAX_BUFF_SIZE);
       memset(recvBuff, 0, MAX_RECV_SIZE);
       timer.async_wait(boost::bind(&Session::onTimer, this));
   }
   
   ~Session(){
       timer.cancel();
       service.stop();
       serviceThread.join();
   }
   
    void setUser(const std::string& mUser){
        userName = mUser;
    }
    
    void log(const std::string msg){
        time_t t = time(nullptr);
        std::cout   << "[" << t << "]" << msg << std::endl;
    }

   void onTimer(){
        switch (status){
            case Status::CONNECTING:{
                if (lastConnecting < DateTimeUtils::now(-reconnectTimeoutSec)){
                    tcp::endpoint endpoint(address::from_string(ip),port);
                    log("Connect:" + ip + ":" + std::to_string(port));
                    sock.connect(endpoint);
                    lastConnecting = DateTimeUtils::now();
                }
                if (sock.is_open()){
                    status = Status::CONNECTED;
                }
                break;
            }
            case Status::CONNECTED:{
                if (lastEstablish < DateTimeUtils::now(-5)){
                    log("Establish sended");
                    Establish est(keepAlive, userName);
                    sendCommand(&est);
                    lastEstablish = DateTimeUtils::now();
                }
                break;
            }
            case Status::AUTHORIZED:{
                if (lastSended < DateTimeUtils::now(-keepAlive/1000)){
                    log("Heartbeat sended");
                    Sequence seq(sendSeqNum);
                    sendCommand(&seq);
                    lastSended = DateTimeUtils::now();
                    //sendSeqNum ++;
                }
                /*if (keepAliveServer != 0 && lastRecv < DateTimeUtils::now(-keepAliveServer/1000)){
                    log("Heartbeat from server");
                    sock.close();
                    status = Status::DISCONNECTED;
                }*/
                break;
            }
            case Status::DISCONNECTING:{
                if (!sock.is_open()){
                    status = Status::DISCONNECTED;
                }
                break;
            }
            case Status::DISCONNECTED:{
                break;
            }
        }
        timer.expires_at(timer.expires_at() + boost::posix_time::seconds(1));
        timer.async_wait(boost::bind(&Session::onTimer, this));
    }
   
   int connect(const std::string& mIp, const unsigned int& mPort){
        port = mPort;
        ip = mIp;         
        if (status == Status::DISCONNECTED){         
            status = Status::CONNECTING;
            log("Connecting state");
        }
        return 0;
   }
   
   int disconnect(){
      if (status == Status::AUTHORIZED){
         Terminate term(0);
         sendCommand(&term);
         log("Terminate sended");
      }
      else if (status == Status::CONNECTED){
          sock.close();
      }
      status = Status::DISCONNECTING;
      return 0;
   }
   
   bool isRunning(){
       return status != Status::DISCONNECTED && status != Status::DISCONNECTING;
   }

   int send(const char *buff, const int len){
       printBuff(">>", buff, len);
       boost::asio::write(sock, boost::asio::buffer(buff, len));
       lastSended = DateTimeUtils::now();
       init_read();
       return 0;
   }
   
   int sendCommand(FixMessage* command){
        if (isRunning()){
            int len = command->encode(sendBuff, MAX_BUFF_SIZE);
            if (len > 0)
                return send(sendBuff, len);
            else
                log("Error: Can't encode command");
        } else {
            log("Error: Can't send message. Status is not ready");
        }        
        return -1;
   }

    void onError(const std::string& error){
        log("Error message parsing:" + error);
    }    
    
    void onMessage(const std::shared_ptr<EstablishmentReject> reject){        
        log("EstalishReject received. Reject:" + std::to_string(reject->getRejectCode()));
        disconnect();
    }
    
    void onMessage(const std::shared_ptr<Sequence>){
        log("Heartbeat received");
    }
    
    void onMessage(const std::shared_ptr<Terminate>){
        log("Terminate received");
        status = Status::DISCONNECTED;
        sock.close();
    }
    
    void onMessage(const std::shared_ptr<EstablishmentAck> msg){        
        status = Status::AUTHORIZED;
        keepAliveServer = msg->getKeepAlive();
        nextSeqNum = msg->getNextSeqNum();
        log("EstablishmentAck received ");// + std::to_string(nextSeqNum) + "/" + std::to_string(keepAliveServer));
    }
    
private:
    
    void printBuff(const char* temp, const char *buff, int len){
        if (len == 0)
            return;
        std::cout << temp << "Buff(" << len << "):";
        for(int i = 0; i< len;++i){
            unsigned char c = buff[i];
            //std::cout << << std::setfill('0') << std::setw(2) << c << ":";
            std::cout << std::setfill('0') << std::setw(2) << static_cast<uint16>(c) << ":";
        }
        std::cout << std::endl;
    }
    
    void onRead(const boost::system::error_code& error, size_t bytes){
        lastRecv = DateTimeUtils::now();
        std::cout << "onRead:" << error << " len:" << bytes << std::endl;
        printBuff("<<", recvBuff, bytes);
        if (!error){           
            int ind = 0;
            size_t offset = 0;
            if (bytes > 0 && bytes < MAX_BUFF_SIZE){
                while(offset < bytes 
                    && ind >= 0 
                    && (ind = parser.parse(recvBuff + offset, bytes-offset)) > 0){
                    offset = ind;
                    std::cout << "parsing: " << offset << " " << ind << "" << bytes <<std::endl; 
                };                 
            }
        } else {
            log("Error: onRead error:" + std::string(error.message()) + " sock:" + std::to_string(sock.is_open()) );
        }
        init_read();
    }
    
    void init_read(){
        if (sock.is_open() && isRunning()){
            sock.async_read_some(boost::asio::buffer(recvBuff, MAX_RECV_SIZE), boost::bind(&Session::onRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    }   
};
};
