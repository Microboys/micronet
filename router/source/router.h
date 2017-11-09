#pragma once
#include "MicroBit.h"
#include "packet.h"
#include "graph.h"
#include "config.h"
#define DELIMITER "\r\n"

/* Interface for desktop app to tell router to send a message.
 *
 * Example:
 * MSG\t12345\tHello world!\n
 */
#define MESSAGE_REQUEST "MSG"
#define MESSAGE_DELIMITER '\t'

void print_neighbours();
void onData(MicroBitEvent e);
void ping(MicroBitEvent e);
void send_payload(uint16_t target, ManagedString message);
void send_lsa(MicroBitEvent e);
void broadcast(Packet p);
unsigned long get_system_time();
void setup(MicroBitEvent e);
void update();
int main();
