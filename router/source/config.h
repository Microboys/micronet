#pragma once
// Global configuration variables.

/* Initial time-to-live value for broadcasted packets. Corresponds to number
 * of hops before the packet is discarded. 
 */
#define INITIAL_TTL 5

/* Size of a single packet. Note that 32B is the maximum size of a single
 * datagram the microbit can send via radio.
 */
#define PACKET_SIZE 32

/* Maximum distance for establishing connection. Expressed as an RSSI value,
 * lower value means farther away (e.g. -40 is closer than -60).
 * Threshold is inclusive.
 */
#define CONNECTION_THRESHOLD -57

/* Maximum distance for disconnection. Expressed as an RSSI value,
 * lower value means farther away (e.g. -40 is closer than -60).
 * This value should be farther away than CONNECTION_THRESHOLD to prevent
 * constant connections/disconnections when two devices are on the boundary. 
 * Threshold is inclusive.
 */
#define DISCONNECTION_THRESHOLD -67

/* Maximum number of neighbours. This limit exists because we want to be able
 * to send all neighbour information in the payload of one LSA packet 
 * (which is 28B maximum). Since IPs are 2B each and we need 1B for the 
 * distance, we can only fit floor(28/3) = 9 neighbours.
 */
#define MAX_NEIGHBOURS 9

/* 2^16 - 1 */
#define IP_MAXIMUM 65535

/* Delay in ms before each action in update loop. Note that there are multiple
 * of these delays in a single loop (e.g. before sending ping and lsa) so one
 * update cycle will take longer than this value.
 */
#define UPDATE_RATE 500

/* Delay in ms before processing each packet. */
#define PACKET_PROCESS_RATE 1

/* Serial read buffer size. */
#define RX_BUFFER_SIZE 200

/* Time in ms before deciding a router is dead. If we don't receive a packet
 * from a router before the time is up, we delete it and every corresponding
 * arc from our graph. */
#define NODE_LIFESPAN 4000

/* Delimiter for serial communication. */
#define SERIAL_DELIMITER "\n"

/* Interface for desktop app to tell router to send a message.
 *
 * Example:
 * MSG\t12345\tHello world!\n
 */
#define MESSAGE_REQUEST "MSG"
#define MESSAGE_DELIMITER '\t'

/* Interface for sending a simple 'hello' back over serial to check the
 * microbit is still alive.
 */
#define HELLO_REQUEST "HELLO"
