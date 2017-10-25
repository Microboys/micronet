#include "router.h"
#include <vector>
MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

uint16_t ip = 0;
uint16_t transmit_power = 7;

std::vector<router_info> neighbours;

// TODO: Decide mapped distance type.
std::unordered_map<struct edge, int> graph;

void broadcast(Packet p) {
    if (p.ttl > 0) {
        p.ttl--;
        for (auto n : neighbours) {
            p.imm_dest_ip = n.ip;
            uBit.radio.datagram.send(p.format());
        }
    }
}

void onData(MicroBitEvent e) {
    PacketBuffer buffer = uBit.radio.datagram.recv();
    Packet p = Packet(buffer, uBit.radio.getRSSI());
    p.print_packet(serial);
    print_graph(serial, graph);

    uBit.sleep(1);

    if (p.ptype == PING) {
        uBit.display.printAsync("!!");
        // Received new ping packet, send it back to source
        if (p.imm_dest_ip == 0) {
            p.imm_dest_ip = p.source_ip;
            p.source_ip = ip;
            p.ttl--;
            uBit.radio.datagram.send(p.format());
        } else if (p.imm_dest_ip == ip) {
            // Got back our own ping packet
            struct router_info neighbour;
            neighbour.ip = p.source_ip;
            neighbour.distance = p.rssi;
            neighbours.push_back(neighbour);
            update_graph(graph, ip, p.source_ip, p.rssi);
            // TODO: print new neighbours using graph
            print_neighbours();
        }
    } else if (p.ptype == MESSAGE) {
        if (p.dest_ip == ip) {
            serial.printf("%i sent me %s\n\r", p.source_ip, p.payload.toCharArray());
            uBit.display.printAsync(p.payload);
        } else if (p.imm_dest_ip == ip) {
            broadcast(p);
        }
    } else if (p.ptype == LSA) {
        uint8_t ttl = p.ttl;
        //payload = buffer[2];
        //update_graph(buffer);
        update_graph(graph, &p);

        if (ttl > 0) {
            send_new_graph(ttl-1);
        }
    }
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

void send_lsa(MicroBitEvent e) {
    std::unordered_map<edge, int> to_send;
    for (auto it : graph) {
        if (it.first.from == ip) {
            to_send[it.first] = it.second;
        }
    }
    Packet p(LSA, ip, 0, 0, 0, MAX_TTL, to_send);
    uBit.radio.datagram.send(p.format());
}

void setup() {
    // Generate a random IP, exclude 0
    ip = uBit.random(65534) + 1;
    //uBit.radio.setTransmitPower(transmit_power);
    serial.printf("======= BOOTING ======\n\r");
    serial.printf("Device IP: %i\n\r", ip);
    serial.printf("==== BOOTING DONE ====\n\r");
}

int main() {
    uBit.init();
    setup();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, ping);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, send_message);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, send_lsa);
    uBit.radio.enable();
    release_fiber();
}
