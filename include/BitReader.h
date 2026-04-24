#pragma once
#include <cstdio>
#include <cstdint>
#include "defines.h"

// Buffers bytes from a file and exposes them one bit at a time.
class BitReader {
public:
    explicit BitReader(FILE* in);

    bool     read_bit(uint8_t& bit);  // returns false when no more bits available
    uint64_t bytes_read() const;

private:
    FILE*    infile;
    uint8_t  buffer[BLOCK];
    uint32_t byte_pos;      // current byte index in buffer
    uint32_t bytes_in_buf;  // valid bytes loaded in buffer
    uint8_t  bit_pos;       // current bit index within current byte (0-7)
    uint64_t total_bytes;

    bool fill_buffer();
};
