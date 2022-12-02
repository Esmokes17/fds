#include <sstream>
#include <iomanip>
#include <math.h>
#include <assert.h>

#include "hash.h"

Hash::Hash(std::string hash_value) {
    size_t size_str = hash_value.size();
    result = new char[size_str + 1];
    memcpy(result, hash_value.c_str(), hash_value.size());
    result[size_str] = 0;
}

void Hash::setResult(char* newResult){
    result = newResult;
}

void Hash::setInput(char* newData){
    inputSize = strlen(newData);

    inputData = new byte_t[inputSize];
    memcpy(inputData, newData, inputSize);
}

// sha256 functions
uint32_t inline Ch(uint32_t x, uint32_t y, uint32_t z) { return z ^ (x & (y ^ z)); }
uint32_t inline Maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (z & (x | y)); }
uint32_t inline Sigma0(uint32_t x) { return ror(x, 2) ^ ror(x, 13) ^ ror(x, 22); }
uint32_t inline Sigma1(uint32_t x) { return ror(x, 6) ^ ror(x, 11) ^ ror(x, 25); }
uint32_t inline sigma0(uint32_t x) { return ror(x, 7) ^ ror(x, 18) ^ shr(x, 3); }
uint32_t inline sigma1(uint32_t x) { return ror(x, 17) ^ ror(x, 19) ^ shr(x, 10); }

const uint32_t Sha256::k[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

Sha256::Sha256(std::string input) : Hash(""){
    setInput(const_cast<char*>(input.c_str()));
    result = new char[SHA256_SIZE + 1];
}

void Sha256::init(){
    hashValue[0] = 0x6a09e667;
    hashValue[1] = 0xbb67ae85;
    hashValue[2] = 0x3c6ef372;
    hashValue[3] = 0xa54ff53a;
    hashValue[4] = 0x510e527f;
    hashValue[5] = 0x9b05688c;
    hashValue[6] = 0x1f83d9ab;
    hashValue[7] = 0x5be0cd19;
}

std::string Sha256::encode(){
    init();
    int paddingZeroSize = 64 - ((inputSize + 8) % 64);
    const int preProcessingSize = (inputSize + 8 ) +  paddingZeroSize;
    byte_t* pre = new byte_t[preProcessingSize];
    byte_t* ptr = pre;
    for (size_t i = 0; i < inputSize; i++, ptr++) {
        *ptr = inputData[i];
    }
    // pad '1000 0000' bits
    *ptr = 128;
    ptr++;

    for (int i = 0; i < paddingZeroSize - 1; i++, ptr++) {
        *ptr = 0;
    }

    uint64_t temp_inputSize = inputSize * 8;
    uint64_t mask = 0xFF00000000000000;
    for (int i = 0; i < 8; i++, ptr++) {
        *ptr = (temp_inputSize & (mask)) >> 56;
        temp_inputSize <<= 8;
    }

    const int nChunk = ceil(preProcessingSize / 64.);

    uint32_t * w = new uint32_t[64]{0};

    for (int i = 0; i < nChunk; i++) {
        uint32_t a = hashValue[0];
        uint32_t b = hashValue[1];
        uint32_t c = hashValue[2];
        uint32_t d = hashValue[3];
        uint32_t e = hashValue[4];
        uint32_t f = hashValue[5];
        uint32_t g = hashValue[6];
        uint32_t h = hashValue[7];

        memset(w, 0, 64 * 4);
        for (int j = 0; j < 64; j++) {
            int index = (i * 64) + (j * 4);
            for (int k = 0; k < 4; k++) {
                w[j] <<= 8;
                w[j] |= pre[index + k];
            }
        }

        memset(&w[16], 0, (64 - 16) * 4);
        for (int j = 16; j < 64; j++) {
            w[j] = sigma0(w[j - 15]) + w[j - 7] + sigma1(w[j - 2]) + w[j - 16];
        }

        for (int j = 0; j < 64; j++) {
            uint32_t temp1 = h + Sigma1(e) + Ch(e, f, g) + k[j] + w[j];
            uint32_t temp2 = Sigma0(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp2 + temp1;
        }
        hashValue[0] = hashValue[0] + a;
        hashValue[1] = hashValue[1] + b;
        hashValue[2] = hashValue[2] + c;
        hashValue[3] = hashValue[3] + d;
        hashValue[4] = hashValue[4] + e;
        hashValue[5] = hashValue[5] + f;
        hashValue[6] = hashValue[6] + g;
        hashValue[7] = hashValue[7] + h;
    }

    delete[] w;
    delete[] pre;

    std::stringstream sstream;
    for (int i = 0; i < 8; i++) {
        sstream << std::setfill('0') << std::setw(8) << std::hex << hashValue[i];
        // memcpy(&result[i * 8], sstream.str().c_str(), 8);
    }
    // end of string
    // result[SHA256_SIZE] = 0;

    // std::string * res = new std::string(result);
    str = sstream.str();

    return str;
}