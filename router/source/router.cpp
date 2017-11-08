#include "router.h"
#include "lsr.h"
#include <vector>
MicroBit uBit;
MicroBitSerial

serial(USBTX, USBRX);

uint16_t ip = 0;
uint16_t transmit_power = 7;

void broadcast(Packet p) {
    if (p.ttl > 0) {
        p.ttl--;
        std::vector<uint16_t> neighbours = get_neighbours(ip);
        for (auto n : neighbours) {
            p.imm_dest_ip = ip;
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
            update_graph(ip, p.source_ip, p.rssi);
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
        //print_graph(serial);

        if (ttl > 0) {
            p.ttl = p.ttl - 1;
            uBit.radio.datagram.send(p.format());
        }
    }
}

void ping(MicroBitEvent e) {
    Packet p(PING, ip, 0, 0, 0, MAX_TTL, 0);
    uBit.radio.datagram.send(p.format());
    delete_all_edges(ip);
}

void send_message(MicroBitEvent e) {
    std::vector<uint16_t> neighbours = get_neighbours(ip);
    if (!neighbours.empty()) {
        uint16_t target = neighbours[uBit.random(neighbours.size())];

        //TODO
        ManagedString message = "Hello!";

        for (auto n : neighbours) {
            Packet p(MESSAGE, ip, n, target, 0, MAX_TTL, message);
            uBit.radio.datagram.send(p.format());
        }

        //serial.printf("Sending %s to %i...\n\r", message.toCharArray(), target);
    }
}

void send_message_via_routing(MicroBitEvent e) {
    std::vector<uint16_t> neighbours = get_neighbours(ip);
    if (!neighbours.empty()) {
        uint16_t target = neighbours[uBit.random(neighbours.size())];

        //TODO
        ManagedString message = "Hello!";
        uint16_t next_node = get_path_for_node(target);
        Packet p(MESSAGE, ip, next_node, target, 0, MAX_TTL, message);

        //serial.printf("Sending %s to %i...\n\r", message.toCharArray(), target);
    }
}

void send_payload(uint16_t dest_ip, ManagedString message) {
    std::vector<uint16_t> neighbours = get_neighbours(ip);
    if (!neighbours.empty()) {
        for (auto n : neighbours) {
            Packet p(MESSAGE, ip, n, dest_ip, 0, MAX_TTL, message);
            uBit.radio.datagram.send(p.format());
        }
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
