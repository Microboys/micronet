#pragma once
#include <unordered_map>
#include "util.h"
#include "MicroBit.h"
struct edge {
    uint16_t from, to;
    bool operator==(const edge &o) const {
        return from == o.from && to == o.to;
    }
};

namespace std {
    template <>
    struct hash<edge> {
        size_t operator()(const edge& e) const {
            return ((hash<int>()(e.from)) ^ (hash<int>()(e.to)));
        }
    };
}

void update_graph(PacketBuffer& p);
void update_graph(uint16_t from, uint16_t to, int distance);
void send_new_graph(int ttl);
