#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "filed.h"

int chunk_size = DEFAULT_SIZE_OF_CHINK;
std::string folder_name = DEFAULT_FOLDER_NAME;

std::string file_name;    // file name that should be hash
int chunk_index = -1;     // the index of chunk that is write in file or print on screen
bool print_only = false;  // just print the proof of exist or write in the files 
bool exist_proof = false; // if it is true that means get hash of chunk from folder (before calculated)

void print_merkle(merkle_node_ptr tree) {
    // TODO: delete all files of chunk_* in folder_name before writting
    if (tree == nullptr)
        return;
    print_merkle(tree->left);
    print_merkle(tree->right);
    std::string hash = tree->get_hash();
}

void write_merkle(merkle_node_ptr tree) {
    if (tree == nullptr)
        return;
    std::vector<merkle_node_ptr> leaves = *MerkleTree::find_leaves(tree);
    std::ostringstream oss;
    std::ofstream file;
    for (size_t i = 0 ; i < leaves.size() ; i++) {
        oss << folder_name << "/chunk_" << i;
        OpenFileAndClose(file, oss.str(), {
            file << leaves.at(i)->get_hash();
        });
        oss.str("");  // clear string stream
    }
}

std::shared_ptr<std::vector<std::string>>
get_chunks(std::string content) {
    size_t start = 0;
    const size_t number_of_chunk = ceil(content.size() / (float)chunk_size);
    std::shared_ptr<std::vector<std::string>> chunks(new std::vector<std::string>);
    for (size_t i = 0 ; i < number_of_chunk ; i++) {
        std::string chunk = content.substr(start, chunk_size);
        Sha256 sh(chunk);
        chunks->push_back(sh.encode());
        start += chunk_size;
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
        std::string hash = need_hash.at(i);
        std::cout << "Hash " << i << " '" << hash << "' is written!" << std::endl;

        if (!print_only) {
            std::ostringstream oss;
            oss << folder_name << "/hash_" << i;
            OpenFileAndClose(hash_file, oss.str(), {
                    hash_file.write(hash.data(), hash.size());
                });
        }
    }
}

void write_root_hash(const std::string root_hash) {
    std::cout << "Root hash '" << root_hash << "' is written!" << std::endl;
    if (!print_only) {
        std::ofstream root_file;
        OpenFileAndClose(root_file, folder_name + "/root", {
                root_file.write(root_hash.data(), root_hash.size());
            });
    }
}

void write_index(const std::string bits_str) {
    std::cout << "Index '" << bits_str << "' is written!" << std::endl;
    if (!print_only) {
        std::ofstream index_file;
        OpenFileAndClose(index_file, folder_name + "/index", {
                index_file.write(bits_str.data(), bits_str.size());
            });
    }
}

void print_usage() {
    std::cout << "Usage: main [options] <file-name>...\n" << std::endl;    
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                                  Print usage" << std::endl;
    std::cout << "  --folder-name <folder-name=./.file_hash>    Folder that you want hashes are saved in there." << std::endl;
    std::cout << "  --chunk <number=8>                          Size of chunk to split file." << std::endl;
    std::cout << "  --get-proof <index>                         Write the things that you need for proof this index chunk in folder-name." << std::endl;
    std::cout << "  --print-only                                For get proof if this option set then will not write proof in the files." << std::endl;
    std::cout << "  --get-proof-from <filename>                 Get the things that you need for proof this index chunk from files hash in folder-name." << std::endl;
}

void parse_argv(int argc, char * argv[]) {
    if (argc <= 1) {
        std::cerr << "Error: You should pass filename and part of that for get proof to hash" << std::endl;
        print_usage();
        exit(1);
    }

    for (int i = 1 ; i < argc ; i++) {
        // option argv
        const char * arg = argv[i];
        if (arg[0] == '-') {
            if ((!strcmp(arg, "--help")) || (!strcmp(arg, "-h"))) {
                print_usage();
                exit(1);
            }
            else if (!strcmp(arg, "--folder-name")) {
                folder_name = argv[++i];
            }
            else if (!strcmp(arg, "--chunk")) {
                chunk_size = atoi(argv[++i]);
            }
            else if (!strcmp(arg, "--get-proof")) {
                chunk_index = std::stoi(argv[++i]);
            }
            else if (!strcmp(arg, "--print-only")) {
                print_only = true;
            }
            else if (!strcmp(arg, "--get-proof-from")) {
                exist_proof = true;
                folder_name = argv[++i];
            }
            else {
                std::cerr << "ERROR: unknown option " << arg << std::endl;
                std::cout << std::endl;
                print_usage();
                exit(1);
            }
        }
        // it should be filename
        else {
            file_name = arg;
        }
    }
}

int main(int argc, char * argv[]) {
    parse_argv(argc, argv);

    std::shared_ptr<std::vector<std::string>> chunks(new std::vector<std::string>);
    // get chunk of hash from a folder (before calculated)
    if (exist_proof) {
        std::vector<std::string> chunk_files = get_all_files_names_within_folder(folder_name, "chunk_*");
        std::ifstream file;
        std::string str;
        for (std::string file_name : chunk_files) {
            OpenFileAndClose(file, folder_name + "/" + file_name, {
                    file >> str;
                    chunks->push_back(str);
            });
        }
    }
    // calculate hash of chunk
    else {
        // read whole file
        // TODO: efficient way for big files
        std::stringstream buffer;
        std::ifstream file(argv[1]);
        buffer << file.rdbuf();
        file.close();

        chunks = get_chunks(buffer.str());
    }
    // build merkle tree and get proof
    merkle_ptr tree = MerkleTree::build_merkle_tree(chunks);
    // print_merkle(fd, tree->get_head());

    std::vector<std::string> need_hash;
    std::cout << "**************************" << std::endl;
    std::cout << "Hashes file '" << file_name << "' for chunk size " << chunk_size << " has been written in path '" << folder_name << "'" << std::endl;
    if (create_folder(folder_name)) {
        std::cerr << "ERROR: Could not create folder for outputs!" << std::endl;
        exit(1);
    }
    write_merkle(tree->get_head());
    
    std::cout << "**************************" << std::endl;
    // get proof
    if (chunk_index > -1 ) {
        if (chunk_index >= (int) chunks->size() || chunk_index < 0) {
            std::cerr << "ERROR: Out of range index that you provided!" << std::endl;
            exit(1);
        }
        DynamicBits bits = tree->get_proof(chunk_index, need_hash);    
        write_needed_hashes(need_hash);
        write_root_hash(tree->get_head()->get_hash());
        write_index(bits.getStr());
    }
    
    return 0;
}

