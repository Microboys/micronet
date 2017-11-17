#include "packet_cache.h"
#include "packet.h"

std::unordered_set<PacketBuffer> cache;
std::queue<PacketBuffer> fifo_queue;
bool packet_cache_contains(PacketBuffer buffer) {
    bool contains = cache.find(buffer) != cache.end();
    if (!contains) {
        if (fifo_queue.size() > CACHE_SIZE) {
            PacketBuffer to_be_removed = fifo_queue.front();
            fifo_queue.pop();
            to_be_removed[F_LSA_TTL] = 0;
            cache.erase(to_be_removed);
        }
        cache.insert(buffer);
        fifo_queue.push(buffer);
        return false;
    }
    return true;
}
