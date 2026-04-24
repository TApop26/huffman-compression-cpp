#pragma once
#include <cstdint>

class Node {
public:
    Node *left;
    Node *right;
    uint8_t symbol;
    uint64_t frequency;

    Node(uint8_t symbol, uint64_t frequency);
};

Node* node_join(Node* left, Node* right);
