#include "graph.h"

// TODO: Decide mapped distance type.
std::unordered_map<struct edge, int> graph;

int MAX_NEIGHBOURS = 3;

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

// <node, path node>
// <node, distance>
// <node, boolean computation done>
std::unordered_map<uint16_t,uint16_t> path_for_node;
std::unordered_map<uint16_t,int> distance_for_node;
std::unordered_map<uint16_t,bool> if_computed;

void calculate_syn_tree(uint16_t source) {

    distance_for_node[source] = 0;

    //get min distance and edge for next iteration
    uint16_t min_edge = 0;

    //get distance and write path for neighbours
    for (auto it : graph) {
        struct edge cur_edge = it.first;
        if (cur_edge.from == source) {
            int distance = abs(it.second);
            distance_for_node[cur_edge.to] = distance;
            path_for_node[cur_edge.to] = cur_edge.to;
        }
    }
    if_computed[source] = true;

    //write iterative for rest
    while(true){
      min_edge = find_min_edge();
      if (min_edge == 0){
        //loop complete
        break;
      }

      for (auto it : graph) {
        struct edge cur_edge = it.first;
          if (cur_edge.from == min_edge) {
              int distance = abs(it.second) + distance_for_node[min_edge];

              // if value not initialized in the graph
              auto it = distance_for_node.find(cur_edge.to);
              if (it == distance_for_node.end()){
                distance_for_node[cur_edge.to] = INT_MAX;
                if_computed[cur_edge.to] = false;
              }

              if (distance < distance_for_node[cur_edge.to]){
                distance_for_node[cur_edge.to] = distance;
                path_for_node[cur_edge.to] = path_for_node[min_edge];
              }
          }
       }
       if_computed[min_edge] = true;
    }
}

uint16_t find_min_edge() {
  int min_distance = INT_MAX;
  uint16_t min_edge = 0;

  for (auto it : distance_for_node) {
      if (it.second <= min_distance && !if_computed[it.first]) {
          min_edge = it.first;
      }
  }
  return min_edge;
}

bool if_all_nodes_computed(std::unordered_map<uint16_t,bool> if_computated){
  for (auto it : if_computated) {
      if (!it.second) {
          return false;
      }
  }
  return true;
}

int16_t get_node_for_path(int16_t target){
  for (auto it : path_for_node) {
      if (it.first == target) {
          return it.second;
      }
  }
  return 0;
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
