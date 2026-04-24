#pragma once
#include "Node.h"
#include "Code.h"
#include "defines.h"

class HuffmanCoding {
public:
    // Build a Huffman tree from a 256-entry frequency histogram.
    static Node* build_tree(uint64_t histogram[ALPHABET]);

    // Populate table[0..255] with the Huffman codes derived from root.
    static void build_codes(Node* root, Code table[ALPHABET]);

    // Reconstruct a Huffman tree from its post-order byte dump (used by decoder).
    static Node* reconstruct_tree(uint8_t* tree_dump, uint32_t tree_size);

private:
    static void fill_codes(Node* node, Code& current, Code table[ALPHABET]);
};
