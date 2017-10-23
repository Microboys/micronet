#include "packet.h"

void Packet::print_packet(MicroBitSerial serial) {
    serial.printf(" ==== PACKET START ====\n\r");
    switch (ptype) {
        case PING:
            serial.printf("source_ip: %i\n\r", this->source_ip);
            serial.printf("imm_dest_ip: %i\n\r", this->imm_dest_ip);
            break;
        case LSA:
            serial.printf("ttl: %i\n\r", this->ttl);
            //TODO payload p[2-31]
            break;
        case MESSAGE:
            serial.printf("ptype: %i\n\r", this->ptype);
            serial.printf("source_ip: %i\n\r", this->source_ip);
            serial.printf("imm_dest_ip: %i\n\r", this->imm_dest_ip);
            serial.printf("dest_ip: %i\n\r", this->dest_ip);
            serial.printf("ttl: %i\n\r", this->ttl);
            serial.printf("timestamp: %i\n\r", this->timestamp);
            break;
        case DNS:
            //TODO
            break;
    }
    serial.printf(" ==== PACKET END ====\n\r");
}

// Outgoing packets
Packet::Packet(packet_type ptype, uint16_t source_ip, uint16_t imm_dest_ip,
        uint16_t dest_ip, uint8_t timestamp, uint8_t ttl,
        uint8_t payload) {

        this->source_ip = source_ip;
        this->imm_dest_ip = imm_dest_ip;
        this->dest_ip = dest_ip;
        this->timestamp = timestamp;
        this->ptype = ptype;
        this->ttl = ttl;
        this->payload = payload;
}

// Incoming packets
Packet::Packet(PacketBuffer p, int rssi) {
    this->ptype = (packet_type)p[0];
    this->rssi = rssi;
    switch (ptype) {
        case PING:
            this->source_ip = concat(p[1], p[2]);
            this->imm_dest_ip = concat(p[3], p[4]);
            break;
        case LSA:
            this->ttl = p[1];
            //TODO payload p[2-31]
            break;
        case MESSAGE:
            this->source_ip = concat(p[1], p[2]);
            this->imm_dest_ip = concat(p[3], p[4]);
            this->dest_ip = concat(p[5], p[6]);
            this->timestamp = p[7];
            this->ttl = p[8];
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
            //TODO payload p[2-31]
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
            p[9] = this->payload;
            break;
        case DNS:
            //TODO
            break;
    }
    return p;
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
