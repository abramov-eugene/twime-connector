#pragma once
#include <iostream>
#include <fstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

using namespace std;

namespace twime
{

class Logger {
   std::ofstream outStream;
   public:
    Logger(const std::string& fileName) 
    : outStream()
    {
       outStream.open(fileName, std::ofstream::out);
    }

    ~Logger() {
       if (outStream.is_open()) {
          outStream.close();
       }
    }

    void log(const string& msg) {
        outStream << "[" << time(nullptr) << "]" << msg << endl;
        outStream.flush();
    }

    void printBuff(const char* temp, const char *buff, int len) {
        if (len == 0)
            return;
        outStream << temp << "Buff(" << std::dec << len << "):";
        for(int i = 0; i< len; ++i) {
            unsigned char c = buff[i];
            outStream << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint16>(c) << ((i == 7) ? " = " : ":");
        }
        outStream << std::dec << std::endl;
    }

};
};
