#pragma once
// Global configuration variables.

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

#define UPDATE_RATE 500

/* Serial read buffer size. */
#define RX_BUFFER_SIZE 200

/* Time in ms before deciding a router is dead. If we don't receive a packet
 * from a router before the time is up, we delete it and every corresponding
 * arc from our graph. */
#define NODE_LIFESPAN 4000

#define SERIAL_DELIMITER "\n"
