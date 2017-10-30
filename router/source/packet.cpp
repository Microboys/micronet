#include "packet.h"

void Packet::print_packet(MicroBitSerial serial) {
    serial.printf("===== PACKET START ====\n\r");
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
    serial.printf("===== PACKET END ====\n\r");
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
    this->ptype = (packet_type)p[F_PTYPE];
    this->rssi = rssi;
    switch (ptype) {
        case LSA:
            this->ttl = p[F_LSA_TTL];
            this->source_ip = concat(p[F_LSA_SOURCE_IP], p[F_LSA_SOURCE_IP + 1]);
            this->graph = decode_lsa(p, this->source_ip);
            break;
        case PING:
            this->source_ip = concat(p[F_PING_SOURCE_IP], p[F_PING_SOURCE_IP + 1]);
            this->imm_dest_ip = concat(p[F_PING_IMM_DEST_IP], p[F_PING_IMM_DEST_IP + 1]);
            break;
        case MESSAGE:
            this->source_ip = concat(p[F_MESSAGE_SOURCE_IP], p[F_MESSAGE_SOURCE_IP + 1]);
            this->imm_dest_ip = concat(p[F_MESSAGE_IMM_DEST_IP], p[F_MESSAGE_IMM_DEST_IP + 1]);
            this->dest_ip = concat(p[F_MESSAGE_DEST_IP], p[F_MESSAGE_DEST_IP + 1]);
            this->timestamp = p[F_MESSAGE_TIMESTAMP];
            this->ttl = p[F_MESSAGE_TTL];
            this->payload = decode_payload(p, F_MESSAGE_PAYLOAD);
            break;
        case DNS:
            //TODO
            break;
    }
}

PacketBuffer Packet::format() {
    PacketBuffer p = PacketBuffer(PACKET_SIZE);
    p[F_PTYPE] = this->ptype;

    switch (ptype) {
        case PING:
            p[F_PING_SOURCE_IP] = (uint8_t)(this->source_ip >> BYTE_SIZE);
            p[F_PING_SOURCE_IP + 1] = (uint8_t)(this->source_ip);
            p[F_PING_IMM_DEST_IP] = (uint8_t)(this->imm_dest_ip >> BYTE_SIZE);
            p[F_PING_IMM_DEST_IP + 1] = (uint8_t)(this->imm_dest_ip);
            break;
        case LSA:
            p[F_LSA_TTL] = this->ttl;
            p[F_LSA_SOURCE_IP] = (uint8_t)(this->source_ip >> BYTE_SIZE);
            p[F_LSA_SOURCE_IP + 1] = (uint8_t)(this->source_ip);
            encode_lsa(p, this->graph);
            break;
        case MESSAGE:
            p[F_MESSAGE_SOURCE_IP] = (uint8_t)(this->source_ip >> BYTE_SIZE);
            p[F_MESSAGE_SOURCE_IP + 1] = (uint8_t)(this->source_ip);
            p[F_MESSAGE_IMM_DEST_IP] = (uint8_t)(this->imm_dest_ip >> BYTE_SIZE);
            p[F_MESSAGE_IMM_DEST_IP + 1] = (uint8_t)(this->imm_dest_ip);
            p[F_MESSAGE_DEST_IP] = (uint8_t)(this->dest_ip >> BYTE_SIZE);
            p[F_MESSAGE_DEST_IP + 1] = (uint8_t)(this->dest_ip);
            p[F_MESSAGE_TIMESTAMP] = this->timestamp;
            p[F_MESSAGE_TTL] = this->ttl;
            encode_payload(p, payload, F_MESSAGE_PAYLOAD);
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
    for (size_t i = F_LSA_PAYLOAD; i <= PACKET_SIZE - LSA_EDGE_DATA_SIZE; i += LSA_EDGE_DATA_SIZE) {
        uint16_t to = concat(p[i], p[i + 1]);
        if (to == 0) {
            break;
        }
        int distance = (int8_t)(p[i + 2]);
        edge new_edge = {source_ip, to};
        pair<edge, int> p(new_edge, distance);
        graph.insert(p);
    }
    return graph;
}

void Packet::encode_lsa(PacketBuffer p, std::unordered_map<edge, int> graph) {
    int index = F_LSA_PAYLOAD;
    for (std::pair<edge, int> it : graph) {
        if (index <= PACKET_SIZE - LSA_EDGE_DATA_SIZE) {
            uint16_t to = it.first.to;
            uint8_t to_upper = (uint8_t)(to >> BYTE_SIZE);
            uint8_t to_lower = (uint8_t)(to);
            p[index++] = to_upper;
            p[index++] = to_lower;
            p[index++] = it.second;
        } else {
            break;
        }
    }
    for (int i = index; i < PACKET_SIZE; i++) {
        p[i] = 0;
    }
}

ManagedString Packet::to_json() {
    ManagedString result = "{";
    result = result + format_attr("type", "packet");
    switch (ptype) {
        case PING:
            result = result + format_attr("ptype", "PING");
            result = result + format_attr("source_ip", this->source_ip);
            result = result + format_attr("imm_dest_ip", this->imm_dest_ip, true);
            break;
        case LSA:
            result = result + format_attr("ptype", "LSA");
            result = result + format_attr("ttl", this->ttl);
            result = result + format_attr("source_ip", this->source_ip, true);
            break;
        case MESSAGE:
            result = result + format_attr("source_ip", this->source_ip);
            result = result + format_attr("imm_dest_ip", this->imm_dest_ip);
            result = result + format_attr("dest_ip", this->dest_ip);
            result = result + format_attr("ttl", this->ttl);
            result = result + format_attr("timestamp", this->timestamp);
            result = result + format_attr("payload", this->payload.toCharArray(), true);
            break;
        case DNS:
            result = result + format_attr("ptype", "DNS", true);
            //TODO
            break;
    }
    result = result + "}\0";
    return ManagedString(result);
}

ManagedString Packet::format_attr(ManagedString attr, ManagedString val, bool last) {
    ManagedString result = ManagedString("\"") 
        + ManagedString(attr) 
        + ManagedString("\":") 
        + ManagedString("\"") 
        + ManagedString(val) 
        + ManagedString("\"");
    if (last)
        return result;
    else
        return result + ",";
}

ManagedString Packet::format_attr(ManagedString attr, int val, bool last) {
    ManagedString result = ManagedString("\"") 
        + ManagedString(attr) 
        + ManagedString("\":") 
        + ManagedString(val);
    if (last)
        return result;
    else
        return result + ",";
}
