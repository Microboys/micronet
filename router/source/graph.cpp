#include "graph.h"
#include "lsr.h"

std::unordered_map<struct edge, int> graph;
std::unordered_map<uint16_t, unsigned long> alive_nodes;

/* Updates node with the time of the last packet. */
void update_alive_nodes(uint16_t ip, unsigned long time) {
    alive_nodes[ip] = time;
}

/* Updates graph from ping response. If the ping came from a router that is
 * closer than CONNECTION_THRESHOLD, we add it to our graph or update its arc
 * value if it's already present. If it's farther away than DISCONNECTION_THRESHOLD,
 * we delete the arc from our graph.
 */
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

/* Updates graph from LSA packet. Treats the packet as ground truth - deletes
 * all outgoing arcs we currently know and replaces them with ones from packet.
 */
void update_graph(Packet* p) {
    uint16_t source_ip = p->source_ip;
    delete_all_edges(source_ip);
    for (auto it : p->graph) {
        graph[it.first] = it.second;
    }
}


void recalculate_graph(uint16_t source) {
    calculate_syn_tree(source, graph);
}

/* Prunes the graph. Deletes arcs to neighbours who are too far away and if we
 * have more than MAX_NEIGHBOURS neighbours, we take the strongest MAX_NEIGHBOURS
 * ones and delete the rest.
 */
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

/* Deletes all outgoing edges in the graph from a given IP. */
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

/* Prints graph layout to serial. */
void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph) {
    serial.printf("===== GRAPH =====\n\r");
    for (auto it : graph) {
        int distance = it.second;
        serial.printf("%i --> %i (distance: %i)\n\r", it.first.from, it.first.to, distance);
    }
    serial.printf("===== GRAPH =====\n");
}

/* Returns a graph consisting of all outgoing arcs from the given IP, which
 * can be immediately sent as an LSA packet.
 */
std::unordered_map<edge, int> get_lsa_graph(uint16_t ip) {
    std::unordered_map<edge, int> to_send;
    for (auto it : graph) {
        if (it.first.from == ip) {
            to_send[it.first] = it.second;
        }
    }
    return to_send;
}

/* Returns a vector of all neighbour IPs of a given IP.
 */
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


ManagedString sink_tree_to_json(std::unordered_map<struct edge, int> graph) {
    ManagedString result = "[";
    unsigned int index = 0;
    for (auto it = graph.begin(); it != graph.end(); ++it) {
        result = result + "{";
        result = result + format_attr("to", it->first.to);
        std::vector<uint16_t> path = get_full_path_for_node(it->first.to);
        result = result + format_attr("path", path, true);
        result = result + "}";
        index++;
        if (index < graph.size()) {
            result = result + ",";
        }
    }
    return result + "]";
}

ManagedString get_topology_json(uint16_t ip) {
    ManagedString result = "{";
    result = result + format_attr("type", "graph");
    result = result + format_attr("ip", ip);
    result = result + "\"graph\":" + graph_to_json(graph);
    result = result + "}" + SERIAL_DELIMITER;
    return result;
}

ManagedString path_json(uint16_t ip) {
    recalculate_graph(ip);
    ManagedString result = "{";
    result = result + format_attr("type", "sink-tree");
    result = result + format_attr("ip", ip);
    result = result + "\"sink-tree\":" + sink_tree_to_json(graph);
    return result + "}" + SERIAL_DELIMITER;
}

/* Prunes graph by deleting 'dead' routers and every arc that contains them.
 */
void remove_dead_nodes(unsigned long current_time) {
    std::unordered_set<uint16_t> dead_nodes = get_dead_nodes(current_time);

    std::unordered_map<struct edge, int> new_graph;
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        if (!contains(dead_nodes, cur_edge.from) && !contains(dead_nodes, cur_edge.to)) {
            new_graph.insert(it);
        }
    }
    graph = new_graph;
}

bool contains(std::unordered_set<uint16_t> set, uint16_t value) {
    return set.find(value) != set.end();
}

/* Returns 'dead' routers in the graph. These are defined as nodes from whom
 * we have not received a packet in the past NODE_LIFESPAN milliseconds. */
std::unordered_set<uint16_t> get_dead_nodes(unsigned long current_time) {
    std::unordered_set<uint16_t> dead_nodes;
    for (auto it : alive_nodes) {
        uint16_t ip = it.first;
        if ((current_time - it.second) > NODE_LIFESPAN) {
            dead_nodes.insert(ip);
        }
    }

    for (uint16_t ip : dead_nodes) {
        alive_nodes.erase(ip);
    }
    return dead_nodes;
}
