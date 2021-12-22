#pragma once
#include <iostream>
#include <fstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "messages.hpp"
#include "parser.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

namespace twime
{
enum Status {DISCONNECTED=0, CONNECTING=1, CONNECTED=2, AUTHORIZED=3, DISCONNECTING=4};

class Session {
   Status status;
   io_service service;
   deadline_timer timer;
   tcp::socket sock;
   boost::thread serviceThread;
   static const size_t MAX_BUFF_SIZE = 2048;
   static const size_t MAX_RECV_SIZE = 2048;
   char* sendBuff;
   char* recvBuff;
   string userName;
   unsigned int keepAlive;
   long reestablishTimeoutSec;
   long reconnectTimeoutSec;
   long lastConnecting;
   long lastEstablish;
   long lastSended;
   uint64 nextSeqNum;
   uint64 sendSeqNum;
   long lastRecv;
   unsigned int keepAliveServer;
   Parser<Session> parser;
   string ip;
   unsigned int port;
   std::ofstream outStream;
   public:
   Session() 
   : status(Status::DISCONNECTED)
   , service()
   , timer(service, boost::posix_time::seconds(2))
   , sock(service)
   , serviceThread(boost::bind(&io_service::run, &service))
   , sendBuff(nullptr)
   , recvBuff(nullptr)
   , userName("")
   , keepAlive(10000)
   , reestablishTimeoutSec(5)
   , reconnectTimeoutSec(10)
   , lastConnecting(0)
   , lastEstablish(DateTimeUtils::now())
   , lastSended(DateTimeUtils::now())
   , nextSeqNum(0)
   , sendSeqNum(UINT64_MAX)
   , lastRecv(DateTimeUtils::now())
   , keepAliveServer(30000)/*get from Establish Ack*/
   , parser(*this)
   , ip("")
   , port(0)
   , outStream()
   {
       sendBuff = new char [MAX_BUFF_SIZE];
       recvBuff = new char [MAX_RECV_SIZE];
       timer.async_wait(boost::bind(&Session::onTimer, this));
       outStream.open("twime.log", std::ofstream::out);
   }
   
   ~Session() {
       timer.cancel();
       service.stop();
       serviceThread.join();
       outStream.close();
       if (sendBuff)
           delete [] sendBuff;
       if (recvBuff)
           delete [] recvBuff;
   }

    void setUser(const string& mUser) {
        userName = mUser;
    }

    void log(const string& msg) {
        outStream << "[" << time(nullptr) << "]" << msg << endl;
        outStream.flush();
    }

