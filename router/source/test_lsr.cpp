//
//  main.cpp
//  microbit
//
//  Created by vinamra agrawal on 04/11/2017.
//  Copyright © 2017 vinamra agrawal. All rights reserved.

#include "lsr.h"
#include <stdio.h>

std::unordered_map<struct edge, int> test_graph;

void add_to_graph(uint16_t from, uint16_t to, int distance) {
    test_graph[edge({from, to})] = distance;
}

void print_graph() {
    printf("===== graph =====\n");
    for (auto it : test_graph) {
        int distance = it.second;
        printf("%i --> %i (distance: %i)\n", it.first.from, it.first.to, distance);
    }
    printf("===== graph =====\n");
}

int test_main() {

    add_to_graph(1, 2, -4);
    add_to_graph(1, 3, -1);
    add_to_graph(3, 2, -2);
    add_to_graph(2, 5, -3);
    add_to_graph(3, 4, -2);
    add_to_graph(4, 5, -3);

//    calculate_sink_tree(1, test_graph);

    int test_failed = 0;

//    // Testing path returned
//    if (get_next_node(1) != 0){
//        test_failed++;
//        printf("Test Failed: wrong path for node 1 \n");
//    }
//
//    if (get_next_node(2) != 3){
//        test_failed++;
//        printf("Test Failed: wrong path for node 2 \n");
//    }
//
//    if (get_next_node(3) != 3){
//        test_failed++;
//        printf("Test Failed: wrong path for node 3 \n");
//    }
//
//    if (get_next_node(4) != 3){
//        test_failed++;
//        printf("Test Failed: wrong path for node 4 \n");
//    }
//
//    if (get_next_node(5) != 3){
//        test_failed++;
//        printf("Test Failed: wrong path for node 5 \n");
//    }
//
//    // Testing full path returned
//    {
//        std::vector<uint16_t> path = get_path_for_node(1);
//        if (path.size() != 0){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 1 \n");
//        }
//    }
//
//    {
//        std::vector<uint16_t> path = get_path_for_node(2);
//        if (path.at(0) != 3 || path.at(1) != 2){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 2 \n");
//        }
//    }
//
//    {
//        std::vector<uint16_t> path = get_path_for_node(3);
//        if (path.size() != 1){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 3 \n");
//        } else if (path.at(0) != 3) {
//            test_failed++;
//            printf("Test Failed: wrong full path for node 3 \n");
//        }
//    }
//
//    {
//        std::vector<uint16_t> path = get_path_for_node(4);
//        if (path.size() != 2){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 4 \n");
//        } else if (path.at(0) != 3 || path.at(1) != 4){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 4 \n");
//        }
//    }
//
//    {
//        std::vector<uint16_t> path = get_path_for_node(5);
//        if (path.size() != 3){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 5\n");
//        } else if (path.at(0) != 3 || path.at(1) != 2 || path.at(2) != 5){
//            test_failed++;
//            printf("Test Failed: wrong full path for node 5 \n");
//        }
//    }

    // new algo
    {
        std::vector<uint16_t> path = get_path_for_node(test_graph,1,1);
        if (path.size() != 0){
            test_failed++;
            printf("Test Failed: wrong full path for node 1 \n");
        }
    }

    {
        std::vector<uint16_t> path = get_path_for_node(test_graph,1,2);
        if (path.at(0) != 1 || path.at(1) != 3 || path.at(2) != 2){
            test_failed++;
            printf("Test Failed: wrong full path for node 2 \n");
        }
    }

    {
        std::vector<uint16_t> path = get_path_for_node(test_graph,1,3);
        if (path.size() != 2){
            test_failed++;
            printf("Test Failed: wrong full path for node 3 \n");
        } else if (path.at(0) != 1 || path.at(1) != 3) {
            test_failed++;
            printf("Test Failed: wrong full path for node 3 \n");
        }
    }

    {
        std::vector<uint16_t> path = get_path_for_node(test_graph,1,4);
        if (path.size() != 3){
            test_failed++;
            printf("Test Failed: wrong full path for node 4 \n");
        } else if (path.at(0) != 1 || path.at(1) != 3 || path.at(2) != 4){
            test_failed++;
            printf("Test Failed: wrong full path for node 4 \n");
        }
    }

    {
        std::vector<uint16_t> path = get_path_for_node(test_graph,1,5);
        if (path.size() != 4){
            test_failed++;
            printf("Test Failed: wrong full path for node 5\n");
        } else if (path.at(0) != 1 || path.at(1) != 3 || path.at(2) != 2 || path.at(3) != 5){
            test_failed++;
            printf("Test Failed: wrong full path for node 5 \n");
        }
    }

//    // Testing value returned to be correct
//    if (get_distance_for_node(1) != 0){
//        test_failed++;
//        printf("Test Failed: wrong distance for node 1 \n");
//    }
//
//    if (get_distance_for_node(2) != 3){
//        test_failed++;
//        printf("Test Failed: wrong distance for node 2 \n");
//    }
//
//    if (get_distance_for_node(3) != 1){
//        test_failed++;
//        printf("Test Failed: wrong distance for node 3 \n");
//    }
//
//    if (get_distance_for_node(4) != 3){
//        test_failed++;
//        printf("Test Failed: wrong distance for node 4 \n");
//    }
//
//    if (get_distance_for_node(5) != 6){
//        test_failed++;
//        printf("Test Failed: wrong distance for node 5 \n");
//    }

    printf("All Tests done test failed are: %i \n", test_failed);
    return 0;
}
