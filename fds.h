#ifndef FDS_H
#define FDS_H

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

#define SIZE_OF_CHINK 8

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

#endif