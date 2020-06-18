#pragma once
#include <cstring>
#include <memory>
#include "messages.hpp"
#include "parserutils.hpp"
#include "types.hpp"

namespace twime{

template<class Handler>
class Parser{   
   
    Handler handler;
    public:
    Parser(const Handler& mHandler)
    : handler(mHandler){        
    }
    
    int parser(char *buff, const size_t len){
        uint16 msgLen = 0;
        uint16 templateId = 0;
        uint16 schemaId = 0;
        uint16 version = 0;
        size_t offset = ParserUtils::unpack<uint16>(buff,len,offset,&msgLen);
        if (offset == 0)
           //onError();
           return -1;
        if (msgLen < len)
        {
            return 0; //need to wait the whole packet
        }
        offset = ParserUtils::unpack<uint16>(buff,len,offset,&templateId);
        offset = ParserUtils::unpack<uint16>(buff,len,offset,&schemaId);
        if (schemaId != FixMessage::schemaId){
            //onError
            return -1;
        }
        offset = ParserUtils::unpack<uint16>(buff,len,offset,&version);
        if (version != FixMessage::version){
            //onError
            return -1;
        }
        std::shared_ptr<FixMessage> message = nullptr;
        switch(templateId){
            case FixMessageType::ESTABLISHMENT_ACK:
                message = std::make_shared<EstablishmentAck>();
                break;
            case FixMessageType::ESTABLISHMENT_REJECT:
                message = std::make_shared<EstablishmentReject>();
                break;
            case FixMessageType::TERMINATE:
                message = std::make_shared<Terminate>();
                break;
            case FixMessageType::SEQUENCE:
                message = std::make_shared<Sequence>();
                break;
            default:
                handler.onError("Unknown type message");
                return -1;
                break;                
        }
        if(message->decode(buff, msgLen)){
            handler.onMessage(message);
            return offset + msgLen;
        }
        else {
            handler.onError("Can't parse message");
            return -1;
        }
    }
};

};
