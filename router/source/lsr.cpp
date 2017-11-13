#include "lsr.h"
#include <stdlib.h>

// <node, path node>
// <node, distance>
// <node, boolean computation done>
std::unordered_map<uint16_t,std::vector<uint16_t>> path_for_node;
std::unordered_map<uint16_t,int> distance_for_node;
std::unordered_map<uint16_t,bool> if_computed;

void calculate_syn_tree(uint16_t source, std::unordered_map<struct edge, int> input_graph) {
    distance_for_node[source] = 0;

    //get distance and write path for neighbours
    for (auto it : input_graph) {
        struct edge cur_edge = it.first;
        if (cur_edge.from == source) {
            int distance = abs(it.second);
            distance_for_node[cur_edge.to] = distance;
            std::vector<uint16_t> path;
            path.push_back(cur_edge.to);
            path_for_node[cur_edge.to] = path;
            if_computed[cur_edge.to] = false;
        }
    }
    if_computed[source] = true;

    //get min distance and edge for next iteration
    uint16_t min_edge = 0;

    //write iterative for rest
    while(true){
      min_edge = find_min_edge();
      if (min_edge == 0){
        //loop complete
        break;
      }

      for (auto it : input_graph) {
        struct edge cur_edge = it.first;
          if (cur_edge.from == min_edge) {
              int distance = abs(it.second) + distance_for_node[min_edge];

              // if value not initialized in the graph
              auto iter = distance_for_node.find(cur_edge.to);
              if (iter == distance_for_node.end()){
                distance_for_node[cur_edge.to] = INT_MAX;
                if_computed[cur_edge.to] = false;
              }

              if (distance < distance_for_node[cur_edge.to]){
                distance_for_node[cur_edge.to] = distance;
                if (path_for_node.find(min_edge) == path_for_node.end()) {
                    std::vector<uint16_t> path = path_for_node[min_edge];
                    path.push_back(cur_edge.to);
                    path_for_node[cur_edge.to] = path;
                }
                //std::vector<uint16_t> path = path_for_node[min_edge];
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
      if (abs(it.second) <= min_distance && !if_computed[it.first]) {
          min_edge = it.first;
          min_distance = abs(it.second);
      }
  }
  return min_edge;
}

uint16_t get_path_for_node(uint16_t target){
  for (auto it : path_for_node) {
      if (it.first == target) {
          std::vector<uint16_t> path = it.second;
          return path.front();
      }
  }
  return 0;
}

std::vector<uint16_t> get_full_path_for_node(uint16_t target){
  for (auto it : path_for_node) {
      if (it.first == target) {
          return it.second;
      }
  }
  return std::vector<uint16_t>();;
}

int get_distance_for_node(uint16_t target){
    if (distance_for_node.find(target) != distance_for_node.end()) {
        return distance_for_node[target];
    } else {
        return -1;
    }
}
