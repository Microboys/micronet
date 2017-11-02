#pragma once
#include <inttypes.h>
#include "MicroBit.h"

uint16_t concat(uint8_t upper, uint8_t lower);
ManagedString format_attr(ManagedString attr, ManagedString val, bool last=false);
ManagedString format_attr(ManagedString attr, int val, bool last=false);
