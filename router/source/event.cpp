#include "event.h"

// TODO: formatting events like they are packets for convenience, should change to
// new format for events.

ManagedString neighbour_discovered_event(uint16_t ip, uint8_t rssi) {
    ManagedString result = "{";
    result = result + format_attr("type", "packet");
    result = result + format_attr("ptype", "neighbour_discovered");
    result = result + format_attr("ip", ip);
    result = result + format_attr("rssi", rssi);
    result = result + "}" + SERIAL_DELIMITER;
    return result;
}

ManagedString router_timed_out_event(uint16_t ip) {
    ManagedString result = "{";
    result = result + format_attr("type", "packet");
    result = result + format_attr("ptype", "router_timed_out");
    result = result + format_attr("ip", ip);
    result = result + "}" + SERIAL_DELIMITER;
    return result;
}
