#include "router.h"
#include "lsr.h"
#include <vector>
MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

/* Device IP */
uint16_t ip = 0;
bool started = false;
std::vector<Packet*> packet_queue;

/* Resends packet to all neighbours. */
void broadcast(Packet* p) {
    if (p->ttl > 0) {
        p->ttl--;
        std::vector<uint16_t> neighbours = get_neighbours(ip);
        for (auto n : neighbours) {
            p->imm_dest_ip = n;
            uBit.radio.datagram.send(p->format());
            uBit.sleep(1);
        }
    }
}

void send_message(Packet* p) {
  if (p->ttl > 0) {
      p->ttl--;
      uint16_t next_node = get_path_for_node(p->dest_ip);
      p->imm_dest_ip = next_node;
      uBit.radio.datagram.send(p->format());
      uBit.sleep(1);
  }
}

unsigned long get_system_time() {
    return uBit.systemTime();
}

/* Handler for receiving a packet. */
void on_packet(MicroBitEvent) {
    PacketBuffer buffer = uBit.radio.datagram.recv();
    if (buffer == PacketBuffer::EmptyPacket || uBit.radio.getRSSI() == 0
            || buffer.length() != PACKET_SIZE) {
        return;
    }

    if (packet_queue.size() > 10) {
        return;
    }

    packet_queue.push_back(new Packet(buffer, uBit.radio.getRSSI()));
}

void process_packets() {
    while (true) {
        if (!packet_queue.empty()) {
            Packet* p = packet_queue.front();

            // Update desktop app
            serial.send(p->to_json());

            update_alive_nodes(p->source_ip, get_system_time());
            handle_packet(p);
            packet_queue.erase(packet_queue.begin());
            delete p;
        }
        uBit.sleep(PACKET_PROCESS_RATE);
    }
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
            break;
    }
}

void handle_lsa(Packet* p) {
    update_graph(p);
    //recalculate_graph(ip);

    if (p->ttl > 0) {
        p->ttl--;
        uBit.radio.datagram.send(p->format());
        uBit.sleep(1);
    }
}

void handle_message(Packet* p) {
    if (p->dest_ip == ip) {
        uBit.display.printAsync(p->payload);
    } else if (p->imm_dest_ip == ip) {
        send_message(p);
    }
}

void handle_ping(Packet* p) {
    if (p->imm_dest_ip == 0) {
        p->imm_dest_ip = p->source_ip;
        p->source_ip = ip;
        uBit.radio.datagram.send(p->format());
        uBit.sleep(1);
    } else if (p->imm_dest_ip == ip) {
        // Got back our own ping packet
        update_graph(ip, p->source_ip, p->rssi);
        //recalculate_graph(ip);
    }
}

void ping(MicroBitEvent) {
    Packet p(PING, ip, 0, 0, 0, INITIAL_TTL, 0);
    uBit.radio.datagram.send(p.format());
    uBit.sleep(1);
}

// void send_payload(uint16_t dest_ip, ManagedString message) {
//     std::vector<uint16_t> neighbours = get_neighbours(ip);
//     if (!neighbours.empty()) {
//         for (auto n : neighbours) {
//             Packet p(MESSAGE, ip, n, dest_ip, 0, INITIAL_TTL, message);
//             uBit.radio.datagram.send(p.format());
//         }
//     }
// }

void send_payload(uint16_t dest_ip, ManagedString message) {
    uint16_t next_node = get_path_for_node(dest_ip);
    Packet p(MESSAGE, ip, next_node, dest_ip, 0, INITIAL_TTL, message);
    uBit.radio.datagram.send(p.format());
    uBit.sleep(1);
}

void send_lsa(MicroBitEvent) {
    std::unordered_map<edge, int> to_send = get_lsa_graph(ip);
    Packet p(LSA, ip, 0, 0, 0, INITIAL_TTL, to_send);
    uBit.radio.datagram.send(p.format());
    uBit.sleep(1);
}

void send_graph_update() {
    serial.send(get_topology_json(ip));
}

void send_path_update() {
    serial.send(path_json(ip));
}

void on_serial(MicroBitEvent) {
    ManagedString request = serial.readUntil(SERIAL_DELIMITER);
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
        //recalculate_graph(ip);
    }
}

void update_desktop_app() {
    while(started) {
        uBit.display.scrollAsync(ip);
        send_graph_update();
        send_path_update();
        //send_name_table();
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
        create_fiber(process_packets);
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
