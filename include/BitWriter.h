#pragma once
#include <cstdio>
#include <cstdint>
#include "defines.h"

// Buffers bits and flushes full bytes to the output file in BLOCK-sized chunks.
class BitWriter {
public:
    explicit BitWriter(FILE* out);
    ~BitWriter();

    void     write_bit(uint8_t bit);
    void     flush();               // writes any remaining buffered bits (zero-padded)
    uint64_t bytes_written() const;

private:
    FILE*    outfile;
    uint8_t  buffer[BLOCK];
    uint32_t byte_pos;  // next byte index in buffer
    uint8_t  bit_pos;   // next bit index within current byte (0-7)
    uint64_t total_bytes;

    void flush_buffer();
};
