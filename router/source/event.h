#pragma once
#include "config.h"
#include "util.h"

ManagedString neighbour_discovered_event(uint16_t ip, uint8_t rssi);
ManagedString router_timed_out_event(uint16_t ip);
