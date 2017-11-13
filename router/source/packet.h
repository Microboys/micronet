#pragma once
#include "util.h"
#include "graph.h"
#include "config.h"
#include "MicroBit.h"
#include <unordered_map>
#include <string>

#define BYTE_SIZE 8
#define F_PTYPE 0

// Ping
#define F_PING_SOURCE_IP 1
#define F_PING_IMM_DEST_IP 3

// LSA
#define LSA_EDGE_DATA_SIZE 3

#define F_LSA_TTL 1
#define F_LSA_SOURCE_IP 2
#define F_LSA_PAYLOAD 4

// Message
#define F_MESSAGE_SOURCE_IP 1
#define F_MESSAGE_IMM_DEST_IP 3
#define F_MESSAGE_DEST_IP 5
#define F_MESSAGE_TIMESTAMP 7
#define F_MESSAGE_TTL 8
#define F_MESSAGE_PAYLOAD 9

// DNS
#define F_DNS_TTL 1
#define F_DNS_SOURCE_IP 2
#define F_DNS_PAYLOAD 4

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
 *  p[2-3] = source_ip,
 *  p[4-31] = [neighbour_ip + rssi] (maximum of 9)
 *  
 * Message:
 *  p[0] = type,
 *  p[1-2] = source_ip,
 *  p[3-4] = imm_dest_ip,
 *  p[5-6] = dest_ip,
 *  p[7] = timestamp,
 *  p[8] = ttl,
 *  p[9-31] = payload
 *
 *  DNS:
 *  p[0] = type,
 *  p[1-2] = source_ip,
 *  p[3-31] = payload
 */

enum packet_type {
    PING = 0,
    LSA = 1,
    MESSAGE = 2,
    DNS = 3,
    INVALID = 4
};


class Packet {
    private:
        void encode_payload(PacketBuffer p, ManagedString payload, int start_index);
        ManagedString decode_payload(PacketBuffer p, int start_index);
        std::unordered_map<struct edge, int> decode_lsa(PacketBuffer p, uint16_t source_ip);
        void encode_lsa(PacketBuffer p, std::unordered_map<struct edge, int> graph);
    public:
        packet_type ptype = INVALID;
        uint16_t source_ip = 0;
        uint16_t imm_dest_ip = 0;
        uint16_t dest_ip = 0;
        uint8_t timestamp = 0;
        uint8_t ttl = INITIAL_TTL;
        ManagedString payload = "";
        int rssi = 0;

        // For LSA packets
        std::unordered_map<struct edge, int> graph;

        void print_packet(MicroBitSerial);
        Packet(PacketBuffer p, int rssi);
        Packet(packet_type ptype, uint16_t source_ip, uint16_t imm_dest_ip,
            uint16_t dest_ip, uint8_t timestamp, uint8_t ttl,
            ManagedString payload);
        Packet(packet_type ptype, uint16_t source_ip, uint16_t imm_dest_ip, uint16_t dest_ip, uint8_t timestamp, uint8_t ttl, std::unordered_map<struct edge, int> graph);
        Packet();
        PacketBuffer format();
        ManagedString to_json();
};
