#include "router.h"
#include "lsr.h"
#include <vector>
MicroBit uBit;
MicroBitSerial

serial(USBTX, USBRX);

uint16_t ip = 0;
uint16_t transmit_power = 7;
bool started = false;

void broadcast(Packet p) {
    if (p.ttl > 0) {
        p.ttl--;
        std::vector<uint16_t> neighbours = get_neighbours(ip);
        for (auto n : neighbours) {
            p.imm_dest_ip = n;
            uBit.radio.datagram.send(p.format());
        }
    }
}

unsigned long get_system_time() {
    return uBit.systemTime();
}

void onData(MicroBitEvent e) {
    PacketBuffer buffer = uBit.radio.datagram.recv();
    Packet p = Packet(buffer, uBit.radio.getRSSI());
    uBit.sleep(1);
    update_alive_nodes(p.source_ip, get_system_time());
    if (p.ptype == PING) {
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
            uBit.display.printAsync(p.payload);
        } else if (p.imm_dest_ip == ip) {
            broadcast(p);
        }
    } else if (p.ptype == LSA) {
        update_graph(&p);

        if (p.ttl > 0) {
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

void send_message_via_routing(MicroBitEvent e) {
    std::vector<uint16_t> neighbours = get_neighbours(ip);
    if (!neighbours.empty()) {
        uint16_t target = neighbours[uBit.random(neighbours.size())];

        //TODO
        ManagedString message = "Hello!";
        uint16_t next_node = get_path_for_node(target);
        Packet p(MESSAGE, ip, next_node, target, 0, MAX_TTL, message);
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

void send_graph_update() {
    serial.printf("%s", topology_json(ip, uBit.systemTime()).toCharArray());
}

void onMessage(MicroBitEvent e) {
    ManagedString request = serial.readUntil(DELIMITER);
    ManagedString substr = request.substring(0, MESSAGE_REQUEST_LENGTH);

    if (substr == MESSAGE_REQUEST) {
        int delim = -1;
        for (int i = MESSAGE_REQUEST_LENGTH + 1; i < request.length(); i++) {
            if (request.charAt(i) == MESSAGE_DELIMITER) {
                delim = i;
                break;
            }
        }
        if (delim > -1) {
            ManagedString ip_string = request.substring(MESSAGE_REQUEST_LENGTH + 1, delim - MESSAGE_REQUEST_LENGTH - 1);
            ManagedString message = request.substring(delim + 1, request.length() - delim - 1);
            uint16_t ip = atoi(ip_string);
            send_payload(ip, message);
        }

    } else {
        // TODO: Parse message and trigger actions
        if (request == GRAPH_REQUEST) {
            //send_graph_update();
        } else if (request == IP_REQUEST) {
            serial.printf("%i\n", ip);
        } else if (request == PING_REQUEST) {
            ping(MicroBitEvent());
        }
    }

    serial.eventOn(DELIMITER);
};

void initSerialRead() {
    uBit.messageBus.listen(MICROBIT_ID_SERIAL, MICROBIT_SERIAL_EVT_DELIM_MATCH, onMessage);
    serial.eventOn(DELIMITER);
    serial.setRxBufferSize(200);
}

void update() {
    while(1) {
        uBit.display.scrollAsync(ManagedString(ip));
        uBit.sleep(500);
        ping(MicroBitEvent());
        uBit.sleep(1000);
        send_lsa(MicroBitEvent());
        uBit.sleep(1000);
        delete_extra_neighbours(ip);
        send_graph_update();
        uBit.sleep(1000);
    }
}

void setup(MicroBitEvent e) {
    if (!started) {
        started = true;
        // Generate a random IP, exclude 0
        ip = uBit.random(65534) + 1;

        uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
        uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, ping);
        initSerialRead();
        uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, send_lsa);
        uBit.radio.enable();

        send_graph_update();
        create_fiber(update);
    }
}


int main() {
    uBit.init();
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, setup);
    uBit.display.print("<");
    release_fiber();
}
