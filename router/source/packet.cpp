#include "packet.h"

void Packet::print_packet(MicroBitSerial serial) {
    serial.printf(" ==== PACKET START ====\n\r");
    switch (ptype) {
        case PING:
            serial.printf("ptype: PING\n\r");
            serial.printf("source_ip: %i\n\r", this->source_ip);
            serial.printf("imm_dest_ip: %i\n\r", this->imm_dest_ip);
            break;
        case LSA:
            serial.printf("ptype: LSA\n\r");
            serial.printf("ttl: %i\n\r", this->ttl);
            serial.printf("source_ip: %i\n\r", this->source_ip);
            print_graph(serial, this->graph);
            break;
        case MESSAGE:
            serial.printf("ptype: MESSAGE\n\r");
            serial.printf("source_ip: %i\n\r", this->source_ip);
            serial.printf("imm_dest_ip: %i\n\r", this->imm_dest_ip);
            serial.printf("dest_ip: %i\n\r", this->dest_ip);
            serial.printf("ttl: %i\n\r", this->ttl);
            serial.printf("timestamp: %i\n\r", this->timestamp);
            serial.printf("payload: %s\n\r", this->payload.toCharArray());
            break;
        case DNS:
            serial.printf("ptype: DNS\n\r");
            //TODO
            break;
    }
    serial.printf(" ==== PACKET END ====\n\r");
}

// Outgoing packets
Packet::Packet(packet_type ptype, uint16_t source_ip, uint16_t imm_dest_ip,
        uint16_t dest_ip, uint8_t timestamp, uint8_t ttl,
        ManagedString payload) {

        this->source_ip = source_ip;
        this->imm_dest_ip = imm_dest_ip;
        this->dest_ip = dest_ip;
        this->timestamp = timestamp;
        this->ptype = ptype;
        this->ttl = ttl;
        this->payload = payload;
}

// Outgoing packets
Packet::Packet(packet_type ptype, uint16_t source_ip, uint16_t imm_dest_ip,
        uint16_t dest_ip, uint8_t timestamp, uint8_t ttl,
        std::unordered_map<edge, int> graph) {

        this->source_ip = source_ip;
        this->imm_dest_ip = imm_dest_ip;
        this->dest_ip = dest_ip;
        this->timestamp = timestamp;
        this->ptype = ptype;
        this->ttl = ttl;
        this->graph = graph;
}
// Incoming packets
Packet::Packet(PacketBuffer p, int rssi) {
    this->ptype = (packet_type)p[0];
    this->rssi = rssi;
    switch (ptype) {
        case LSA:
            this->ttl = p[1];
            this->source_ip = concat(p[2], p[3]);
            this->graph = decode_lsa(p, this->source_ip);
            break;
        case PING:
            this->source_ip = concat(p[1], p[2]);
            this->imm_dest_ip = concat(p[3], p[4]);
            break;
        case MESSAGE:
            this->source_ip = concat(p[1], p[2]);
            this->imm_dest_ip = concat(p[3], p[4]);
            this->dest_ip = concat(p[5], p[6]);
            this->timestamp = p[7];
            this->ttl = p[8];
            this->payload = decode_payload(p, MESSAGE_PAYLOAD_START);
            break;
        case DNS:
            //TODO
            break;
    }
}

PacketBuffer Packet::format() {
    PacketBuffer p = PacketBuffer(32);
    p[0] = this->ptype;

    switch (ptype) {
        case PING:
            p[1] = (uint8_t)(this->source_ip >> 8);
            p[2] = (uint8_t)(this->source_ip);
            p[3] = (uint8_t)(this->imm_dest_ip >> 8);
            p[4] = (uint8_t)(this->imm_dest_ip);
            break;
        case LSA:
            p[1] = this->ttl;
            p[2] = (uint8_t)(this->source_ip >> 8);
            p[3] = (uint8_t)(this->source_ip);
            encode_lsa(p, this->graph);
            break;
        case MESSAGE:
            p[1] = (uint8_t)(this->source_ip >> 8);
            p[2] = (uint8_t)(this->source_ip);
            p[3] = (uint8_t)(this->imm_dest_ip >> 8);
            p[4] = (uint8_t)(this->imm_dest_ip);
            p[5] = (uint8_t)(this->dest_ip >> 8);
            p[6] = (uint8_t)(this->dest_ip);
            p[7] = this->timestamp;
            p[8] = this->ttl;
            encode_payload(p, payload, MESSAGE_PAYLOAD_START);
            break;
        case DNS:
            //TODO
            break;
    }
    return p;
}

void Packet::encode_payload(PacketBuffer p, ManagedString payload, int start_index) {
    int string_index = 0;
    for (int i = start_index; i < PACKET_SIZE; i++) {
        char c = payload.charAt(string_index++);
        p[i] = c;
    }
}

ManagedString Packet::decode_payload(PacketBuffer p, int start_index) {
    char str[PACKET_SIZE];
    int str_i = 0;
    for (int i = start_index; i < PACKET_SIZE; i++) {
        str[str_i++] = p[i];
    }
    return ManagedString(str);
}

std::unordered_map<edge, int> Packet::decode_lsa(PacketBuffer p, uint16_t source_ip) {
    std::unordered_map<edge, int> graph;
    for (size_t i = LSA_PAYLOAD_START; i < PACKET_SIZE - LSA_EDGE_DATA_SIZE; i += LSA_EDGE_DATA_SIZE) {
        uint16_t to = concat(p[i], p[i + 1]);
        if (to == 0) {
            break;
        }
        int distance = p[i + 2];
        edge new_edge = {source_ip, to};
        pair<edge, int> p(new_edge, distance);
        graph.insert(p);
    }
    return graph;
}

void Packet::encode_lsa(PacketBuffer p, std::unordered_map<edge, int> graph) {
    int index = LSA_PAYLOAD_START;
    for (std::pair<edge, int> it : graph) {
        if (index > PACKET_SIZE - LSA_EDGE_DATA_SIZE) {
            uint16_t to = it.first.to;
            uint8_t to_upper = (uint8_t)(to >> 8);
            uint8_t to_lower = (uint8_t)(to);
            p[index++] = to_upper;
            p[index++] = to_lower;
            p[index++] = it.second;
        } else {
            break;
        }
    }
}

//Packet Packet::ping_packet(uint16_t source_ip, uint16_t dest_ip) {
    //Packet p;
    //p.ptype = PING;
    //p.source_ip = source_ip;
    //p.dest_ip = dest_ip;
    //return p;
//}

//Packet Packet::lsa_packet(uint8_t ttl, uint8_t payload) {
    //Packet p;
    //p.ptype = LSA;
    //p.ttl = ttl;
    //p.payload = payload;
    //return p;
//}

//Packet Packet::message_packet(uint16_t source_ip,
        //uint16_t imm_dest_ip, uint16_t dest_ip, uint8_t timestamp,
        //uint8_t ttl, uint8_t payload) {
    //return Packet(MESSAGE, source_ip, imm_dest_ip, dest_ip, timestamp, ttl, payload);
//}
