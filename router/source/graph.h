#pragma once
#include <unordered_map>
#include <vector>
#include <limits.h>
#include "util.h"
#include "MicroBit.h"
#include <limits.h>
#include <edge.h>
#include "config.h"

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
ManagedString graph_to_json(std::unordered_map<struct edge, int> graph);
ManagedString topology_json(uint16_t ip);
std::vector<uint16_t> get_neighbours(uint16_t ip);
std::unordered_map<struct edge, int> remove_dead_nodes(std::unordered_map<struct edge, int> graph);
bool arcs_incoming(uint16_t ip, std::unordered_map<struct edge, int> graph);
std::vector<std::pair<edge, int>> get_neighbour_edges(uint16_t ip);
