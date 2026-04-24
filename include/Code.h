#pragma once
#include <cstdint>
#include "defines.h"

// Stack of bits used to track the path while traversing the Huffman tree.
class Code {
public:
    uint32_t top;
    uint8_t  bits[MAX_CODE_SIZE];

    Code();
    bool     push_bit(uint8_t bit);   // returns false if full
    bool     pop_bit(uint8_t &bit);   // returns false if empty
    uint32_t size()     const;
    bool     is_empty() const;
    bool     is_full()  const;
    uint8_t  get_bit(uint32_t index) const;
};
