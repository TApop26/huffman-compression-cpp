#include "Code.h"
#include <cstring>

Code::Code() : top(0) {
    memset(bits, 0, sizeof(bits));
}

bool Code::push_bit(uint8_t bit) {
    if (is_full()) return false;
    // Always write the bit explicitly so re-used positions are correct.
    if (bit) {
        bits[top / 8] |= (1u << (top % 8));
    } else {
        bits[top / 8] &= ~(1u << (top % 8));
    }
    top++;
    return true;
}

bool Code::pop_bit(uint8_t &bit) {
    if (is_empty()) return false;
    top--;
    bit = (bits[top / 8] >> (top % 8)) & 1;
    return true;
}

uint32_t Code::size()    const { return top; }
bool     Code::is_empty()const { return top == 0; }
bool     Code::is_full() const { return top == MAX_CODE_SIZE * 8u; }

uint8_t Code::get_bit(uint32_t index) const {
    return (bits[index / 8] >> (index % 8)) & 1;
}
