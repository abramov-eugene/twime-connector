#pragma once
#include <chrono>
#include "types.hpp"
namespace twime{
struct DateTimeUtils{

    static timestamp_t now(int delta = 0){
       auto secs = static_cast<std::chrono::seconds>(std::time(nullptr)).count();
       return static_cast<timestamp_t>((secs+delta)*1000000);
    }    
};
};
