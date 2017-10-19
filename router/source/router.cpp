#include "router.h"
#include <vector>
MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

uint16_t ip = 0;

std::vector<router_info> neighbours;

uint16_t concat(uint8_t upper, uint8_t lower) {
    return (upper << 8) | lower;
}

void print_packet(PacketBuffer p) {
    serial.printf(" ==== PACKET START ====\n\r");
    serial.printf("ptype: %i\n\r", p[0]);
    serial.printf("source_ip: %i\n\r", concat(p[1], p[2]));
    serial.printf("imm_dest_ip: %i\n\r", concat(p[3], p[4]));
    serial.printf("dest_ip: %i\n\r", concat(p[5], p[6]));
    serial.printf("timestamp: %i\n\r", p[7]);
    serial.printf("RSSI: %i\n\r", uBit.radio.getRSSI());
    serial.printf(" ==== PACKET END ====\n\r");
}

PacketBuffer format_packet(uint16_t source_ip, uint16_t imm_dest_ip,
        uint16_t dest_ip, uint8_t timestamp, packet_type ptype) {
    PacketBuffer p(PACKET_SIZE);
    //TODO: fragmentation data
    //TODO: payload
    p[0] = ptype;
    p[1] = (uint8_t)(source_ip >> 8);
    p[2] = (uint8_t)(source_ip);
    p[3] = (uint8_t)(imm_dest_ip >> 8);
    p[4] = (uint8_t)(imm_dest_ip);
    p[5] = (uint8_t)(dest_ip >> 8);
    p[6] = (uint8_t)(dest_ip);
    p[7] = timestamp;
    return p;
}

void onData(MicroBitEvent e) {
    PacketBuffer p = uBit.radio.datagram.recv();
    print_packet(p);

    packet_type ptype = (packet_type) p[0];
    uint16_t source_ip = concat(p[1], p[2]);
    uint16_t imm_dest_ip = concat(p[3], p[4]);
    uint16_t dest_ip = concat(p[5], p[6]);
    uint8_t timestamp = p[7];

    if (ptype == PING) {
        if (dest_ip == 0) {
            dest_ip = source_ip;
            source_ip = ip;
            PacketBuffer pnew = format_packet(source_ip, dest_ip, dest_ip, 0, PING);
            uBit.radio.datagram.send(pnew);
        } else if (imm_dest_ip == ip) {
            struct router_info neighbour;
            neighbour.ip = source_ip;
            neighbour.distance = uBit.radio.getRSSI();
            neighbours.push_back(neighbour);
            print_neighbours();
        }
    }

    uBit.display.printAsync("!!");
}

void print_neighbours() {
    serial.printf("======= NEIGHBOURS =====\n\r");
    for (router_info info : neighbours) {
        serial.printf("%i: %i\n\r", info.ip, info.distance);
    }
    serial.printf("==== END NEIGHBOURS ====\n\r");

}

void onClick(MicroBitEvent e) {
    PacketBuffer p = format_packet(ip, 0, 0, 0, PING);
    uBit.radio.datagram.send(p);
    serial.send("pinging...\n\r");
}

void setup() {
    // Generate a random IP, exclude 0
    ip = uBit.random(65534) + 1;
    serial.printf("======= BOOTING ======\n\r");
    serial.printf("Device IP: %i\n\r", ip);
    serial.printf("==== BOOTING DONE ====\n\r");
}

int main() {
    uBit.init();
    setup();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onClick);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onClick);
    uBit.radio.enable();
    release_fiber();
}
