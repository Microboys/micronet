#include "graph.h"

// TODO: Decide mapped distance type.
std::unordered_map<struct edge, int> graph;

// Updates graph from ping response
void update_graph(uint16_t from, uint16_t to, int distance) {
    edge e({from, to});
    auto it = graph.find(e);
    if (it != graph.end()) {
        if (distance < DISCONNECTION_THRESHOLD) {
            graph.erase(it);
        } else {
            graph[e] = distance;
        }
    } else if (distance >= CONNECTION_THRESHOLD) {
        graph[e] = distance;
    }
    delete_extra_neighbours(from);
}

// Update graph from LSA packet
void update_graph(Packet* p) {
    uint16_t source_ip = p->source_ip;
    delete_all_edges(source_ip);
    for (auto it : p->graph) {
        graph[it.first] = it.second;
    }
}

void delete_extra_neighbours(uint16_t ip) {
    std::vector<std::pair<edge, int>> neighbours = get_neighbour_edges(ip);
    while (neighbours.size() > 0 && neighbours.size() > MAX_NEIGHBOURS) {
        int min_strength = neighbours[0].second;
        struct edge weakest_edge = neighbours[0].first;
        for (auto it : neighbours) {
            if (it.second < DISCONNECTION_THRESHOLD) {
                graph.erase(it.first);
            } else if (min_strength > it.second) {
                min_strength = it.second;
                weakest_edge = it.first;
            }
        }

        graph.erase(weakest_edge);
    }
}

void delete_all_edges(uint16_t ip) {
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        if (cur_edge.from == ip) {
            graph.erase(cur_edge);
        }
    }
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
    serial.printf("===== GRAPH =====\n");
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

std::vector<uint16_t> get_neighbours(uint16_t ip) {
    std::vector<uint16_t> neighbours;
    for (auto it : graph) {
        if (it.first.from == ip) {
            neighbours.push_back(it.first.to);
        }
    }
    return neighbours;
}

std::vector<std::pair<edge, int>> get_neighbour_edges(uint16_t ip) {
    std::vector<std::pair<edge, int>> neighbours;
    for (auto it : graph) {
        if (it.first.from == ip) {
            neighbours.push_back(it);
        }
    }
    return neighbours;
}

ManagedString graph_to_json(std::unordered_map<struct edge, int> graph) {
    ManagedString result = "[";
    unsigned int index = 0;
    for (auto it = graph.begin(); it != graph.end(); ++it) {
        result = result + "{";
        result = result + format_attr("from", it->first.from);
        result = result + format_attr("to", it->first.to);
        result = result + format_attr("distance", it->second, true);
        result = result + "}";
        index++;
        if (index < graph.size()) {
            result = result + ",";
        }
    }
    return result + "]";
}

ManagedString topology_json(uint16_t ip) {
    ManagedString result = "{";
    result = result + format_attr("type", "graph");
    result = result + format_attr("ip", ip);
    result = result + "\"graph\":" + graph_to_json(remove_dead_nodes(graph));
    return result + "}\0";
}

std::unordered_map<struct edge, int> remove_dead_nodes(std::unordered_map<struct edge, int> graph) {
    std::unordered_map<struct edge, int> new_graph;
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        if (arcs_incoming(cur_edge.from, graph)) {
            new_graph.insert(it);
        }
    }
    return new_graph;
}

bool arcs_incoming(uint16_t ip, std::unordered_map<struct edge, int> graph) {
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        uint16_t to = cur_edge.to;
        if (to == ip) {
            return true;
        }
    }
    return false;
}
