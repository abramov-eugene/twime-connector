#pragma once
#include <string>
#include <cstring>
#include "datetimeutils.hpp"
#include "parserutils.hpp"
#include "types.hpp"

using namespace std;

namespace twime {

const uint16 SCHEMA_ID = 19781;
const uint16 VERSION = 5;
constexpr size_t header_size = 4 * sizeof(uint16);

class FixMessage {
   FixMessageType type;
   uint16 blockLength;

public:
   static const uint16 schemaId = 19781;
   static const uint16 version = 5;
   
   FixMessage(const FixMessageType& mType)
   : type(mType)
   , blockLength(0)
   {}
   
   const FixMessageType getType() const {
        return type;
   }
   
   void setLengthMessage(unsigned int len) {
       blockLength = len;
   }

   size_t encode_header(char *buff, const size_t maxLen)
   {
       size_t offset = 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, blockLength);
       if (offset == 0)
           return 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, type);
       if (offset == 0)
           return 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, SCHEMA_ID);
       if (offset == 0)
           return 0;
       offset = ParserUtils::pack<uint16>(buff, maxLen - offset, offset, VERSION);
       return offset;
   }
};

class Establish : public FixMessage {
    timestamp_t timestamp;
    uint32 keepAlive;
    std::string userName;
    
public:
    Establish() 
    : FixMessage(FixMessageType::ESTABLISH)
    , timestamp(0)
    , keepAlive(0)
    , userName("")
    {
    }
    
    Establish(unsigned int mKeepAlive, std::string mUserName) 
    : FixMessage(FixMessageType::ESTABLISH)
    , timestamp(DateTimeUtils::now())
    , keepAlive(mKeepAlive)
    , userName(mUserName){
    }
    
    size_t encode(char* buff, const size_t len){
        setLengthMessage(12+20);
        size_t offset = FixMessage::encode_header(buff, len);
        offset = ParserUtils::pack<timestamp_t>(buff, len, offset, timestamp);
        if (offset == 0)
            return 0;
        offset = ParserUtils::pack<uint32>(buff, len, offset, keepAlive);
        if (offset == 0)
            return 0;
        offset = ParserUtils::packChar(buff, len, offset, userName, 20);
        return offset;
    }
    
    int decode(const char *, const size_t) {
        return 0;
    }
};

class EstablishmentAck: public FixMessage {
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
    , nextSeqNum(mNextSeqNum) {
    }
    
    uint32 getKeepAlive() const {
        return keepAlive;
    }
    
    uint64 getNextSeqNum() const {
        return nextSeqNum;
    }
    
    size_t encode(char*, const size_t) {
        return 0;
    }
    
    int decode(const char *buff, const size_t size){
        size_t offset = header_size;
        offset = ParserUtils::unpack<timestamp_t>(buff, size, offset, reqTimestamp);
        if (offset == 0)
            return -1;
        offset = ParserUtils::unpack<uint32>(buff, size, offset, keepAlive);
        if (offset == 0)
            return -1;
        offset = ParserUtils::unpack<uint64>(buff, size, offset, nextSeqNum);
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
    
    uint8 getTerminationCode() const{
        return terminationCode;
    }
    
    size_t encode(char* buff, const size_t len){
        setLengthMessage(1);
        size_t offset = FixMessage::encode_header(buff, len);
        offset = ParserUtils::pack<uint8>(buff, len, offset, terminationCode);
        return offset;
    }
    
    int decode(const char * buff, const size_t size){
        size_t offset = header_size; 
        offset = ParserUtils::unpack<uint8>(buff, size, offset, terminationCode);
        return offset;
    }
};

class EstablishmentReject: public FixMessage{
    timestamp_t reqTimestamp;
    uint8 rejectCode;
public:
    
    EstablishmentReject()
    : FixMessage(FixMessageType::ESTABLISHMENT_REJECT)
    , reqTimestamp(0)
    , rejectCode(0) {
    }
    
    EstablishmentReject(timestamp_t mReqTimestamp, uint8 mRejectCode)
    : FixMessage(FixMessageType::ESTABLISHMENT_REJECT)
    , reqTimestamp(mReqTimestamp)
    , rejectCode(mRejectCode) {
    }
    
    uint8 getRejectCode() const {
        return rejectCode;
    }
    
    size_t encode(char*, const size_t) {
        return 0;
    }
    
    int decode(const char *buff, const size_t size) {
        size_t offset = header_size;
        offset = ParserUtils::unpack<timestamp_t>(buff, size, offset, reqTimestamp);
        if (offset == 0)
            return -1;
        offset = ParserUtils::unpack<uint8>(buff, size, offset, rejectCode);
        return offset;
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
        setLengthMessage(8);
        size_t offset = FixMessage::encode_header(buff, len);
        offset = ParserUtils::pack<uint64>(buff, len, offset, nextSeqNum);
        return offset;
    }

    int decode(const char * buff, const size_t size){
        size_t offset = header_size;
        offset = ParserUtils::unpack<timestamp_t>(buff, size, offset, nextSeqNum);
        return offset;
    }
};

class NewOrderSingle : public FixMessage {

    uint64 clOrderId;
    uint64 clOrderLinkId;
    uint64 expDate;
    float price;
    uint32 securityId;
    uint32 orderQty;
    TimeInForce tif;
    Side side;
    ClientFlags flags;
    std::string account;

    public:

    NewOrderSingle()
    : FixMessage(FixMessageType::NEW_ORDER_SINGLE)
    {}

    NewOrderSingle(uint64 mClOrdId, uint64 mClOrdLinkId, uint32 mSecurityId, Side mSide, TimeInForce mTif, float mPrice, Qty mQty, const std::string mAccount,  uint64 mExpDate = 0)
    : FixMessage(FixMessageType::NEW_ORDER_SINGLE)
    , clOrderId{mClOrdId}
    , clOrderLinkId{mClOrdLinkId}
    , price{mPrice}
    , securityId{mSecurityId}
    , side{mSide}
    , tif{mTif}
    , orderQty{mQty}
    , flags{ClientFlags::DONT_CHECK_LIMITS}
    , account{mAccount}
    , expDate{mExpDate}
    {
    }
    
    size_t encode(char* buff, const size_t len) {
        setLengthMessage(8);//todo
        size_t offset = FixMessage::encode_header(buff, len);
        offset = ParserUtils::pack<uint64>(buff, len, offset, clOrderId);
        if (offset == 0)
           return -1;
        if (tif == TimeInForce::GTD) {
           offset = ParserUtils::pack<uint64>(buff, len, offset, expDate);
        }
        offset = ParserUtils::pack<float>(buff, len, offset, price);
        offset = ParserUtils::pack<uint32>(buff, len, offset, securityId);
        offset = ParserUtils::pack<uint32>(buff, len, offset, clOrderLinkId);
        offset = ParserUtils::pack<uint32>(buff, len, offset, orderQty);
        offset = ParserUtils::pack<TimeInForce>(buff, len, offset, tif);
        offset = ParserUtils::pack<Side>(buff, len, offset, side);
        offset = ParserUtils::pack<ClientFlags>(buff, len, offset, flags);
        offset = ParserUtils::packChar(buff, len, offset, account.c_str(), 7);
        return offset;
    }

    int decode(const char * buff, const size_t size) {
        return 0;
    }
};

};
