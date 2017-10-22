#pragma once
struct edge {
    uint16_t from;
    uint16_t to;
};


// TODO: Decide mapped distance type.
std::map<struct edge, int> graph;
