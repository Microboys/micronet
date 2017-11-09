#include "router.h"
#include "lsr.h"
#include <vector>
MicroBit uBit;
MicroBitSerial

serial(USBTX, USBRX);

/* Device IP */
uint16_t ip = 0;

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

    switch (p.ptype) {
        case PING:
            if (p.imm_dest_ip == 0) {
                p.imm_dest_ip = p.source_ip;
                p.source_ip = ip;
                uBit.radio.datagram.send(p.format());
            } else if (p.imm_dest_ip == ip) {
                // Got back our own ping packet
                update_graph(ip, p.source_ip, p.rssi);
            }
            break;
        case MESSAGE:
            if (p.dest_ip == ip) {
                uBit.display.printAsync(p.payload);
            } else if (p.imm_dest_ip == ip) {
                broadcast(p);
            }
            break;
        case LSA:
            update_graph(&p);

            if (p.ttl > 0) {
                p.ttl = p.ttl - 1;
                uBit.radio.datagram.send(p.format());
            }
            break;
        default:
            break;
    }
}

void ping(MicroBitEvent e) {
    Packet p(PING, ip, 0, 0, 0, MAX_TTL, 0);
    uBit.radio.datagram.send(p.format());
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
    serial.printf("%s", topology_json(ip).toCharArray());
}

void onMessage(MicroBitEvent e) {
    ManagedString request = serial.readUntil(DELIMITER);
    int header_length = ManagedString(MESSAGE_REQUEST).length();
    ManagedString substr = request.substring(0, header_length);

    if (substr == MESSAGE_REQUEST) {

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

    }

    serial.eventOn(DELIMITER);
};

void initSerialRead() {
    uBit.messageBus.listen(MICROBIT_ID_SERIAL, MICROBIT_SERIAL_EVT_DELIM_MATCH, onMessage);
    serial.eventOn(DELIMITER);
    serial.setRxBufferSize(RX_BUFFER_SIZE);
}

void update() {
    while(true) {
        uBit.display.scrollAsync(ManagedString(ip));
        ping(MicroBitEvent());
        uBit.sleep(UPDATE_RATE);

        send_lsa(MicroBitEvent());
        uBit.sleep(UPDATE_RATE);

        delete_extra_neighbours(ip);
        remove_dead_nodes(get_system_time());
        send_graph_update();

        uBit.sleep(UPDATE_RATE);
    }
}

void setup(MicroBitEvent e) {
    if (!started) {
        started = true;
        // Generate a random IP, exclude 0
        ip = uBit.random(IP_MAXIMUM - 1) + 1;

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
