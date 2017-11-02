#pragma once
#include <unordered_map>
#include "util.h"
#include "MicroBit.h"
#include <limits.h>

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
void update_graph(Packet* p);
void update_graph(uint16_t from, uint16_t to, int distance);
void delete_all_edges(uint16_t ip);
void delete_extra_neighbours(uint16_t ip);
void print_graph(MicroBitSerial serial);
void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph);
std::unordered_map<edge, int> get_lsa_graph(uint16_t ip);
ManagedString graph_to_json(std::unordered_map<struct edge, int> graph);
ManagedString topology_json();
