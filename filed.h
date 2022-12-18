#ifndef FILED_H
#define FILED_H

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define __win__
#elif __unix__
    #error "Not implemented for Unix yet"
#else
    #error "Could not recognize your os"
#endif


#include "merkle-tree.h"
#include "bits.h"
#include "hash.h"

#define DEFAULT_SIZE_OF_CHINK 8
#define DEFAULT_FOLDER_NAME ".file_hash"

const int HASH_SIZE = Sha256::SHA256_SIZE;

#define OpenFileAndClose(fileStream, filename, work)                         \
    fileStream.open(filename);                                               \
    if (!fileStream.is_open()) {                                             \
        std::cerr << "ERROR: Could not open file " << filename << std::endl; \
        fileStream.close();                                                  \
        exit(1);                                                             \
    }                                                                        \
    work;                                                                    \
    fileStream.close();

std::vector<std::string>
get_all_files_names_within_folder(std::string foldername, std::string files_name_pattern)
{
#ifdef __win__ 
    std::vector<std::string> names;
    std::string search_path = foldername + "/" + files_name_pattern;
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(reinterpret_cast<LPCSTR>(search_path.c_str()), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        } while (::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind);
    } 
    return names;
#endif  // __win__
}
#endif  // FILED_H
