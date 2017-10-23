#include "router.h"
#include <vector>
MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

uint16_t ip = 0;
uint16_t transmit_power = 7;

std::vector<router_info> neighbours;

PacketBuffer format_packet(uint16_t source_ip, uint16_t imm_dest_ip,
        uint16_t dest_ip, uint8_t timestamp, packet_type ptype, uint8_t ttl,
        uint8_t payload) {
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
    p[8] = ttl;
    p[9] = payload;
    return p;
}

void onData(MicroBitEvent e) {
    PacketBuffer buffer = uBit.radio.datagram.recv();
    Packet p = Packet(buffer, uBit.radio.getRSSI());
    p.print_packet(serial);

    if (p.ptype == PING) {
        if (p.imm_dest_ip == 0) {
            p.imm_dest_ip = p.source_ip;
            p.source_ip = ip;
            p.ttl--;
            uBit.radio.datagram.send(p.format());
        } else if (p.imm_dest_ip == ip) {
            struct router_info neighbour;
            neighbour.ip = p.source_ip;
            neighbour.distance = p.rssi;
            neighbours.push_back(neighbour);
            print_neighbours();
        }
    } else if (p.ptype == MESSAGE) {
        if (p.dest_ip == ip) {
            serial.printf("%i sent me %s\n\r", p.source_ip, p.payload.toCharArray());
            uBit.display.printAsync(p.payload);
        } else if (p.ttl > 0 && p.imm_dest_ip == ip) {
            p.ttl--;
            for (auto n : neighbours) {
                p.imm_dest_ip = n.ip;
                uBit.radio.datagram.send(p.format());
            }
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

void ping(MicroBitEvent e) {
    Packet p(PING, ip, 0, 0, 0, MAX_TTL, 0);
    uBit.radio.datagram.send(p.format());
    serial.printf("Pinging...\n\r");
}

void send_message(MicroBitEvent e) {
    if (!neighbours.empty()) {
        uint16_t target = neighbours[uBit.random(neighbours.size())].ip;

        //TODO
        ManagedString message = "Hello!";

        for (auto n : neighbours) {
            Packet p(MESSAGE, ip, n.ip, target, 0, MAX_TTL, message);
            uBit.radio.datagram.send(p.format());
        }

        serial.printf("Sending %s to %i...\n\r", message.toCharArray(), target);
    }
}

void setup() {
    // Generate a random IP, exclude 0
    ip = uBit.random(65534) + 1;
    uBit.radio.setTransmitPower(transmit_power);
    serial.printf("======= BOOTING ======\n\r");
    serial.printf("Device IP: %i\n\r", ip);
    serial.printf("==== BOOTING DONE ====\n\r");
}

int main() {
    uBit.init();
    setup();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, ping);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, send_message);
    uBit.radio.enable();
    release_fiber();
}
