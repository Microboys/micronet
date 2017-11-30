#pragma once
#include "edge.h"
#include <limits.h>
#include <vector>

// uint16_t find_min_edge(std::unordered_set<uint16_t>& computed);
// uint16_t find_min_edge();
// void calculate_sink_tree(uint16_t source,
//                          std::unordered_map<struct edge, int>& input_graph);
// uint16_t get_next_node(uint16_t target);
// std::vector<uint16_t> get_path_for_node(uint16_t target);
// int get_distance_for_node(uint16_t target);

std::vector<uint16_t> get_path_for_node(std::unordered_map<struct edge, int>& input_graph,
                                        uint16_t source, uint16_t destination);
