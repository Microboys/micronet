#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"

#define NETWORK_TRANSMISSION_DELAY 100

uint16_t concat(uint8_t upper, uint8_t lower);
void print_neighbours();
void onData(MicroBitEvent e);
void update_graph(PacketBuffer& p);
void update_graph(uint16_t from, uint16_t to, int distance);
void send_new_graph(int ttl);
void ping(MicroBitEvent e);
void send_message(MicroBitEvent e);
void broadcast(Packet p);
void setup();
int main();
