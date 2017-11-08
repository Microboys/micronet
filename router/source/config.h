#pragma once
// Global configuration variables.

/* Maximum distance for establishing connection. Expressed as an RSSI value,
 * lower value means farther away (e.g. -40 is closer than -60).
 * Threshold is inclusive.
 */
#define CONNECTION_THRESHOLD -70

/* Maximum distance for disconnection. Expressed as an RSSI value,
 * lower value means farther away (e.g. -40 is closer than -60).
 * This value should be farther away than CONNECTION_THRESHOLD to prevent
 * constant connections/disconnections when two devices are on the boundary. 
 * Threshold is inclusive.
 */
#define DISCONNECTION_THRESHOLD -90

/* Maximum number of neighbours. This limit exists because we want to be able
 * to send all neighbour information in the payload of one LSA packet 
 * (which is 28B maximum). Since IPs are 2B each and we need 1B for the 
 * distance, we can only fit floor(28/3) = 9 neighbours.
 */
#define MAX_NEIGHBOURS 9
