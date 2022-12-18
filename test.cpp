#include <iostream>
#include <vector>
#include <fstream>

#include "filed.h"

const std::string FOLDER_NAME = ".file_hash";


std::vector<std::string> get_hash_from_folder(std::string foldername = FOLDER_NAME)
{
    std::vector<std::string> hash_files = get_all_files_names_within_folder(foldername, "hash_*");
    std::vector<std::string> hashes;
    std::ifstream in;
    for (std::string name : hash_files) {
        OpenFileAndClose(in, foldername + "/" + name, {
            char str[HASH_SIZE + 1];
            in.read(str, HASH_SIZE);
            str[HASH_SIZE] = 0;
            hashes.push_back(str);
        });
    }
    return hashes;
}

std::string get_root_hash_from_folder(std::string foldername = FOLDER_NAME)
{
    std::ifstream in;
    char str[HASH_SIZE + 1];
    OpenFileAndClose(in, foldername + "/root", {
        in.read(str, HASH_SIZE);
        str[HASH_SIZE] = 0;
    });
    return str;
}

std::string get_index_from_folder(std::string foldername = FOLDER_NAME)
{
    std::ifstream in;
    std::string str;
    OpenFileAndClose(in, foldername + "/index", {
        in >> str;
    });
    return str;
}

int main() {
    std::vector<std::string> need_hash = get_hash_from_folder();
    for (size_t i = 0 ; i < need_hash.size() ; i++) {
        std::cout << "Hash " << i << " '" << need_hash.at(i) << "' is read!" << std::endl;
    }
    std::string root_hash = get_root_hash_from_folder();
    std::cout << "Root hash '" << root_hash << "' is read!" << std::endl;
    std::string index = get_index_from_folder();
    std::cout << "index '" << index << "' is read!" << std::endl;

    merkle_ptr make_tree = MerkleTree::proof_of_exist(need_hash, DynamicBits::fromStr(index), root_hash);
    if (make_tree == nullptr) std::cerr << "Not proof" << std::endl;
    else std::cout << "Proof root hash: " << make_tree->get_head()->get_hash() << std::endl;
    return 0;
}
