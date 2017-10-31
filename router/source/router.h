#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"
#define DELIMITER "\r\n"
#define GRAPH_REQUEST "GRAPH"

void print_neighbours();
void onData(MicroBitEvent e);
void ping(MicroBitEvent e);
void send_message(MicroBitEvent e);
void send_lsa(MicroBitEvent e);
void broadcast(Packet p);
void setup();
int main();
