#include "lsr.h"
#include <algorithm>
#include <stack>

std::vector<uint16_t> get_path_for_node(std::unordered_map<struct edge, int>& input_graph,
                                        uint16_t source, uint16_t destination) {
    // calculate the dfs path, using stack and graph
    std::vector<uint16_t> dfs_path;
    std::stack<uint16_t> stack;
    stack.push(source);

    while (!stack.empty()) {
        uint16_t curr_node = stack.top();
        stack.pop();
        dfs_path.push_back(curr_node);
        if (curr_node == destination)
            continue;
        for (auto it : input_graph) {
            struct edge cur_edge = it.first;
            if (cur_edge.from == curr_node &&
                    std::find(dfs_path.begin(), dfs_path.end(), cur_edge.to) == dfs_path.end()) {
                stack.push(cur_edge.to);
            }
        }
    }

    // use above dfs path to claculate best path for min cost
    std::vector<uint16_t> curr_path;
    std::vector<uint16_t> best_path;
    uint16_t min_cost = SHRT_MAX;

    curr_path.push_back(dfs_path.at(0));

    //iterate through the dfs path
    for (uint8_t index = 1; index < dfs_path.size(); index++){

        // get next required edge by moving up the graph
        while (input_graph.count({curr_path.at(curr_path.size()-1), dfs_path.at(index)}) <= 0) {
            curr_path.pop_back();
        }
        curr_path.push_back(dfs_path.at(index));

        // if current path reach the destination update the cost and best path
        if (dfs_path.at(index) == destination){
            int16_t cost = 0;
            for (size_t i = 0; i < curr_path.size()-1; i++) {
                cost += input_graph[{curr_path.at(i),curr_path.at(i+1)}];
            }
            if (abs(cost) <= min_cost){
                min_cost = abs(cost);
                best_path = curr_path;
            }
        }
    }

    return best_path;
}

//// <node, path node>
//// <node, distance>
//// <node, boolean computation done>
//std::unordered_map<uint16_t,std::vector<uint16_t>> path_for_node;
//std::unordered_map<uint16_t,int> distance_for_node;
//
//void calculate_sink_tree(uint16_t source, std::unordered_map<struct edge, int>& input_graph) {
//    std::unordered_set<uint16_t> computed;
//
//    distance_for_node[source] = 0;
//
//    //get distance and write path for neighbours
//    for (auto it : input_graph) {
//        struct edge cur_edge = it.first;
//        if (cur_edge.from == source) {
//            int distance = abs(it.second);
//            distance_for_node[cur_edge.to] = distance;
//            std::vector<uint16_t> path;
//            path.push_back(cur_edge.to);
//            path_for_node[cur_edge.to] = path;
//        }
//    }
//    computed.insert(source);
//
//    //get min distance and edge for next iteration
//    uint16_t min_edge = 0;
//
//    //write iterative for rest
//    while(true){
//        min_edge = find_min_edge(computed);
//        if (min_edge == 0){
//            //loop complete
//            break;
//        }
//
//        for (auto it : input_graph) {
//            struct edge cur_edge = it.first;
//            if (cur_edge.from == min_edge) {
//                int distance = abs(it.second) + distance_for_node[min_edge];
//
//                // if value not initialized in the graph
//                auto iter = distance_for_node.find(cur_edge.to);
//                if (iter == distance_for_node.end()){
//                    distance_for_node[cur_edge.to] = INT_MAX;
//                }
//
//                if (distance < distance_for_node[cur_edge.to]){
//                    distance_for_node[cur_edge.to] = distance;
//                    if (path_for_node.find(min_edge) != path_for_node.end()) {
//                        std::vector<uint16_t> path = get_path_for_node(min_edge);
//                        path.push_back(cur_edge.to);
//                        path_for_node[cur_edge.to] = path;
//                    }
//                }
//            }
//        }
//        computed.insert(min_edge);
//    }
//}
//
//uint16_t find_min_edge(std::unordered_set<uint16_t>& computed) {
//    int min_distance = INT_MAX;
//    uint16_t min_edge = 0;
//
//    if (distance_for_node.empty())
//        return 0;
//
//    for (auto it : distance_for_node) {
//        auto got = computed.find(it.first);
//        if (abs(it.second) <= min_distance && got == computed.end()) {
//            min_edge = it.first;
//            min_distance = abs(it.second);
//        }
//    }
//    return min_edge;
//}
//
//uint16_t get_next_node(uint16_t target){
//    std::vector<uint16_t> path = get_path_for_node(target);
//    if (path.size() == 0)
//        return 0;
//    return path.front();
//}
//
//std::vector<uint16_t> get_path_for_node(uint16_t target){
//    if (path_for_node.find(target) != path_for_node.end()){
//        return path_for_node[target];
//    }
//    return std::vector<uint16_t>();
//}
//
//int get_distance_for_node(uint16_t target){
//    if (distance_for_node.find(target) != distance_for_node.end()) {
//        return distance_for_node[target];
//    } else {
//        return -1;
//    }
//}
