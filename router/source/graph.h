#pragma once
#include <unordered_map>
#include "util.h"
#include "MicroBit.h"
#include <limits.h>
#include <edge.h>

class Packet;


#include "packet.h"

//void update_graph(PacketBuffer& p);
void update_graph(Packet* p);
void update_graph(uint16_t from, uint16_t to, int distance);
void delete_all_edges(uint16_t ip);
void delete_extra_neighbours(uint16_t ip);
bool if_all_nodes_computed(std::unordered_map<uint16_t,bool> if_computated);
void print_graph(MicroBitSerial serial);
void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph);
std::unordered_map<edge, int> get_lsa_graph(uint16_t ip);
