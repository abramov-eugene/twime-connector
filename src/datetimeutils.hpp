#pragma once
#include <chrono>
#include "types.hpp"
namespace twime{
struct DateTimeUtils{

    static timestamp_t now() {
       auto secs = static_cast<std::chrono::seconds>(std::time(nullptr)).count();
       return static_cast<timestamp_t>(secs*1000000);
    }
    
    static long delta(const timestamp_t t1, const timestamp_t t2) {
        long delta  = t1 - t2;
        return static_cast<long>(delta / 1000000);
    }
};
};
