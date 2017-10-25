#include "graph.h"

// TODO: Decide mapped distance type.
std::unordered_map<struct edge, int> graph;

// Updates graph from another graph
void update_graph(PacketBuffer& p) {
    for (size_t i = 2; i < 31; i += 5) {
        uint16_t from = concat(p[i], p[i + 1]);
        uint16_t to = concat(p[i + 2], p[i + 3]);
        int distance = p[i + 4];
        struct edge new_edge = {from, to};
        graph[new_edge] = distance;
    }
    // TODO: print all values to check with graph
}

// Updates graph from ping response
void update_graph(uint16_t from, uint16_t to, int distance) {
    pair<edge, int> p((edge{from, to}), distance);
    graph.insert(p);
}

void send_new_graph(int ttl) {
    //TODO: propogate the packet with new ttl
    //NOTE: here ttl decide the range of the graph

}
