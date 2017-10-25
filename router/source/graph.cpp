#include "graph.h"

// Updates graph from ping response
void update_graph(std::unordered_map<struct edge, int> &graph, uint16_t from, uint16_t to, int distance) {
    graph[edge({from, to})] = distance;
}

void update_graph(std::unordered_map<struct edge, int> &graph, Packet* p) {
    for (auto it : p->graph) {
        graph[it.first] = it.second;
    }
}

void send_new_graph(int ttl) {
    //TODO: propogate the packet with new ttl
    //NOTE: here ttl decide the range of the graph
}

void print_graph(MicroBitSerial serial, std::unordered_map<struct edge, int> input) {
    serial.printf("===== GRAPH =====\n\r");
    for (auto it : input) {
        int distance = it.second;
        serial.printf("%i --> %i (distance: %i)\n\r", it.first.from, it.first.to, distance);
    }
}
