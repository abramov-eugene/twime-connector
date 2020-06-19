#pragma once
#include <cstring>
#include <memory>
#include "messages.hpp"
#include "parserutils.hpp"
#include "types.hpp"

namespace twime{

template<class Handler>
class Parser{   
    Handler& handler;
    public:
    Parser(Handler& mHandler)
    : handler(mHandler){        
    }
    
    int parse(char *buff, const size_t len){
        size_t headerLen = 8;
        uint16 msgLen = 0;
        uint16 templateId = 0;
        uint16 schemaId = 0;
        uint16 version = 0;
        size_t offset = 0;
        offset = ParserUtils::unpack<uint16>(buff, len, offset, msgLen);
        std::cout << "MsgLen:" << msgLen << ":" << offset <<  std::endl;
        if (offset == 0)
           //onError();
           return -1;
        if (msgLen + headerLen < len)
        {
            return 0; //need to wait the whole packet
        }
        offset = ParserUtils::unpack<uint16>(buff,len,offset,templateId);
        std::cout << "TemplateId:" << templateId << std::endl;
        offset = ParserUtils::unpack<uint16>(buff,len,offset,schemaId);
        if (schemaId != FixMessage::schemaId){
            //onError
            return -1;
        }
        offset = ParserUtils::unpack<uint16>(buff,len,offset,version);
        if (version != FixMessage::version){
            //onError
            return -1;
        }
        switch(templateId){
            case FixMessageType::ESTABLISHMENT_ACK:
                decode<EstablishmentAck>(buff, msgLen);
                break;
            case FixMessageType::ESTABLISHMENT_REJECT:
                decode<EstablishmentReject>(buff, msgLen);
                break;
            case FixMessageType::TERMINATE:
                decode<Terminate>(buff, msgLen);
                break;
            case FixMessageType::SEQUENCE:
                decode<Sequence>(buff, msgLen);
                break;
            default:
                handler.onError("Unknown type message");
                return -1;
                break;                
        }        
        return offset + msgLen;
    }
    
private:
    template<class T>
    bool inline decode(const char* buff, size_t msgLen){
        auto message = std::make_shared<T>();                
        if(message->decode(buff, msgLen)){
            handler.onMessage(message);
            return true;
        }
        handler.onError("Can't parse message");
        return false;
    }
};

};
