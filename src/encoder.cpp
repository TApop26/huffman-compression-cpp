#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#include "defines.h"
#include "Node.h"
#include "Code.h"
#include "HuffmanCoding.h"
#include "encoder.h"

static void print_help(const char* prog) {
    std::cerr
        << "Usage " << prog << ": \n"
        << "huffman encode [-h] [-i infile] [-o outfile] [-s]\n\n"
        << "OPTIONS\n"
        << "  -h           Print this help message and exit.\n"
        << "  -i infile    Input file to encode (default: stdin).\n"
        << "  -o outfile   Output file for encoded data.\n"
        << "  -s           Print compression statistics to stderr.\n";
}

// Collects the Huffman tree in post-order as a sequence of bytes.
// Leaf node  -> 'L' + symbol byte.
// Inner node -> 'I'.
static void collect_tree(Node* node, std::vector<uint8_t>& out) {
    if (!node) return;
    if (!node->left && !node->right) {
        out.push_back('L');
        out.push_back(node->symbol);
    } else {
        collect_tree(node->left, out);
        collect_tree(node->right, out);
        out.push_back('I');
    }
}

static void free_tree(Node* node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    delete node;
}

int run_encoder(int argc, char* argv[]) {
    FILE*       infile      = stdin;
    FILE*       outfile     = stdout;
    const char* infile_name = nullptr;
    bool        print_stats = false;

    optind = 1;
    int opt;
    while ((opt = getopt(argc, argv, "hi:o:s")) != -1) {
        switch (opt) {
            case 'h': print_help(argv[0]); return 0;
            case 'i':
                infile_name = optarg;
                infile = fopen(optarg, "rb");
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

    // ── Pass 1: get file size and build frequency histogram ──────────────

    uint64_t file_size = 0;
    std::vector<uint8_t> input_buf;

    if (infile_name) {
        fseek(infile, 0, SEEK_END);
        file_size = static_cast<uint64_t>(ftell(infile));
        rewind(infile);
    } else {
        uint8_t tmp[BLOCK];
        size_t n;
        while ((n = fread(tmp, 1, BLOCK, stdin)) > 0)
            input_buf.insert(input_buf.end(), tmp, tmp + n);
        file_size = static_cast<uint64_t>(input_buf.size());
    }

    uint64_t histogram[ALPHABET] = {};
    if (infile_name) {
        uint8_t buf[BLOCK]; size_t n;
        while ((n = fread(buf, 1, BLOCK, infile)) > 0)
            for (size_t i = 0; i < n; i++) histogram[buf[i]]++;
        rewind(infile);
    } else {
        for (uint8_t b : input_buf) histogram[b]++;
    }

    // ── Build tree and code table ─────────────────────────────────────────

    uint64_t real_hist[ALPHABET];
    memcpy(real_hist, histogram, sizeof(histogram));

    // Guarantee at least two distinct symbols so the tree is always valid.
    histogram[0]++;
    histogram[255]++;

    Node* root = HuffmanCoding::build_tree(histogram);
    Code  table[ALPHABET];
    HuffmanCoding::build_codes(root, table);

    // Total encoded bits from real histogram x code lengths.
    uint64_t total_bits = 0;
    for (int i = 0; i < ALPHABET; i++)
        total_bits += real_hist[i] * table[i].size();

    // Serialise the tree (post-order).
    std::vector<uint8_t> tree_bytes;
    collect_tree(root, tree_bytes);

    // ── Write encoded file ────────────────────────────────────────────────
    //
    //  Line 1 : original file size (decimal)
    //  Line 2 : number of tree bytes (decimal)
    //  Line 3 : tree bytes as space-separated decimals
    //  Line 4 : total encoded bit count (decimal)
    //  Line 5 : encoded bit-stream as ASCII '0'/'1' characters
    //
    fprintf(outfile, "%llu\n", (unsigned long long)file_size);
    fprintf(outfile, "%u\n",   (unsigned)tree_bytes.size());
    for (size_t i = 0; i < tree_bytes.size(); i++) {
        if (i > 0) fputc(' ', outfile);
        fprintf(outfile, "%u", (unsigned)tree_bytes[i]);
    }
    fputc('\n', outfile);
    fprintf(outfile, "%llu\n", (unsigned long long)total_bits);

    // ── Pass 2: write encoded bits as ASCII '0'/'1' ───────────────────────
    auto write_symbol = [&](uint8_t b) {
        const Code& c = table[b];
        for (uint32_t j = 0; j < c.size(); j++)
            fputc(c.get_bit(j) ? '1' : '0', outfile);
    };

    if (infile_name) {
        uint8_t buf[BLOCK]; size_t n;
        while ((n = fread(buf, 1, BLOCK, infile)) > 0)
            for (size_t i = 0; i < n; i++) write_symbol(buf[i]);
    } else {
        for (uint8_t b : input_buf) write_symbol(b);
    }
    fputc('\n', outfile);

    // ── Statistics ────────────────────────────────────────────────────────
    if (print_stats) {
        uint64_t compressed_bytes = (total_bits + 7) / 8;
        double   saving = 100.0 * (1.0 - static_cast<double>(compressed_bytes)
                                         / static_cast<double>(file_size));
        std::cerr << "Uncompressed file size: " << file_size        << " bytes\n"
                  << "Compressed file size:   " << compressed_bytes << " bytes\n"
                  << "Space saving:           " << saving           << "%\n";
    }

    free_tree(root);
    if (infile  != stdin)  fclose(infile);
    if (outfile != stdout) fclose(outfile);
    return 0;
}
