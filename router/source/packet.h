#pragma once
#include "util.h"
#include "MicroBit.h"

#define PACKET_SIZE 32
#define MAX_TTL 5

// TODO: fragmentation

/*
 * Packet structure:
 * Ping:
 *  p[0] = type,
 *  p[1-2] = source_ip,
 *  p[3-4] = imm_dest_ip
 *  p[5-31] = 0
 * 
 * LSA:
 *  p[0] = type,
 *  p[1] = ttl,
 *  p[2-31] = payload
 *  
 * Message:
 *  p[0] = type,
 *  p[1-2] = source_ip,
 *  p[3-4] = imm_dest_ip,
 *  p[5-6] = dest_ip,
 *  p[7] = timestamp,
 *  p[8] = ttl,
 *  p[9-31] = payload
 */

enum packet_type {
    PING = 0,
    LSA = 1,
    MESSAGE = 2,
    DNS = 3
};

struct router_info {
    uint16_t ip;
    int distance;
};

class Packet {
    public:
        packet_type ptype;
        uint16_t source_ip = 0;
        uint16_t imm_dest_ip = 0;
        uint16_t dest_ip = 0;
        uint8_t timestamp = 0;
        uint8_t ttl = MAX_TTL;
        uint8_t payload = 0;
        int rssi = 0;

        void print_packet(MicroBitSerial);
        Packet(PacketBuffer p, int rssi);
        Packet(packet_type ptype, uint16_t source_ip, uint16_t imm_dest_ip,
            uint16_t dest_ip, uint8_t timestamp, uint8_t ttl,
            uint8_t payload);
        Packet();
        PacketBuffer format();
        //static Packet ping_packet(uint16_t source_ip, uint16_t dest_ip);
        //static Packet lsa_packet(uint8_t ttl, uint8_t payload);
        //static Packet message_packet(uint16_t source_ip,
            //uint16_t imm_dest_ip, uint16_t dest_ip, uint8_t timestamp,
            //uint8_t ttl, uint8_t payload);
};
