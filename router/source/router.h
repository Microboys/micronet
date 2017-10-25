#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"

#define NETWORK_TRANSMISSION_DELAY 100

void print_neighbours();
void onData(MicroBitEvent e);
void ping(MicroBitEvent e);
void send_message(MicroBitEvent e);
void broadcast(Packet p);
void setup();
int main();
