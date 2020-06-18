#pragma once
#include <string>
#include "datetimeutils.hpp"
using namespace std;

namespace twime{ 
enum FixMessageType {ESTABLISH=5000, ESTABLISHMENT_ACK=5001,ESTABLISHMENT_REJECT=5002,TERMINATE=5003,SEQUENCE=5006, NEW_SINGLE=6000,};
enum Tif{IOC,GTC,FOK};
enum OrdType{LIMIT,MARKET};
enum Side {BUY, SELL};

class FixMessage{
   FixMessageType type;
   
   public:
   FixMessage(const FixMessageType& mType)
   : type(mType)
   {
   }
   
   const FixMessageType getType() const{
        return type;
   }
   
   //virtual bool decode(const char *buff, size_t size) = 0; //parsing
   virtual size_t encode(char *buff, const size_t maxLen) = 0; //make message
};

/*class NewOrderSingle : public FixMessage{
   public:
   NewOrderSingle(std::string clOrdId, std::string symbol, Side side, OrdType ordType, Tif tif, double price) : FixMessage(FixMessageType::NEW_SINGLE){
   }   
};*/

class Establish : public FixMessage{
    unsigned int keepAlive;
    std::string userName;
    long timestamp;
public:
    Establish(unsigned int mKeepAlive, std::string mUserName) : FixMessage(FixMessageType::ESTABLISH)
    , keepAlive(mKeepAlive)
    , userName(mUserName)
    , timestamp(DateTimeUtils::now()){        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
};

class EstablishmentAck: public FixMessage{
    long reqTimestamp;
    unsigned int keepAlive;
    unsigned long nextSeq;
public:
    EstablishmentAck(long mReqTimestamp, unsigned int mKeepAlive, unsigned long mNextSeqNum)
    : FixMessage(FixMessageType::ESTABLISHMENT_ACK)
    , reqTimestamp(mReqTimestamp)
    , keepAlive(mKeepAlive)
    , nextSeq(mNextSeqNum){        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
};

class Terminate : public FixMessage{
    unsigned int terminationCode;
public :
    Terminate(unsigned int mTerminationCode)
    : FixMessage(FixMessageType::TERMINATE)
    , terminationCode(mTerminationCode){
        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
};

class EstablishmentReject: public FixMessage{
    long reqTimestamp;
    unsigned int rejectCode;
public:
    EstablishmentReject(long mReqTimestamp, unsigned int mRejectCode)
    : FixMessage(FixMessageType::ESTABLISHMENT_REJECT)
    , reqTimestamp(mReqTimestamp)
    , rejectCode(mRejectCode){        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
};

class Sequence : public FixMessage{
    unsigned long nextSeqNum;
    public:
    Sequence(unsigned long mNextSeqNum)
    : FixMessage(FixMessageType::SEQUENCE)
    , nextSeqNum(mNextSeqNum){        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
};

};
