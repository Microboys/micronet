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

ManagedString format_attr(ManagedString attr, std::vector<uint16_t> val, bool last) {
    ManagedString result = ManagedString("\"")
        + ManagedString(attr)
        + ManagedString("\":[");
    if (val.size() == 0) {
      result = result + ManagedString("]");
    } else {
      for (uint8_t i = 0; i < val.size()-1; i++){
          result = result + ManagedString(val.at(i)) + ManagedString(", ");
      }
      result = result + ManagedString(val.at(val.size()-1))
       + ManagedString("]");
    }
    if (last)
        return result;
    else
        return result + ",";
}

uint16_t atoi(ManagedString s) {
    uint16_t result = 0;
    for (int i = 0; i < s.length(); i++) {
        char c = s.charAt(i);
        uint16_t value = (uint16_t)(c - '0');
        result = result * 10 + value;
    }
    return result;
}
