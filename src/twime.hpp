#pragma once

#include "session.hpp"
using namespace std;

namespace twime
{
class TwimeConnector {

   Session session;

   public:
    TwimeConnector() 
    : session()
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
};
};
