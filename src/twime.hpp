#pragma once

#include "session.hpp"
#include "logger.hpp"

using namespace std;

namespace twime
{
class TwimeConnector {

   Session<TwimeConnector> session;
   Logger& logger;

   public:
    TwimeConnector(Logger& mLogger) 
    : session(*this, mLogger)
    , logger(mLogger)
    {}

    void setUser(const string mUser) {
         session.setUser(mUser);
    }

    int connect(const string& mIp, const unsigned int& mPort) {
          return session.connect(mIp, mPort);
    }

    int disconnect() {
          return session.disconnect();
    }

    bool isRunning() {
          return session.isRunning();
    }

    template<class MessageType>
    int send(MessageType& message) {
       return session.sendCommand(message);
    }

    Status getStatus() {
       return session.getStatus();
    }


    void onMessage(const shared_ptr<BusinessMessageReject> msg) {
       auto error = "BusinessMessageReject. RejectCode:" + to_string(msg->getRejectCode());
       logger.log(error);
    }
};
};
