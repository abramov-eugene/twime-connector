#pragma once
#include <string>
#include <cstring>
#include "datetimeutils.hpp"
#include "parserutils.hpp"
#include "types.hpp"
using namespace std;

namespace twime{ 
class FixMessage{
   FixMessageType type;
   uint16 blockLength;
   uint16 templateId;
public:
   static const uint16 schemaId = 19781;
   static const uint16 version = 3;
   
   FixMessage(const FixMessageType& mType)
   : type(mType)
   , blockLength(0)
   , templateId(mType)
   {}
   
   const FixMessageType getType() const{
        return type;
   }
   
   void setLengthMessage(unsigned int len){
       blockLength = 64 + len;
   }
   
   virtual int decode(const char *buff, const size_t size) = 0; //parsing
   virtual size_t encode(char *buff, const size_t maxLen)
   {
       size_t offset = 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, blockLength);
       if (offset == 0)
           return 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, templateId);
       if (offset == 0)
           return 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, schemaId);
       if (offset == 0)
           return 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, version);       
       return offset;      
   }//make message
    
    
};

/*class NewOrderSingle : public FixMessage{
   public:
   NewOrderSingle(std::string clOrdId, std::string symbol, Side side, OrdType ordType, Tif tif, double price) : FixMessage(FixMessageType::NEW_SINGLE){
   }   
};*/

class Establish : public FixMessage{
    uint32 keepAlive;
    std::string userName;
    timestamp_t timestamp;
public:
    Establish() 
    : FixMessage(FixMessageType::ESTABLISH)
    , keepAlive(0)
    , userName("")
    , timestamp(0){        
    }
    
    Establish(unsigned int mKeepAlive, std::string mUserName) 
    : FixMessage(FixMessageType::ESTABLISH)
    , keepAlive(mKeepAlive)
    , userName(mUserName)
    , timestamp(DateTimeUtils::now()){        
    }
    
    size_t encode(char* buff, const size_t len){
        setLengthMessage(96+20);
        size_t offset = FixMessage::encode(buff, len);
        offset += ParserUtils::pack<uint32>(buff, len, offset, keepAlive);
        if (offset == 0)
            return 0;
        offset += ParserUtils::packChar(buff, len, offset, userName, 20);
        if (offset == 0)
            return 0;
        offset += ParserUtils::pack<timestamp_t>(buff, len, offset, timestamp);
        return offset;
    }
    
    int decode(const char *, const size_t){
        return 0;
    }
};

class EstablishmentAck: public FixMessage{
    timestamp_t reqTimestamp;
    uint32 keepAlive;
    uint64 nextSeqNum;
public:
    EstablishmentAck()
    : FixMessage(FixMessageType::ESTABLISHMENT_ACK)
    , reqTimestamp(0)
    , keepAlive(0)
    , nextSeqNum(0){}
    
    EstablishmentAck(timestamp_t mReqTimestamp, uint32 mKeepAlive, uint64 mNextSeqNum)
    : FixMessage(FixMessageType::ESTABLISHMENT_ACK)
    , reqTimestamp(mReqTimestamp)
    , keepAlive(mKeepAlive)
    , nextSeqNum(mNextSeqNum){        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
    
    int decode(const char *buff, const size_t size){
        size_t offset = 4 * sizeof(uint16); 
        offset = ParserUtils::unpack<timestamp_t>(buff, size, offset, &reqTimestamp);
        if (offset == 0)
            return -1;
        offset = ParserUtils::unpack<uint32>(buff, size, offset, &keepAlive);
        if (offset == 0)
            return -1;
        offset = ParserUtils::unpack<uint64>(buff, size, offset, &nextSeqNum);
        return offset;
    }
};

class Terminate : public FixMessage{
    uint8 terminationCode;
public:
    Terminate()
    : FixMessage(FixMessageType::TERMINATE)
    , terminationCode(0){}
    
    
    Terminate(uint8 mTerminationCode)
    : FixMessage(FixMessageType::TERMINATE)
    , terminationCode(mTerminationCode){        
    }
    
    size_t encode(char* buff, const size_t len){
        setLengthMessage(8);
        size_t offset = FixMessage::encode(buff, len);
        offset += ParserUtils::pack<uint8>(buff, len, offset, terminationCode);
        return offset;
    }
    
    int decode(const char *, const size_t){
        return 0;
    }
};

class EstablishmentReject: public FixMessage{
    long reqTimestamp;
    unsigned int rejectCode;
public:
    
    EstablishmentReject()
    : FixMessage(FixMessageType::ESTABLISHMENT_REJECT)
    , reqTimestamp(0)
    , rejectCode(0){        
    }
    
    EstablishmentReject(long mReqTimestamp, unsigned int mRejectCode)
    : FixMessage(FixMessageType::ESTABLISHMENT_REJECT)
    , reqTimestamp(mReqTimestamp)
    , rejectCode(mRejectCode){        
    }
    
    size_t encode(char*, const size_t){
        return 0;
    }
    
    int decode(const char *, const size_t){
        return 0;
    }
};

class Sequence : public FixMessage{
    uint64 nextSeqNum;
    public:
    Sequence()
    : FixMessage(FixMessageType::SEQUENCE)
    , nextSeqNum(0){       
    }
    
    Sequence(uint64 mNextSeqNum)
    : FixMessage(FixMessageType::SEQUENCE)
    , nextSeqNum(mNextSeqNum){        
    }
    
    size_t encode(char* buff, const size_t len){
        setLengthMessage(64);
        size_t offset = FixMessage::encode(buff, len);
        offset += ParserUtils::pack<uint64>(buff, len, offset, nextSeqNum);
        return offset;
    }
    
    int decode(const char *, const size_t){
        return 0;
    }
};

};
