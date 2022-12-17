#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "filed.h"

const std::string FOLDER_NAME = ".file_hash";

void print_merkle(merkle_node_ptr tree) {
    if (tree == nullptr)
        return;
    print_merkle(tree->left);
    print_merkle(tree->right);
    std::string hash = tree->get_hash();
    std::cout << hash << " " << std::endl;
}

std::shared_ptr<std::vector<std::string>>
get_chunks(std::string content) {
    size_t start = 0;
    const size_t number_of_chunk = ceil(content.size() / (float)SIZE_OF_CHINK);
    std::shared_ptr<std::vector<std::string>> chunks(new std::vector<std::string>);
    for (size_t i = 0 ; i < number_of_chunk ; i++) {
        std::string chunk = content.substr(start, SIZE_OF_CHINK);
        Sha256 sh(chunk);
        chunks->push_back(sh.encode());
        start += SIZE_OF_CHINK;
    }
    return chunks;
}

int create_folder(std::string foldername)
{
#ifdef __win__
    if (CreateDirectory(reinterpret_cast<LPCSTR>(foldername.data()), NULL) == 0
        && ERROR_ALREADY_EXISTS != GetLastError()) return 1;
    return 0;
#endif
}


void write_needed_hashes(const std::vector<std::string> need_hash) {
    std::ofstream hash_file;
    for (size_t i = 0 ; i < need_hash.size() ; i++) {
        std::ostringstream oss;
        oss << FOLDER_NAME << "/hash_" << i;
        OpenFileAndClose(hash_file, oss.str(), {
            std::string hash = need_hash.at(i);
            hash_file.write(hash.data(), hash.size());
            std::cout << "Hash " << i << " '" << hash << "' is written!" << std::endl;
        });
    }
}

void write_root_hash(const std::string root_hash) {
    std::ofstream root_file;
    OpenFileAndClose(root_file, FOLDER_NAME + "/root", {
        root_file.write(root_hash.data(), root_hash.size());
        std::cout << "Root hash '" << root_hash << "' is written!" << std::endl;
    });
}

void write_index(const std::string bits_str) {
    std::ofstream index_file;
    OpenFileAndClose(index_file, FOLDER_NAME + "/index", {
        index_file.write(bits_str.data(), bits_str.size());
        std::cout << "Index '" << bits_str << "' is written!" << std::endl;
    });
}

int main(int argc, char * argv[]) {
    if (argc <= 1) {
        std::cerr << "Error: You should pass filename and"
                  << "part of that for get proof to hash" << std::endl;
        return 1;
    }

    // read whole filename filename
    // TODO: efficient way for big files
    std::stringstream buffer;
    std::ifstream file(argv[1]);
    buffer << file.rdbuf();
    file.close();

    std::shared_ptr<std::vector<std::string>> chunks = get_chunks(buffer.str());

    // build merkle tree and get proof
    merkle_ptr tree = MerkleTree::build_merkle_tree(chunks);
    // print_merkle(tree->get_head());
    std::cout << "**************************" << std::endl;
    std::vector<std::string> need_hash;
    int index = std::stoi(argv[2]);
    if (index >= (int) chunks->size() || index < 0) {
        std::cerr << "ERROR: Out of range index that you provided!";
        exit(1);
    }
    DynamicBits bits = tree->get_proof(1, need_hash);

    if (create_folder(FOLDER_NAME)) {
        std::cerr << "ERROR: Could not create folder for outputs!" << std::endl;
        exit(1);
    }
    write_needed_hashes(need_hash);
    write_root_hash(tree->get_head()->get_hash());
    write_index(bits.getStr());

    return 0;
}

