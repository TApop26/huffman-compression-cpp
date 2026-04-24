#include "Node.h"

Node::Node(uint8_t sym, uint64_t freq)
    : left(nullptr), right(nullptr), symbol(sym), frequency(freq) {}

// Creates a parent node whose children are left and right.
// Parent symbol is '$', frequency is the sum of children's frequencies.
Node* node_join(Node* left, Node* right) {
    Node* parent = new Node('$', left->frequency + right->frequency);
    parent->left  = left;
    parent->right = right;
    return parent;
}
