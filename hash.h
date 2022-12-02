#ifndef HASH_H
#define HASH_H

#include <string>
#include <cstdint>
#include <cstring>

const uint8_t BIT_SIZE = 8;
typedef uint8_t byte_t;

class Hash {
    protected:
        char* result;
        byte_t* inputData;
        uint64_t inputSize;
        void setResult(char*);
    public:
        Hash(std::string hash_value);
        void setInput(char*);
        //virtual void encode() const = 0;
        char * getResult() {
            // size_t size_str = sizeof(result) / sizeof(result[0]);
            size_t size_str = strlen(result);
            char * str = new char[size_str + 1];
            memcpy(str, result, size_str);
            str[size_str] = 0;
            return str;
        }
};

// rotate right
#define ror(X, n) ( (x >> n) | (x << (32-n)) )
// shift right
#define shr(X, n) ( X >> n )

class Sha256 : public Hash {
    public:
        static const uint8_t SHA256_SIZE = 64;
        Sha256(std::string);
        std::string encode();
        void init();
        static void selfTest();
        std::string getResult() { return str; }
    private:
        std::string str;
        static const uint32_t k[];
        uint32_t hashValue[8];
        uint64_t digest = 0;
        uint32_t w[SHA256_SIZE];
        unsigned get_last_n_bits(unsigned u, int n);
};

#endif