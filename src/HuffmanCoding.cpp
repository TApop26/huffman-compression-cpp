#include "HuffmanCoding.h"
#include <queue>
#include <vector>
#include <stack>

Node* HuffmanCoding::build_tree(uint64_t histogram[ALPHABET]) {
    // Min-heap: node with lowest frequency is dequeued first.
    auto cmp = [](Node* a, Node* b) { return a->frequency > b->frequency; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);

    for (int i = 0; i < ALPHABET; i++) {
        if (histogram[i] > 0) {
            pq.push(new Node(static_cast<uint8_t>(i), histogram[i]));
        }
    }

    // Repeatedly join the two least-frequent nodes until one root remains.
    while (pq.size() > 1) {
        Node* left  = pq.top(); pq.pop();   // first dequeued → left child
        Node* right = pq.top(); pq.pop();   // second dequeued → right child
        pq.push(node_join(left, right));
    }

    return pq.empty() ? nullptr : pq.top();
}

void HuffmanCoding::fill_codes(Node* node, Code& current, Code table[ALPHABET]) {
    if (!node) return;

    if (!node->left && !node->right) {
        // Leaf: the accumulated path is this symbol's code.
        table[node->symbol] = current;
        return;
    }

    uint8_t bit;
    current.push_bit(0);
    fill_codes(node->left, current, table);
    current.pop_bit(bit);

    current.push_bit(1);
    fill_codes(node->right, current, table);
    current.pop_bit(bit);
}

void HuffmanCoding::build_codes(Node* root, Code table[ALPHABET]) {
    Code current;
    fill_codes(root, current, table);
}

Node* HuffmanCoding::reconstruct_tree(uint8_t* tree_dump, uint32_t tree_size) {
    std::stack<Node*> stk;
    for (uint32_t i = 0; i < tree_size; i++) {
        if (tree_dump[i] == 'L') {
            i++;
            stk.push(new Node(tree_dump[i], 0));
        } else if (tree_dump[i] == 'I') {
            Node* right = stk.top(); stk.pop();  // first pop → right child
            Node* left  = stk.top(); stk.pop();  // second pop → left child
            stk.push(node_join(left, right));
        }
    }
    return stk.top();
}
