#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"
#include "config.h"

void broadcast(Packet* p);
unsigned long get_system_time();
void on_packet(MicroBitEvent e);
void handle_packet(Packet* p);
void handle_lsa(Packet* p);
void handle_message(Packet* p);
void handle_ping(Packet* p);
void ping(MicroBitEvent e);
void send_message_via_routing(MicroBitEvent e);
void send_payload(uint16_t dest_ip, ManagedString message);
void send_lsa(MicroBitEvent e);
void send_graph_update();
void on_serial(MicroBitEvent e);
void initSerialRead();
void update();
void setup(MicroBitEvent e);
int main();
