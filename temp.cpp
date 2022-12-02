#include <memory>
#include <iostream>
#include <vector>
#include <math.h>

#include "merkle-tree.h"

void print_merkle(merkle_node_ptr tree) {
    if (tree == nullptr)
        return;
    print_merkle(tree->left);
    print_merkle(tree->right);
    std::string hash = tree->get_hash();
    std::cout << hash << " " << std::endl;
}

int main() {
    std::vector<std::string> values;
    values.push_back("1");
    values.push_back("2");
    values.push_back("3");
    values.push_back("4");
    values.push_back("5");
    values.push_back("6");
    merkle_ptr tree = MerkleTree::build_merkle_tree(values);
    print_merkle(tree->get_head());
    std::cout << "**************************" << std::endl;
    std::vector<std::string> need_hash;
    DynamicBits bits = tree->get_proof("12", need_hash);
    merkle_ptr make_tree = MerkleTree::proof_of_exist(need_hash, bits, tree->get_head()->get_hash());
    if (make_tree == nullptr) std::cerr << "Not proof" << std::endl;
    else std::cout << "Proof root hash: " << make_tree->get_head()->get_hash() << std::endl;
    print_merkle(make_tree->get_head());
    return 0;
}