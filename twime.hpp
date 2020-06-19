#pragma once

#include "session.hpp"
using namespace std;

namespace twime
{
class TwimeConnector{

 
    Session session;
    
   public:
    TwimeConnector() 
    : session()
    {}

    void setUser(const std::string mUser){
         session.setUser(mUser);
    }

    int connect(const std::string& mIp, const unsigned int& mPort){
          return session.connect(mIp, mPort);
    }

    int disconnect(){
          return session.disconnect();
    }
      
      bool isRunning(){
          return session.isRunning();
    }

      //int sendCommand(const CommandNos& nos){
      //    return session.sendCommand(nos);
    //}
};
};
