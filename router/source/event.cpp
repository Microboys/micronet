#include "event.h"

// TODO: formatting events like they are packets for convenience, should change to
// new format for events.

ManagedString neighbour_discovered_event(uint16_t ip, uint8_t rssi) {
    ManagedString result = "{";
    result = result + format_attr("type", "packet");
    result = result + format_attr("ptype", "NEIGHBOUR_DISCOVERED");
    result = result + format_attr("source_ip", ip);
    result = result + format_attr("rssi", rssi, true);
    result = result + "}" + SERIAL_DELIMITER;
    return result;
}

ManagedString router_timed_out_event(uint16_t ip) {
    ManagedString result = "{";
    result = result + format_attr("type", "packet");
    result = result + format_attr("ptype", "ROUTER_TIMED_OUT");
    result = result + format_attr("source_ip", ip, true);
    result = result + "}" + SERIAL_DELIMITER;
    return result;
}
