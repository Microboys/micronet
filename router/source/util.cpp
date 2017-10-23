#include "util.h"

uint16_t concat(uint8_t upper, uint8_t lower) {
    return (upper << 8) | lower;
}
