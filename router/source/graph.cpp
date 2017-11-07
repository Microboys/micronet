#include "graph.h"

// TODO: Decide mapped distance type.
std::unordered_map<struct edge, int> graph;

// Updates graph from ping response
void update_graph(uint16_t from, uint16_t to, int distance) {
    graph[edge({from, to})] = distance;
    delete_extra_neighbours(from);
}

void update_graph(Packet* p) {
    uint16_t source_ip = p->source_ip;
    delete_all_edges(source_ip);
    for (auto it : p->graph) {
        graph[it.first] = it.second;
    }
}

void delete_extra_neighbours(uint16_t ip) {
  int num_edges = 0;
  int min_strength  = INT_MAX;
  struct edge weakest_edge = {0,0};
  for (auto it : graph) {
      struct edge cur_edge = it.first;
      if (cur_edge.from == ip) {
          num_edges++;
          if (min_strength > it.second){
            min_strength = it.second;
            weakest_edge = cur_edge;
          }
      }
  }

  if (num_edges > MAX_NEIGHBOURS){
    graph.erase(weakest_edge);
    num_edges--;
  }

  if (num_edges > MAX_NEIGHBOURS) {
    delete_extra_neighbours(ip);
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
    result = result + "\"graph\":" + graph_to_json(graph);
    return result + "}\0";
}
