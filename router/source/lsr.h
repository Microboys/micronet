#pragma once
#include "edge.h"
#include <limits.h>

uint16_t find_min_edge();
void calculate_syn_tree(uint16_t source,
   std::unordered_map<struct edge, int> graph);
uint16_t get_path_for_node(uint16_t target);
int get_distance_for_node(uint16_t target);
