#pragma once
#include <string>
#include <cstring>

namespace twime{
    
using namespace std;

struct ParserUtils{
        
    template<class T>
    static size_t unpack(const char* buff, const size_t maxLen, const size_t offset, T& value){
        //if (offset + sizeof(T) > maxLen){
        //    return 0;
        //}
        memcpy(&value, buff + offset, sizeof(T));
        return offset + sizeof(T);
    }
    
    template<class T>
    static size_t pack(char*buff, const size_t maxLen, const size_t offset, const T value){
       if (offset+sizeof(T) >= maxLen){
           return 0;
       }
       memcpy(buff+offset,(&value), sizeof(T));
       return offset+sizeof(T);
    }
    
    static size_t packChar(char* buff ,const size_t maxLen, const size_t offset, const std::string str, const size_t N = 20){
        const char* temp = str.c_str();
        const size_t len = str.size();
        if (len > N || offset + N >= maxLen)
            return 0;        
        memcpy(buff + offset, temp, len);
        memset(buff + offset + len, 0, std::max<int>(0,N-len));
        return offset + N;
    }
};

};
