#include "router.h"

MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

unordered_map<uint16_t, ManagedString> name_table;
unordered_map<uint16_t, PacketBuffer> lsa_table;
uint16_t sequence_number;

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
      // recalculate_graph(ip);
      uint16_t next_node = get_next_node(p->dest_ip);
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
    auto rssi = uBit.radio.getRSSI();
    if (buffer == PacketBuffer::EmptyPacket || rssi == 0
            || buffer.length() != PACKET_SIZE
            || rssi < DISCONNECTION_THRESHOLD) {
        return;
    }

    if (packet_queue.size() > MAX_PACKET_QUEUE_SIZE && buffer[F_PTYPE] != MESSAGE) {
        return;
    }

    packet_queue.push_back(new Packet(buffer, rssi));
}

void process_packets() {
    while (true) {
        if (!packet_queue.empty()) {
            Packet* p = packet_queue.front();

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
            handle_dns(p);
            break;
    }
}

void handle_lsa(Packet* p) {
    if (p->source_ip == ip) {
      /* If we see a packet sent from us with a higher sequence number, we should
       * update our sequence number so our LSAs aren't ignored. */
      if (p->sequence_number > sequence_number) {
        sequence_number = p->sequence_number + 1;
      }
      return;
    }

    bool should_flood = false;
    uint8_t new_ttl = p->ttl - 1;
    p->ttl = 0;

    if (lsa_table.count(p->source_ip) == 0) {
      lsa_table[p->source_ip] = p->format();
      should_flood = true;
      update_graph(p);

      /* This packet, has introduced a new node so we notify the desktop app. */
      serial.send(p->to_json());

    } else {
      PacketBuffer old_packet_buffer = lsa_table[p->source_ip];
      uint16_t old_sequence_number = Packet::get_sequence_number(old_packet_buffer);

      if (old_sequence_number < p->sequence_number) {
        should_flood = true;

        // TODO: We're deserialising the packet just to serialise it again - better way?
        PacketBuffer new_packet_buffer = p->format();

        Packet::set_sequence_number(new_packet_buffer, 0);
        Packet::set_sequence_number(old_packet_buffer, 0);

        /* PacketBuffer does not support '!='. */
        if (!(new_packet_buffer == old_packet_buffer)) {

          if (update_graph(p)) {
            /* Update desktop app with new packet only if the topology changes. */
            // TODO: Make LSAs an event, like the neighbour updates.
            serial.send(p->to_json());
          }

        } 

        Packet::set_sequence_number(new_packet_buffer, sequence_number);
        lsa_table[p->source_ip] = new_packet_buffer;
      } else if (old_sequence_number > p->sequence_number) {
        /* If the router is a neighbour, we should flood its previous packet so it can
         * learn its latest sequence number. */
        std::vector<uint16_t> neighbours = get_neighbours(ip);
        if (std::find(neighbours.begin(), neighbours.end(), p->source_ip) != neighbours.end()) {
          uBit.radio.datagram.send(old_packet_buffer);
          uBit.sleep(1);
          return;
        }
      }
    }

    if (new_ttl > 0 && should_flood) {
        p->ttl = new_ttl;
        uBit.radio.datagram.send(p->format());
        uBit.sleep(1);
    }
}

void handle_message(Packet* p) {
    if (p->imm_dest_ip == ip) {
        uBit.display.printAsync(p->payload);
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
        if (update_graph(ip, p->source_ip, p->rssi)) {
            serial.send(neighbour_discovered_event(p->source_ip, p->rssi));
        }
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
    // recalculate_graph(ip);
    uint16_t next_node = get_next_node(dest_ip);
    Packet p(MESSAGE, ip, next_node, dest_ip, 0, INITIAL_TTL, message);
    uBit.radio.datagram.send(p.format());
    uBit.sleep(1);
}

void send_lsa(MicroBitEvent) {
    std::unordered_map<edge, int> to_send = get_lsa_graph(ip);
    Packet p(LSA, ip, 0, 0, 0, INITIAL_TTL, sequence_number, to_send);
    sequence_number++;
    uBit.radio.datagram.send(p.format());
    uBit.sleep(1);
}

void send_dns(ManagedString name) {
    Packet p(DNS, ip, 0, 0, 0, INITIAL_TTL, name);
    uBit.radio.datagram.send(p.format());
}

ManagedString name_table_json() {
    size_t index = 0;
    ManagedString result = "{";
    result = result + format_attr("type", "dns");
    result = result + "\"dns\":[";
    for (auto it : name_table) {
        result = result + "{";
        result = result + format_attr("ip", it.first);
        result = result + format_attr("name", it.second, true);
        result = result + "}";
        index++;
        if (index < name_table.size()) {
            result = result + ",";
        }
    }
    result = result + "]";
    return result + "}" + SERIAL_DELIMITER;
}

void send_graph_update() {
    serial.send(get_topology_json(ip));
}

void send_path_update() {
    serial.send(path_json(ip));
}

void send_name_table() {
    serial.send(name_table_json());
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
        // Refresh visual ID on display in case we were displaying something else
        uBit.display.print(get_visual_id(ip));

        ping(MicroBitEvent());
        uBit.sleep(UPDATE_RATE);

        send_lsa(MicroBitEvent());
        uBit.sleep(UPDATE_RATE);

        std::unordered_set<uint16_t> dead_nodes = remove_dead_nodes(get_system_time());
        for (uint16_t dead_ip: dead_nodes) {
          /* One of the nodes that can be removed is ourself, for some reason.
           * Let's not tell the desktop app that this is the case. */
          if (dead_ip != ip) {
            serial.send(router_timed_out_event(dead_ip));
          }
        }
    }
}

void update_desktop_app() {
    while(started) {
        //uBit.display.scrollAsync(ip);
        // recalculate_graph(ip);

        send_graph_update();
        // send_path_update();
        send_name_table();
        uBit.sleep(UPDATE_RATE);
    }
}

MicroBitImage get_visual_id(uint16_t ip) {
    ManagedString high(255);
    ManagedString low(0);
    ManagedString result = "";
    ManagedString delim = "\n";
    for (int i = 0; i < 25; i++) {
        if (i % 5 == 0) {
            delim = "\n";
        } else {
            delim = ",";
        }

        if (ip % 2 == 1) {
            result = high + delim + result;
            ip--;
        } else {
            result = low + delim + result;
        }
        ip /= 2;
    }

    return MicroBitImage(result.toCharArray());
}

void setup(MicroBitEvent) {
    if (!started) {
        started = true;
        // Generate a random IP, exclude 0
        ip = uBit.random(IP_MAXIMUM - 1) + 1;
        uBit.display.print(get_visual_id(ip));

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
