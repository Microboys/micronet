#pragma once
#include "MicroBit.h"
#include "packet.h"

#define PACKET_SIZE 32

uint16_t concat(uint8_t upper, uint8_t lower);
void print_packet(PacketBuffer p);
PacketBuffer format_packet(uint16_t source_ip, uint16_t imm_dest_ip,
        uint16_t dest_ip, uint8_t timestamp, packet_type ptype);
void print_neighbours();
void onData(MicroBitEvent e);
void onClick(MicroBitEvent e);
void setup();
int main();
