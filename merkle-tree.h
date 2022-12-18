#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <vector>
#include <list>
#include <memory>

#include "hash.h"
#include "bits.h"

#define IN
#define OUT

class MerkleTreeNode;
class MerkleTree;

using merkle_node_ptr = std::shared_ptr<MerkleTreeNode>;
using merkle_ptr = std::shared_ptr<MerkleTree>;

class MerkleTreeNode {
    private:
        uint64_t depth;
        // std::shared_ptr<Hash> hash;
        std::string hash;
    public:
        merkle_node_ptr right;
        merkle_node_ptr left;
        merkle_node_ptr parent;
        MerkleTreeNode(std::string);

        void set_hash(std::string hash) { this->hash = hash; }
        void increase_depth() { depth++; }
        std::string get_hash() { return hash; }
        uint64_t get_depth() { return depth; }
        void set_depth(uint64_t depth) { this->depth = depth; }
};

class MerkleTree {
    private:
        merkle_node_ptr head;
        static DynamicBits trace(
                IN merkle_node_ptr head,
                OUT std::vector<std::string>& needed_hashes);
        static merkle_node_ptr construct(
            IN uint64_t depth,
            IN DynamicBits& bits,
            IN std::vector<std::string>& needed_hashes,
            IN const uint64_t max_depth
        );
    public:
        MerkleTree(merkle_node_ptr head) {
            this->head = head;
        }
        static merkle_ptr build_merkle_tree(const std::shared_ptr<std::vector<std::string>> values);
        DynamicBits get_proof(
            IN std::string key_hash,
            OUT std::vector<std::string>& needed_hashes);
        DynamicBits get_proof(
            IN size_t index,
            OUT std::vector<std::string>& needed_hashes);
        static merkle_ptr proof_of_exist(
            IN std::vector<std::string> needed_hashes,
            IN DynamicBits bits,
            std::string root_hash
        );
        static std::shared_ptr<std::vector<merkle_node_ptr>> find_leaves(merkle_node_ptr head);
        merkle_node_ptr get_head() { return head; }
        static std::string compute_hash(merkle_node_ptr head);
        // ~MerkleTree();
};

#endif
