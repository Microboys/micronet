#include "MicroBit.h"

MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);
int ip = 0;

void onData(MicroBitEvent e) {
    PacketBuffer p = uBit.radio.datagram.recv();

    for (int i = 0; i < 4; i++) {
        serial.send(p[i] + " | ");
    }
    serial.send(uBit.radio.getRSSI() + " | ");
    uBit.display.printAsync("AA");
}

int main() {
    uBit.init();
    ip = uBit.random(254) + 1;
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    while (true) {
        if (uBit.buttonA.isPressed() || uBit.buttonB.isPressed()) {
            //ManagedString packet = format_packet(ip, 0, "", true);
            PacketBuffer p(200);
            p[0] = ip;
            p[1] = 0;
            p[2] = 0;
            p[3] = true;
            uBit.radio.datagram.send(p);
            serial.send("pinging... | ");
        }
        uBit.sleep(100);
    }
    release_fiber();
}
