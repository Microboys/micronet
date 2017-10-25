#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"

void print_neighbours();
void onData(MicroBitEvent e);
void ping(MicroBitEvent e);
void send_message(MicroBitEvent e);
void broadcast(Packet p);
void setup();
int main();
