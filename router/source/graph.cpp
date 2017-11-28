#include "graph.h"
#include "lsr.h"

std::unordered_map<struct edge, int> graph;
std::unordered_map<uint16_t, unsigned long> alive_nodes;

static bool graph_lock = false;

void lock_graph() {
    if (!graph_lock) {
        graph_lock = true;
    } else {
        fiber_wait_for_event(GRAPH_EVENT_ID, GRAPH_EVENT_UNLOCK);
    }
}

void unlock_graph() {
    if (graph_lock) {
        graph_lock = false;
        MicroBitEvent evt(GRAPH_EVENT_ID, GRAPH_EVENT_UNLOCK);
        evt.fire();
    }
}

/* Updates node with the time of the last packet. */
void update_alive_nodes(uint16_t ip, unsigned long time) {
    alive_nodes[ip] = time;
}

/* Updates graph from ping response. If the ping came from a router that is
 * closer than CONNECTION_THRESHOLD, we add it to our graph or update its arc
 * value if it's already present.
 * Returns a bool indicating whether or not a new neighbour was added.
 */
bool update_graph(uint16_t from, uint16_t to, int distance) {
    bool new_neighbour = false;
    edge e({from, to});
    lock_graph();
    auto it = graph.find(e);
    if (it != graph.end()) {
        graph[e] = distance;
    } else if (distance >= CONNECTION_THRESHOLD) {
        graph[e] = distance;
        new_neighbour = true;
    }
    unlock_graph();
    delete_extra_neighbours(from);
    return new_neighbour;
}

/* Updates graph from LSA packet. Treats the packet as ground truth - deletes
 * all outgoing arcs we currently know and replaces them with ones from packet.
 * Returns whether the *topology* of the graph changes as a result of processing
 * the packet.
 */
bool update_graph(Packet* p) {
    uint16_t source_ip = p->source_ip;
    bool topology_change = false;

    std::unordered_set<struct edge> old_edges = delete_all_edges(source_ip);

    lock_graph();
    for (auto it : p->graph) {
        graph[it.first] = it.second;
        if (old_edges.count(it.first) != 1) {
          topology_change = true;
        }
    }
    unlock_graph();
    return topology_change;
}

// void recalculate_graph(uint16_t source) {
//     lock_graph();
//     calculate_sink_tree(source, graph);
//     unlock_graph();
// }

/* Prunes the graph. Deletes arcs to neighbours who are too far away and if we
 * have more than MAX_NEIGHBOURS neighbours, we take the strongest MAX_NEIGHBOURS
 * ones and delete the rest.
 */
void delete_extra_neighbours(uint16_t ip) {
    std::vector<std::pair<edge, int>> neighbours = get_neighbour_edges(ip);
    lock_graph();
    while (neighbours.size() > MAX_NEIGHBOURS) {
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
    unlock_graph();
}

/* Deletes all outgoing edges in the graph from a given IP. */
std::unordered_set<struct edge> delete_all_edges(uint16_t ip) {
    std::unordered_set<struct edge> deleted_edges;
    lock_graph();
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        if (cur_edge.from == ip) {
            deleted_edges.emplace(cur_edge);
            graph.erase(cur_edge);
        }
    }
    unlock_graph();
    return deleted_edges;
}

void print_graph(MicroBitSerial serial) {
    print_graph(serial, graph);
}

/* Prints graph layout to serial. */
void print_graph(MicroBitSerial serial, std::unordered_map<edge, int> graph) {
    serial.printf("===== GRAPH =====\n\r");
    lock_graph();
    for (auto it : graph) {
        int distance = it.second;
        serial.printf("%i --> %i (distance: %i)\n\r", it.first.from, it.first.to, distance);
    }
    unlock_graph();
    serial.printf("===== GRAPH =====\n");
}

/* Returns a graph consisting of all outgoing arcs from the given IP, which
 * can be immediately sent as an LSA packet.
 */
std::unordered_map<edge, int> get_lsa_graph(uint16_t ip) {
    std::unordered_map<edge, int> to_send;
    lock_graph();
    for (auto it : graph) {
        if (it.first.from == ip) {
            to_send[it.first] = it.second;
        }
    }
    unlock_graph();
    return to_send;
}


/* Returns next node from the routing algorithm.
 */
uint16_t get_next_node(uint16_t source, uint16_t target){
  lock_graph();
  std::vector<uint16_t> path = get_path_for_node(graph, source, target);
  unlock_graph();
  if (path.size() == 0)
        return 0;
  return path.at(1);
}

/* Returns a vector of all neighbour IPs of a given IP.
 */
std::vector<uint16_t> get_neighbours(uint16_t ip) {
    std::vector<uint16_t> neighbours;
    lock_graph();
    for (auto it : graph) {
        if (it.first.from == ip) {
            neighbours.push_back(it.first.to);
        }
    }
    unlock_graph();
    return neighbours;
}

std::vector<std::pair<edge, int>> get_neighbour_edges(uint16_t ip) {
    std::vector<std::pair<edge, int>> neighbours;
    lock_graph();
    for (auto it : graph) {
        if (it.first.from == ip) {
            neighbours.push_back(it);
        }
    }
    unlock_graph();
    return neighbours;
}

ManagedString graph_to_json(std::unordered_map<struct edge, int> graph) {
    ManagedString result = "[";
    unsigned int index = 0;
    lock_graph();
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
    unlock_graph();
    return result + "]";
}


ManagedString sink_tree_to_json(std::unordered_map<struct edge, int>& graph,
                                          uint16_t source) {
    ManagedString result = "[";
    unsigned int index = 0;
    std::vector<uint16_t> unique_nodes;
    lock_graph();
    for (auto it = graph.begin(); it != graph.end(); ++it){
      if (unique_nodes.count(it->first.to) <= 0) {
          unique_nodes.push_back(it->first.to);
      }
    }

    for (auto it = unique_nodes.begin(); it != unique_nodes.end(); ++it) {
        result = result + "{";
        result = result + format_attr("to", *it);
        std::vector<uint16_t> path = get_path_for_node(graph, source, *it);

        result = result + format_attr("path", path, true);
        result = result + "}";
        index++;
        if (index < unique_nodes.size()) {
            result = result + ",";
        }
    }
    unlock_graph();
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
    ManagedString result = "{";
    result = result + format_attr("type", "sink-tree");
    result = result + format_attr("ip", ip);
    result = result + "\"sink-tree\":" + sink_tree_to_json(graph, ip);
    return result + "}" + SERIAL_DELIMITER;
}

/* Prunes graph by deleting 'dead' routers and every arc that contains them.
 * Returns the nodes that were found to be dead. */
std::unordered_set<uint16_t> remove_dead_nodes(unsigned long current_time) {
    std::unordered_set<uint16_t> dead_nodes = get_dead_nodes(current_time);

    std::unordered_map<struct edge, int> new_graph;
    lock_graph();
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        if (!contains(dead_nodes, cur_edge.from) && !contains(dead_nodes, cur_edge.to)) {
            new_graph.insert(it);
        }
    }
    graph = new_graph;
    unlock_graph();
    return dead_nodes;
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
