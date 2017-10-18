#include "MicroBit.h"

#define MY_APP_ID 4000

MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

void onData(MicroBitEvent e) {
    ManagedString s = uBit.radio.datagram.recv();

    uBit.display.print(uBit.radio.getRSSI());
}

int main() {
    uBit.init();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    while (true) {
        if (uBit.buttonA.isPressed() || uBit.buttonB.isPressed()) {
            uBit.radio.datagram.send("0");
            uBit.display.print("11");
        }
        serial.send("HELLO FROM MICROBIT");
        uBit.sleep(100);
    }
    release_fiber();
}
