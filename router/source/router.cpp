#include "router.h"
#include <vector>
MicroBit uBit;
MicroBitSerial

serial(USBTX, USBRX);

uint16_t ip = 0;
uint16_t transmit_power = 7;

std::vector<router_info> neighbours;

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
    //p.print_packet(serial);
    //serial.printf("%s", p.to_json().toCharArray());
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
            struct router_info neighbour; neighbour.ip = p.source_ip;
            neighbour.distance = p.rssi;
            neighbours.push_back(neighbour);
            update_graph(ip, p.source_ip, p.rssi);
            // TODO: print new neighbours using graph
            //print_neighbours();
            //print_graph(serial);
        }
    } else if (p.ptype == MESSAGE) {
        if (p.dest_ip == ip) {
            //serial.printf("%i sent me %s\n\r", p.source_ip, p.payload.toCharArray());
            uBit.display.printAsync(p.payload);
        } else if (p.imm_dest_ip == ip) {
            broadcast(p);
        }
    } else if (p.ptype == LSA) {
        uint8_t ttl = p.ttl;
        update_graph(&p);
        print_graph(serial);

        if (ttl > 0) {
            p.ttl = p.ttl - 1;
            uBit.radio.datagram.send(p.format());
        }
    }
}

void print_neighbours() {
    //serial.printf("======= NEIGHBOURS =====\n\r");
    for (router_info info : neighbours) {
        //serial.printf("%i: %i\n\r", info.ip, info.distance);
    }
    //serial.printf("==== END NEIGHBOURS ====\n\r");

}

void ping(MicroBitEvent e) {
    Packet p(PING, ip, 0, 0, 0, MAX_TTL, 0);
    uBit.radio.datagram.send(p.format());
    delete_all_edges(ip);
    neighbours.clear();
    //serial.printf("Pinging...\n\r");
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

        //serial.printf("Sending %s to %i...\n\r", message.toCharArray(), target);
    }
}

void send_lsa(MicroBitEvent e) {
    std::unordered_map<edge, int> to_send = get_lsa_graph(ip);
    Packet p(LSA, ip, 0, 0, 0, MAX_TTL, to_send);
    uBit.radio.datagram.send(p.format());
}

void echo_message(MicroBitEvent e) {
    ManagedString in = serial.readUntil("\n\0");
    //serial.printf("UBit: received %s", in.toCharArray());
    uBit.display.printAsync(in);
}

void send_graph_update() {
    serial.printf("%s", topology_json().toCharArray());
}

void setup() {
    // Generate a random IP, exclude 0
    ip = uBit.random(65534) + 1;
    //uBit.radio.setTransmitPower(transmit_power);
    //serial.printf("======= BOOTING ======\n\r");
    //serial.printf("Device IP: %i\n\r", ip);
    //serial.printf("==== BOOTING DONE ====\n\r");
}

void onMessage(MicroBitEvent e) {
    ManagedString request = serial.readUntil(DELIMITER);
    // TODO: Parse message and trigger actions
    if (request == GRAPH_REQUEST) {
        send_graph_update();
    } else if (request == IP_REQUEST) {
        serial.printf("%i\n", ip);
    } else if (request == PING_REQUEST) {
        ping(MicroBitEvent());
    }

    serial.eventOn(DELIMITER);
};

void initSerialRead() {
    uBit.messageBus.listen(MICROBIT_ID_SERIAL, MICROBIT_SERIAL_EVT_DELIM_MATCH, onMessage);
    serial.eventOn(DELIMITER);
    serial.setRxBufferSize(200);
}

int main() {
    uBit.init();
    setup();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, ping);
    initSerialRead();
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, send_lsa);
    uBit.radio.enable();
    release_fiber();
}
