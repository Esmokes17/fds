#include <algorithm>
#include <queue>
#include <iostream>
#include <assert.h>

#include "bits.h"
#include "merkle-tree.h"

#define concat_two_vector(a, b) \
    std::move(b.begin(), b.end(), std::back_inserter(a))

template <typename T>
std::vector<T>& operator +(std::vector<T>& vector1, const std::vector<T>& vector2)
{
  // std::vector<T> result(vector1.size() + vector2.size());
  std::vector<T> * result = new std::vector<T>();
    result->reserve( vector1.size() + vector2.size() ); // preallocate memory
    result->insert( result->end(), vector1.begin(), vector1.end() );
    result->insert( result->end(), vector2.begin(), vector2.end() );
    return * result;
}

MerkleTreeNode::MerkleTreeNode(std::string hash)
{
    this->hash = hash;
    this->depth = 1;
    this->right = nullptr;
    this->left = nullptr;
    this->parent = nullptr;
}

merkle_ptr
MerkleTree::build_merkle_tree(const std::shared_ptr<std::vector<std::string>> values)
{
    // check the size of vector
    if (values->size() == 0)
        return std::make_shared<MerkleTree>(nullptr);
    std::queue<merkle_node_ptr> q;
    for (const auto& value: *values)
        q.push(std::make_shared<MerkleTreeNode>(value));
    while (q.size() > 1) {
        size_t q_size = q.size();
        bool is_odd = false;
        if (q_size != 0 && q_size % 2 != 0)
            is_odd = true;
        for (int i = 0 ; i < ceil(q_size / 2) ; i++) {
            // shared ptr
            merkle_node_ptr left = q.front();
            q.pop();
            merkle_node_ptr right = q.front();
            q.pop();
            merkle_node_ptr parent = std::make_shared<MerkleTreeNode>("");
            left->parent = parent;
            right->parent = parent;
            parent->left = left;
            parent->right = right;
            parent->set_depth(parent->get_depth() + left->get_depth());
            q.push(parent);
        }
        if (is_odd) {
            q.push(q.front());
            q.pop();
        }
    }

    merkle_node_ptr head_node = q.back();
    merkle_ptr root = std::make_shared<MerkleTree>(head_node);
    MerkleTree::compute_hash(root->get_head());
    return root;
}

std::shared_ptr<std::vector<merkle_node_ptr>>
MerkleTree::find_leaves(merkle_node_ptr head)
{
    if (head->left == nullptr)
        return std::make_shared<std::vector<merkle_node_ptr>>(std::vector<merkle_node_ptr>{head});

    std::shared_ptr<std::vector<merkle_node_ptr>> leaves = (
        std::make_shared<std::vector<merkle_node_ptr>>(
              (* MerkleTree::find_leaves(head->left))
            + (* MerkleTree::find_leaves(head->right))
        )
    );
    return leaves;
}

DynamicBits
MerkleTree::trace(
    IN merkle_node_ptr head,
    OUT std::vector<std::string>& needed_hashes)
{
    needed_hashes.insert(needed_hashes.end(), head->get_hash());
    merkle_node_ptr parent = head->parent;
    merkle_node_ptr current = head;
    DynamicBits bits;
    while (parent != nullptr) {
        if (current->get_hash() == parent->left->get_hash()) {
            needed_hashes.insert(needed_hashes.end(), parent->right->get_hash());
            bits <<= 1;
            bits |= 1ull;
        } else {
            needed_hashes.insert(needed_hashes.begin(), parent->left->get_hash());
            bits <<= 1;
            bits |= 1ull;
            bits <<= 1;
        }
        current = parent;
        parent = parent->parent;
    }
    bits <<= 1;
    bits |= 1ull;
    return bits;
}

DynamicBits
MerkleTree::get_proof(
    IN std::string key_hash,
    OUT std::vector<std::string>& needed_hashes)
{
    std::shared_ptr<std::vector<merkle_node_ptr>> leaves = find_leaves(head);
    std::vector<merkle_node_ptr>::iterator find = (
        std::find_if(
            leaves->begin(), leaves->end(),
            [key_hash](merkle_node_ptr p) { return p->get_hash() == key_hash; }
        )
    );
    if (find == leaves->end()){
        DynamicBits bits;
        return bits;
    }
    return trace(*find, needed_hashes);
}

DynamicBits
MerkleTree::get_proof(
    IN size_t index,
    OUT std::vector<std::string>& needed_hashes)
{
    auto leaves = MerkleTree::find_leaves(head);
    return MerkleTree::trace(leaves->at(index), needed_hashes);
}

merkle_node_ptr MerkleTree::construct(
            IN uint64_t depth,
            IN DynamicBits& bits,
            IN std::vector<std::string>& needed_hashes,
            IN const uint64_t max_depth)
{
    uint8_t one_bit = bits.pop();
    merkle_node_ptr node = nullptr;
    if (depth == max_depth) {
        if (needed_hashes.size() > 0) {
            std::string node_hash = needed_hashes.front();
            needed_hashes.erase(needed_hashes.begin());
            node = std::make_shared<MerkleTreeNode>(node_hash);
        }
    }
    else if (one_bit == 1) {
        node = std::make_shared<MerkleTreeNode>("");
        node->left = construct(depth + 1, bits, needed_hashes, max_depth);
        node->right = construct(depth + 1, bits, needed_hashes, max_depth);
    }
    else {
        if (needed_hashes.size() > 0) {
            std::string node_hash = needed_hashes.front();
            needed_hashes.erase(needed_hashes.begin());
            node = std::make_shared<MerkleTreeNode>(node_hash);
        }
    }
    return node;
}


merkle_ptr
MerkleTree::proof_of_exist(
    IN std::vector<std::string> needed_hashes,
    IN DynamicBits bits,
    std::string root_hash)
{
    uint64_t max_depth = ceil(log2(needed_hashes.size() - 1)) + 1;

    merkle_ptr root = std::make_shared<MerkleTree>(
        construct(0, bits, needed_hashes, max_depth));
    if (bits.len() > 1 || needed_hashes.size() != 0)
        return nullptr; //TODO: error

    std::string compute_root_hash = root->compute_hash(root->get_head());
    if (compute_root_hash != root_hash)
        return nullptr;
    return root;
}

std::string
MerkleTree::compute_hash(merkle_node_ptr head)
{
    if (head->left == nullptr)
        return head->get_hash();
    std::string left_hash = MerkleTree::compute_hash(head->left);
    std::string right_hash = MerkleTree::compute_hash(head->right);
    std::string hash = Sha256(left_hash + right_hash).encode();
    head->set_hash(hash);
    return head->get_hash();
}