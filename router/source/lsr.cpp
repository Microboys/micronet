#include "lsr.h"
#include <algorithm>
#include <stack>

void get_path_for_node_recursive(std::unordered_map<struct edge, int>& input_graph,
                                        uint16_t current_node, uint16_t destination_node, int16_t current_cost,
                                        int16_t *best_cost, std::vector<uint16_t>& current_path,
                                        std::vector<uint16_t>& best_path, int8_t depth, int8_t max_depth) {
    if (depth >= max_depth) {
        return;
    }

    current_path.push_back(current_node);

    if (current_node == destination_node) {
        if (current_cost >= *best_cost) {
            *best_cost = current_cost;
            best_path = current_path;
        }
        current_path.pop_back();
        return;
    }

    for (auto& it : input_graph) {
        struct edge current_edge = it.first;
        if (current_edge.from == current_node) {
            get_path_for_node_recursive(input_graph, current_edge.to, destination_node,
                current_cost + it.second, best_cost, current_path, best_path, depth + 1, max_depth);
        }
    }

    current_path.pop_back();
}

std::vector<uint16_t> get_path_for_node(std::unordered_map<struct edge, int>& input_graph,
                                        uint16_t source, uint16_t destination) {

    std::vector<uint16_t> best_path;
    std::vector<uint16_t> current_path;
    int16_t best_cost = SHRT_MIN;

    if (source == destination) {
        return best_path;
    }

    get_path_for_node_recursive(input_graph, source, destination, SHRT_MIN, &best_cost,
        current_path, best_path, 0, input_graph.size() - 1);

    return best_path;
}
