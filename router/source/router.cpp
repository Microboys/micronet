#include "router.h"
#include "lsr.h"
#include <vector>
MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

unordered_map<uint16_t, ManagedString> name_table;

/* Device IP */
uint16_t ip = 0;
bool started = false;

/* Resends packet to all neighbours. */
void broadcast(Packet* p) {
    if (p->ttl > 0) {
        p->ttl--;
        std::vector<uint16_t> neighbours = get_neighbours(ip);
        for (auto n : neighbours) {
            p->imm_dest_ip = n;
            uBit.radio.datagram.send(p->format());
        }
    }
}

unsigned long get_system_time() {
    return uBit.systemTime();
}

/* Handler for receiving a packet. */
void on_packet(MicroBitEvent) {
    PacketBuffer buffer = uBit.radio.datagram.recv();
    Packet p = Packet(buffer, uBit.radio.getRSSI());
    uBit.sleep(1);
    update_alive_nodes(p.source_ip, get_system_time());

    handle_packet(&p);
}

void handle_packet(Packet* p) {
    switch (p->ptype) {
        case PING:
            handle_ping(p);
            break;
        case MESSAGE:
            handle_message(p);
            break;
        case LSA:
            handle_lsa(p);
            break;
        default:
            handle_dns(p);
            break;
    }
}

void handle_lsa(Packet* p) {
    update_graph(p);

    if (p->ttl > 0) {
        p->ttl = p->ttl - 1;
        uBit.radio.datagram.send(p->format());
    }
}

void handle_message(Packet* p) {
    if (p->dest_ip == ip) {
        uBit.display.printAsync(p->payload);
    } else if (p->imm_dest_ip == ip) {
        broadcast(p);
    }
}

void handle_ping(Packet* p) {
    if (p->imm_dest_ip == 0) {
        p->imm_dest_ip = p->source_ip;
        p->source_ip = ip;
        uBit.radio.datagram.send(p->format());
    } else if (p->imm_dest_ip == ip) {
        // Got back our own ping packet
        update_graph(ip, p->source_ip, p->rssi);
    }
}

void handle_dns(Packet* p) {
    name_table[p->source_ip] = p->payload;

    if (p->ttl > 0) {
        p->ttl = p->ttl - 1;
        uBit.radio.datagram.send(p->format());
    }
}

void ping(MicroBitEvent) {
    Packet p(PING, ip, 0, 0, 0, INITIAL_TTL, 0);
    uBit.radio.datagram.send(p.format());
}

void send_message_via_routing(MicroBitEvent) {
    std::vector<uint16_t> neighbours = get_neighbours(ip);
    if (!neighbours.empty()) {
        uint16_t target = neighbours[uBit.random(neighbours.size())];

        //TODO
        ManagedString message = "Hello!";
        uint16_t next_node = get_path_for_node(target);
        Packet p(MESSAGE, ip, next_node, target, 0, INITIAL_TTL, message);
    }
}

void send_payload(uint16_t dest_ip, ManagedString message) {
    std::vector<uint16_t> neighbours = get_neighbours(ip);
    if (!neighbours.empty()) {
        for (auto n : neighbours) {
            Packet p(MESSAGE, ip, n, dest_ip, 0, INITIAL_TTL, message);
            uBit.radio.datagram.send(p.format());
        }
    }
}

void send_lsa(MicroBitEvent) {
    std::unordered_map<edge, int> to_send = get_lsa_graph(ip);
    Packet p(LSA, ip, 0, 0, 0, INITIAL_TTL, to_send);
    uBit.radio.datagram.send(p.format());
}

void send_dns(ManagedString name) {
    Packet p(DNS, ip, 0, 0, 0, INITIAL_TTL, name);
    uBit.radio.datagram.send(p.format());
}

void send_graph_update() {
    serial.printf("%s", get_topology_json(ip).toCharArray());
}

void send_path_update() {
    serial.printf("%s", path_json(ip).toCharArray());
}
void on_serial(MicroBitEvent) {
    ManagedString request = serial.readUntil(SERIAL_DELIMITER);

    // Assume all headers are the same length.
    int header_length = ManagedString(MESSAGE_REQUEST).length();
    ManagedString substr = request.substring(0, header_length);

    if (started && substr == MESSAGE_REQUEST) {
        /* Find the delimiter character between IP and payload */
        int delim = -1;
        for (int i = header_length + 1; i < request.length(); i++) {
            if (request.charAt(i) == MESSAGE_DELIMITER) {
                delim = i;
                break;
            }
        }
        if (delim > -1) {
            ManagedString ip_string = request.substring(header_length + 1, delim - header_length - 1);
            ManagedString message = request.substring(delim + 1, request.length() - delim - 1);
            uint16_t ip = atoi(ip_string);
            send_payload(ip, message);
        }
    } else if (substr == DNS_REQUEST) {
        // Advertise own name
        ManagedString name = request.substring(header_length + 1, request.length() - header_length - 1);
        name_table[ip] = name;
        send_dns(name);
    }

    serial.eventOn(SERIAL_DELIMITER);
};

void init_serial_read() {
    serial.setRxBufferSize(RX_BUFFER_SIZE);
    uBit.messageBus.listen(MICROBIT_ID_SERIAL, MICROBIT_SERIAL_EVT_DELIM_MATCH, on_serial);
    serial.eventOn(SERIAL_DELIMITER);
}

void update_network() {
    while(started) {
        ping(MicroBitEvent());
        uBit.sleep(UPDATE_RATE);

        send_lsa(MicroBitEvent());
        uBit.sleep(UPDATE_RATE);

        delete_extra_neighbours(ip);
        remove_dead_nodes(get_system_time());
    }
}

void update_desktop_app() {
    while(started) {
        uBit.display.scrollAsync(ip);
        send_graph_update();
        send_path_update();
        uBit.sleep(UPDATE_RATE);
    }
}

void setup(MicroBitEvent) {
    if (!started) {
        started = true;
        // Generate a random IP, exclude 0
        ip = uBit.random(IP_MAXIMUM - 1) + 1;

        uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, on_packet, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
        uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, ping);
        uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB, MICROBIT_BUTTON_EVT_CLICK, send_lsa);
        uBit.radio.enable();

        send_graph_update();
        create_fiber(update_network);
        create_fiber(update_desktop_app);
    }
}

int main() {
    uBit.init();
    init_serial_read();
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, setup);
    MicroBitImage arrow("0,0,255,0,0\n0,255,0,0,0\n255,255,255,255,255\n0,255,0,0,0\n0,0,255,0,0\n");
    uBit.display.print(arrow);
    release_fiber();
}
