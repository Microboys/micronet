#pragma once
#include <unordered_map>
#include "util.h"
#include "MicroBit.h"

class Packet;

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

#include "packet.h"

//void update_graph(PacketBuffer& p);
void update_graph(std::unordered_map<struct edge, int> &graph, Packet* p);
void update_graph(std::unordered_map<struct edge, int> &graph, uint16_t from, uint16_t to, int distance);
void send_new_graph(int ttl);
void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph);
