#include "router.h"
#include <vector>
MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

uint16_t ip = 0;
uint16_t transmit_power = 7;

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
    PacketBuffer p = uBit.radio.datagram.recv();
    print_packet(p);

    packet_type ptype = (packet_type) p[0];
    uint16_t source_ip = concat(p[1], p[2]);
    uint16_t imm_dest_ip = concat(p[3], p[4]);
    uint16_t dest_ip = concat(p[5], p[6]);
    uint8_t timestamp = p[7];
    uint8_t ttl = p[8];
    uint8_t payload = p[9];

    if (ptype == PING) {
        if (dest_ip == 0) {
            dest_ip = source_ip;
            source_ip = ip;
            PacketBuffer pnew = format_packet(source_ip, dest_ip, dest_ip, 0, ptype, ttl-1, payload);
            uBit.radio.datagram.send(pnew);
        } else if (imm_dest_ip == ip) {
            pair<edge, int> p((edge{source_ip,ip}),uBit.radio.getRSSI());
            graph.insert(p);
            // TODO: print new neighbours using graph
        }
    } else if (ptype == MESSAGE) {
        if (dest_ip == ip) {
            serial.printf("%i sent me %i!\n\r", source_ip, payload);
            uBit.display.printAsync(payload);
        } else if (ttl > 0 && imm_dest_ip == ip) {
            for (auto n : neighbours) {
                PacketBuffer pnew = format_packet(source_ip, n.ip, dest_ip, 0, ptype, ttl-1, payload);
                uBit.radio.datagram.send(pnew);
            }
        }
    } else if (ptype == LSA) {
      uint8_t ttl = p[1];
      payload = p[2];
      update_graph(p);

      if (ttl > 0) {
        send_new_graph(ttl-1);
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

void update_graph(PacketBuffer& p) {
  for (size_t i = 2; i < 31; i+=5) {
    uint16_t from = concat(p[i], p[i+1]);
    uint16_t to = concat(p[i+2], p[i+3]);
    int distance = p[i+4];
    struct edge ed = {from, to};
    graph[ed] = distance;
  }
  // TODO: print all values to check with graph
}

void send_new_graph(int ttl) {
    //TODO: propogate the packet with new ttl
    //NOTE: here ttl decide the range of the graph
    
}

void ping(MicroBitEvent e) {
    PacketBuffer p = format_packet(ip, 0, 0, 0, PING, MAX_TTL, 0);
    uBit.radio.datagram.send(p);
    serial.send("pinging...\n\r");
}

void send_message(MicroBitEvent e) {
    if (!neighbours.empty()) {
        uint16_t target = neighbours[uBit.random(neighbours.size())].ip;

        //TODO
        uint8_t message = 99;

        for (auto n : neighbours) {
            PacketBuffer p = format_packet(ip, n.ip, target, 0, MESSAGE, MAX_TTL, message);
            uBit.radio.datagram.send(p);
        }

        serial.printf("Sending %i to %i...\n\r", message, target);
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