    void onTimer() {
        timestamp_t now = DateTimeUtils::now();
        switch (status){
            case Status::CONNECTING:{
                log("Connecting...");
                if (DateTimeUtils::delta(now, lastConnecting) > reconnectTimeoutSec){
                    tcp::endpoint endpoint(address::from_string(ip),port);
                    log("Connect:" + ip + ":" + std::to_string(port));
                    try{
                        sock.connect(endpoint);
                    }
                    catch(const std::exception& ex){
                        log("Connection error:" + string( ex.what()));
                        sock.close();
                    }
                    lastConnecting = DateTimeUtils::now();
                }
                if (sock.is_open()){
                    status = Status::CONNECTED;
                    sock.set_option(tcp::no_delay{true});
                    init_read();
                }
                break;
            }
            case Status::CONNECTED:{
                if (DateTimeUtils::delta(now, lastEstablish) > reestablishTimeoutSec){
                    log("Establish sended");
                    Establish est(keepAlive, userName);
                    sendCommand(est);
                    lastEstablish = DateTimeUtils::now();
                }
                break;
            }
            case Status::AUTHORIZED:{
                timestamp_t now = DateTimeUtils::now();
                if (DateTimeUtils::delta(now, lastSended) >= (keepAlive/1000)-1){
                    log("Heartbeat sended");
                    Sequence seq(sendSeqNum);
                    sendCommand(seq);
                    lastSended = DateTimeUtils::now();
                }
                if (keepAliveServer != 0 && DateTimeUtils::delta(now, lastRecv) >=2*keepAliveServer/1000){
                    log("Heartbeat from server missed");
                    close();
                }
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

   int connect(const string& mIp, const unsigned int& mPort) {
        port = mPort;
        ip = mIp;
        if (status == Status::DISCONNECTED) {
            status = Status::CONNECTING;
            log("Connecting state");
        }
        return 0;
   }

   int disconnect() {
        log("Disconnect");
        if (status == Status::AUTHORIZED) {
            Terminate term(0);
            sendCommand(term);
            log("Terminate sended");
        }
        else if (status == Status::CONNECTED 
                || status == Status::CONNECTING) {
            boost::system::error_code ignored;
            sock.close(ignored);
        }
        status = Status::DISCONNECTING;
        return 0;
   }

   bool isRunning() {
       return status != Status::DISCONNECTED;
   }

   int send(const char *buff, const int len) {
       printBuff(">>", buff, len);
       boost::asio::write(sock, boost::asio::buffer(buff, len));
       lastSended = DateTimeUtils::now();
       return 0;
   }

   template<class MessageType>
   int sendCommand(MessageType& command) {
        if (isConnected()) {
            int len = command.encode(sendBuff, MAX_BUFF_SIZE);
            if (len > 0)
                return send(sendBuff, len);
            else
                log("Error: Can't encode command");
        } else {
            log("Error: Can't send message. Status is not ready");
        }
        return -1;
   }

    void onError(const string error) {
        log("Error message parsing:" + error);
    }

    void onMessage(const shared_ptr<EstablishmentReject> reject){
        log("EstalishReject received. Reject:" + to_string(reject->getRejectCode()));
        disconnect();
    }

    void onMessage(const shared_ptr<Sequence>){
        log("Heartbeat received");
    }

    void onMessage(const shared_ptr<Terminate> msg) {
        log("Terminate received: " + to_string(msg->getTerminationCode()));
        close();
    }

    void onMessage(const shared_ptr<EstablishmentAck> msg) {
        status = Status::AUTHORIZED;
        keepAliveServer = msg->getKeepAlive();
        nextSeqNum = msg->getNextSeqNum();
        log("EstablishmentAck received: " + to_string(keepAliveServer) );
    }

private:

    void close() {
        status = Status::DISCONNECTED;
        boost::system::error_code ignored;
        if (sock.is_open()) {
            sock.close(ignored);
        }
    }

    void printBuff(const char* temp, const char *buff, int len) {
        if (len == 0)
            return;
        outStream << temp << "Buff(" << len << "):";
        for(int i = 0; i< len; ++i) {
            unsigned char c = buff[i];
            outStream << std::setfill('0') << std::setw(3) << static_cast<uint16>(c) << ((i == 7) ? " = " : ":");
        }
        outStream << std::endl;
    }

    void onRead(const boost::system::error_code& error, size_t bytes){
        lastRecv = DateTimeUtils::now();
        printBuff("<<", recvBuff, bytes);
        if (!error) {
            int ind = 0;
            size_t offset = 0;
            if (bytes > 0 && bytes < MAX_BUFF_SIZE) {
                while(offset < bytes 
                    && ind >= 0 
                    && (ind = parser.parse(recvBuff + offset, bytes-offset)) > 0) 
                {
                    offset = ind;
                }
            }
        } else {
            log("Error: onRead error:" + string(error.message()) + " sock:" + to_string(sock.is_open()) );
        }
        init_read();
    }

    bool isConnected() {
        return status == Status::CONNECTED || status == Status::AUTHORIZED;
    }

    void init_read() {
        if (sock.is_open() && isConnected()) {
            sock.async_read_some(boost::asio::buffer(recvBuff, MAX_RECV_SIZE), 
                                 boost::bind(&Session::onRead, this, 
                                 boost::asio::placeholders::error, 
                                 boost::asio::placeholders::bytes_transferred));
        }
    }
};
};
