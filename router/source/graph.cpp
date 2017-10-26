#include "graph.h"

// TODO: Decide mapped distance type.
std::unordered_map<struct edge, int> graph;

// Updates graph from ping response
void update_graph(uint16_t from, uint16_t to, int distance) {
    graph[edge({from, to})] = distance;
}

void update_graph(Packet* p) {
    for (auto it : p->graph) {
        graph[it.first] = it.second;
    }
}

void send_new_graph(int ttl) {
    //TODO: propogate the packet with new ttl
    //NOTE: here ttl decide the range of the graph
}

void print_graph(MicroBitSerial serial) {
    print_graph(serial, graph);
}

void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph) {
    serial.printf("===== GRAPH =====\n\r");
    for (auto it : graph) {
        int distance = it.second;
        serial.printf("%i --> %i (distance: %i)\n\r", it.first.from, it.first.to, distance);
    }
}

std::unordered_map<edge, int> get_lsa_graph(uint16_t ip) {
    std::unordered_map<edge, int> to_send;
    for (auto it : graph) {
        if (it.first.from == ip) {
            to_send[it.first] = it.second;
        }
    }
    return to_send;
}
