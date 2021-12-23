#pragma once
#include <iostream>
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
    static size_t pack(char*buff, const size_t maxLen, const size_t offset, const T& value){
        if (offset+sizeof(T) >= maxLen){
            return 0;
        }
        memcpy(buff+offset,(&value), sizeof(T));
        return offset+sizeof(T);
    }

    static size_t pack(char*buff, const size_t maxLen, const size_t offset, const float& value){
        constexpr size_t value_size = sizeof(uint64);// + sizeof(int8);
        if (offset + value_size >= maxLen){
            return 0;
        }
        const uint64 mantissa = static_cast<uint64>(value * 100000.0);
        const int8 exp = -5;
        memcpy(buff + offset, &mantissa, sizeof(uint64));
        //memcpy(buff + offset + sizeof(uint64), &exp, sizeof(int8));
        return offset + value_size;
    }

    static size_t packChar(char* buff ,const size_t maxLen, const size_t offset, const std::string& str, const size_t N = 20){
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
