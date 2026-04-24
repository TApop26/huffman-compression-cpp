cmake_minimum_required(VERSION 3.10)
project(HuffmanCompression)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(COMMON_SOURCES
        src/Node.cpp
        src/Code.cpp
        src/HuffmanCoding.cpp
)

# Single unified executable: huffman encode ... / huffman decode ...
add_executable(huffman
        src/main.cpp
        src/encoder.cpp
        src/decoder.cpp
        ${COMMON_SOURCES}
)
