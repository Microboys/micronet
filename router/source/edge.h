#pragma once
#include <unordered_map>
struct edge {
    uint16_t from, to;
    bool operator==(const edge &o) const {
        return from == o.from && to == o.to;
    }
};

namespace std {
    template <>
    struct hash<edge> {
        size_t operator()(const edge& e) const {
            return ((hash<int>()(e.from)) ^ (hash<int>()(e.to)));
        }
    };
}
