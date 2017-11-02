#include "util.h"

uint16_t concat(uint8_t upper, uint8_t lower) {
    return (upper << 8) | lower;
}

ManagedString format_attr(ManagedString attr, ManagedString val, bool last) {
    ManagedString result = ManagedString("\"") 
        + ManagedString(attr) 
        + ManagedString("\":") 
        + ManagedString("\"") 
        + ManagedString(val) 
        + ManagedString("\"");
    if (last)
        return result;
    else
        return result + ",";
}

ManagedString format_attr(ManagedString attr, int val, bool last) {
    ManagedString result = ManagedString("\"") 
        + ManagedString(attr) 
        + ManagedString("\":") 
        + ManagedString(val);
    if (last)
        return result;
    else
        return result + ",";
}
