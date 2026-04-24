#include "BitReader.h"
#include <cstring>

BitReader::BitReader(FILE* in)
    : infile(in), byte_pos(0), bytes_in_buf(0), bit_pos(0), total_bytes(0) {}

bool BitReader::fill_buffer() {
    bytes_in_buf = static_cast<uint32_t>(fread(buffer, 1, BLOCK, infile));
    byte_pos = 0;
    total_bytes += bytes_in_buf;
    return bytes_in_buf > 0;
}

bool BitReader::read_bit(uint8_t& bit) {
    if (byte_pos >= bytes_in_buf) {
        if (!fill_buffer()) return false;
    }
    bit = (buffer[byte_pos] >> bit_pos) & 1;
    if (++bit_pos == 8) {
        bit_pos = 0;
        byte_pos++;
    }
    return true;
}

uint64_t BitReader::bytes_read() const {
    return total_bytes;
}
