#pragma once
#include <cstdint>

#define BLOCK        4096
#define ALPHABET     256
#define MAGIC        0xDEADEAEF
#define MAX_CODE_SIZE (ALPHABET / 8)        // 32 bytes = 256 bits max code length
#define MAX_TREE_SIZE (3 * ALPHABET - 1)    // max bytes for the tree dump

struct Header {
    uint32_t magic;
    uint32_t tree_size;
    uint64_t file_size;
};
