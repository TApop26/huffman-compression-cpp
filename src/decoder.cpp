#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include "defines.h"
#include "Node.h"
#include "HuffmanCoding.h"
#include "decoder.h"

static void print_help(const char* prog) {
    std::cerr
        << "Usage " << prog << ": \n"
        << "huffman decode [-h] [-i infile] [-o outfile] [-s]\n\n"
        << "OPTIONS\n"
        << "  -h           Print this help message and exit.\n"
        << "  -i infile    Encoded file to decode.\n"
        << "  -o outfile   Output file for decoded text (default: stdout).\n"
        << "  -s           Print decompression statistics to stderr.\n";
}

static void free_tree(Node* node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    delete node;
}

int run_decoder(int argc, char* argv[]) {
    FILE*       infile      = stdin;
    FILE*       outfile     = stdout;
    bool        print_stats = false;

    optind = 1;
    int opt;
    while ((opt = getopt(argc, argv, "hi:o:s")) != -1) {
        switch (opt) {
            case 'h': print_help(argv[0]); return 0;
            case 'i':
                infile = fopen(optarg, "r");
                if (!infile) {
                    std::cerr << "Error: cannot open input file '" << optarg << "'\n";
                    return 1;
                }
                break;
            case 'o':
                outfile = fopen(optarg, "w");
                if (!outfile) {
                    std::cerr << "Error: cannot open output file '" << optarg << "'\n";
                    return 1;
                }
                break;
            case 's': print_stats = true; break;
            default:  print_help(argv[0]); return 1;
        }
    }

    // ── Read encoded file header ──────────────────────────────────────────
    //
    //  Line 1 : original file size
    //  Line 2 : tree byte count
    //  Line 3 : tree bytes (space-separated decimals)
    //  Line 4 : total encoded bit count
    //  Line 5 : encoded bit-stream ('0'/'1' characters)
    //

    char line_buf[64];

    // Line 1: original file size
    if (!fgets(line_buf, sizeof(line_buf), infile)) {
        std::cerr << "Error: failed to read file size.\n"; return 1;
    }
    uint64_t file_size = strtoull(line_buf, nullptr, 10);

    // Line 2: tree byte count
    if (!fgets(line_buf, sizeof(line_buf), infile)) {
        std::cerr << "Error: failed to read tree size.\n"; return 1;
    }
    uint32_t tree_size = (uint32_t)strtoul(line_buf, nullptr, 10);

    // Line 3: tree bytes as space-separated decimals
    std::vector<char> tree_line(4096);
    if (!fgets(tree_line.data(), (int)tree_line.size(), infile)) {
        std::cerr << "Error: failed to read tree bytes.\n"; return 1;
    }
    uint8_t tree_dump[MAX_TREE_SIZE];
    char* ptr = tree_line.data();
    for (uint32_t i = 0; i < tree_size; i++) {
        tree_dump[i] = static_cast<uint8_t>(strtoul(ptr, &ptr, 10));
    }

    // Line 4: total bit count (used for statistics)
    if (!fgets(line_buf, sizeof(line_buf), infile)) {
        std::cerr << "Error: failed to read bit count.\n"; return 1;
    }
    uint64_t total_bits = strtoull(line_buf, nullptr, 10);

    // ── Reconstruct Huffman tree ──────────────────────────────────────────
    Node* root = HuffmanCoding::reconstruct_tree(tree_dump, tree_size);

    // ── Decode bit-stream (Line 5) ────────────────────────────────────────
    Node*    current = root;
    uint64_t decoded = 0;

    while (decoded < file_size) {
        int ch = fgetc(infile);
        if (ch == EOF || ch == '\n') break;

        uint8_t bit = (ch == '1') ? 1 : 0;
        current = bit ? current->right : current->left;

        if (!current->left && !current->right) {
            fputc(current->symbol, outfile);
            decoded++;
            current = root;
        }
    }

    // ── Statistics ────────────────────────────────────────────────────────
    if (print_stats) {
        uint64_t compressed_bytes = (total_bits + 7) / 8;
        double   saving = 100.0 * (1.0 - static_cast<double>(compressed_bytes)
                                         / static_cast<double>(file_size));
        std::cerr << "Compressed file size:   " << compressed_bytes << " bytes\n"
                  << "Decompressed file size: " << file_size        << " bytes\n"
                  << "Space saving:           " << saving           << "%\n";
    }

    free_tree(root);
    if (infile  != stdin)  fclose(infile);
    if (outfile != stdout) fclose(outfile);
    return 0;
}
