#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"
#include "config.h"
#include "event.h"
#include <vector>

void broadcast_name();
void broadcast(Packet* p);
unsigned long get_system_time();
void on_packet(MicroBitEvent e);
void handle_packet(Packet* p);
void handle_lsa(Packet* p);
void handle_message(Packet* p);
void handle_ping(Packet* p);
void process_packets();
void handle_dns(Packet* p);
void ping(MicroBitEvent e);
void send_message_via_routing(MicroBitEvent e);
void send_payload(uint16_t dest_ip, ManagedString message);
void send_lsa(MicroBitEvent e);
void send_dns(ManagedString name);
void send_graph_update();
ManagedString name_table_json();
void send_name_table();
void on_serial(MicroBitEvent e);
void init_serial_read();
MicroBitImage get_visual_id(uint16_t ip);

void update_network();
void update_desktop_app();
void setup(MicroBitEvent e);
int main();
