#include "BitWriter.h"
#include <cstring>

BitWriter::BitWriter(FILE* out)
    : outfile(out), byte_pos(0), bit_pos(0), total_bytes(0) {
    memset(buffer, 0, sizeof(buffer));
}

BitWriter::~BitWriter() {
    flush();
}

void BitWriter::flush_buffer() {
    fwrite(buffer, 1, byte_pos, outfile);
    total_bytes += byte_pos;
    byte_pos = 0;
    memset(buffer, 0, sizeof(buffer));
}

void BitWriter::write_bit(uint8_t bit) {
    if (bit) {
        buffer[byte_pos] |= (1u << bit_pos);
    }
    if (++bit_pos == 8) {
        bit_pos = 0;
        if (++byte_pos == BLOCK) {
            flush_buffer();
        }
    }
}

void BitWriter::flush() {
    if (bit_pos > 0) {
        // Partial byte: advance to next byte position, bit is already zero-padded.
        byte_pos++;
        bit_pos = 0;
    }
    if (byte_pos > 0) {
        flush_buffer();
    }
}

uint64_t BitWriter::bytes_written() const {
    return total_bytes;
}
