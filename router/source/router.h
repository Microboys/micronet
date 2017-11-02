#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"
#define DELIMITER "\r\n"

#define GRAPH_REQUEST "GRAPH"
#define PING_REQUEST "PING"
#define IP_REQUEST "MYIP"

#define MESSAGE_REQUEST "MSG"
#define MESSAGE_REQUEST_LENGTH 3

#define MESSAGE_DELIMITER '\t'

void print_neighbours();
void onData(MicroBitEvent e);
void ping(MicroBitEvent e);
void send_message(uint16_t target, ManagedString message);
void send_lsa(MicroBitEvent e);
void broadcast(Packet p);
void setup();
int main();
