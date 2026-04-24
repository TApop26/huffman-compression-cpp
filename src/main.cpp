#include <iostream>
#include <string>

#include "encoder.h"
#include "decoder.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: huffman encode|decode [-h] [-i infile] [-o outfile] [-s]\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "encode") {
        return run_encoder(argc - 1, argv + 1);
    } else if (mode == "decode") {
        return run_decoder(argc - 1, argv + 1);
    } else {
        std::cerr << "Unknown mode '" << mode << "'. Use 'encode' or 'decode'.\n";
        return 1;
    }
}
