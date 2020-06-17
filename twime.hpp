#pragma once

#include "session.hpp"
using namespace std;

namespace twime
{
class TwimeConnector{

   Session session;
    
   public:
      TwimeConnector() : session()
      {}
      
      int connect(const std::string& ip, const unsigned int& port){
          return session.connect(ip, port);
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
