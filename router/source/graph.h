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
bool if_all_nodes_computed(std::unordered_map<uint16_t,bool> if_computated);
uint16_t find_min_edge();
void calculate_syn_tree(uint16_t source);
int16_t get_path_for_node(int16_t target);
void print_graph(MicroBitSerial serial);
void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph);
std::unordered_map<edge, int> get_lsa_graph(uint16_t ip);
