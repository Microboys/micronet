#pragma once
enum packet_type {
    PING = 0,
    LSA = 1,
    MESSAGE = 2,
    DNS = 3
};

struct router_info {
    uint16_t ip;
    int distance;
};
