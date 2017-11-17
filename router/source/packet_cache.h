#pragma once
#include <unordered_set>
#include <queue>
#include "MicroBit.h"
#include "config.h"

bool packet_cache_contains(PacketBuffer buffer);
inline bool operator==(const PacketBuffer &pb1, const PacketBuffer &pb2) {
    for (int i = 0; i < PACKET_SIZE; i++) {
        if (pb1[i] != pb2[i]) {
            return false;
        }
    }
    return true;
}

namespace std {
    template <>
    struct hash<PacketBuffer> {
        size_t operator()(const PacketBuffer& b) const {

            const int p = 16777619;
            int hash = (int)2166136261;

            for (int i = 0; i < PACKET_SIZE; i++)
                hash = (hash ^ b[i]) * p;

            hash += hash << 13;
            hash ^= hash >> 7;
            hash += hash << 3;
            hash ^= hash >> 17;
            hash += hash << 5;
            return hash;
        }
    };
}
