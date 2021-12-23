#pragma once

namespace twime{

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;
typedef unsigned long timestamp_t;
typedef uint32 Qty;
typedef float Decimal5;

const uint64 nullValueUint64 = 18446744073709551615;


enum FixMessageType {
    ESTABLISH = 5000,
    ESTABLISHMENT_ACK = 5001,
    ESTABLISHMENT_REJECT = 5002,
    TERMINATE = 5003,
    SEQUENCE = 5006,
    NEW_ORDER_SINGLE = 6000
};

enum TimeInForce : uint8 {
    DAY = 0,
    IOC = 3,
    FOK = 4,
    GTD = 6
};

enum Side : uint8 {
    BUY = 1,
    SELL = 2,
    ALL_ORDERS = 89
};

enum ClientFlags : uint8 {
    DONT_CHECK_LIMITS = 0x1,
    NCC_REQUEST = 0x2
};

};
