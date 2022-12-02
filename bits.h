#ifndef DYNAMIC_BITS_H
#define DYNAMIC_BITS_H

#include <math.h>
#include <stdint.h>
#include <string>

class DynamicBits {
    private:
        uint64_t bits;
        uint8_t size=1;
    public:
        DynamicBits(uint64_t bits_ = 0ULL, uint8_t size_ = 1U) {
            bits = bits_;
            size = size_;
        };
        DynamicBits operator<< (uint64_t shift) {
            DynamicBits * res = new DynamicBits(bits << shift, size + shift);
            return *res;
        }
        DynamicBits& operator<<= (uint64_t shift) {
            this->bits <<= shift;
            size += shift;
            return *this;
        }
        DynamicBits& operator>>= (uint64_t shift) {
            this->bits >>= shift;
            size -= shift;
            return *this;
        }
        DynamicBits& operator|= (const uint64_t other) {
            bits |= other;
            return *this;
        }
        DynamicBits operator& (const uint64_t other) {
            DynamicBits *res = new DynamicBits(bits & other, size);
            return *res;
        }
        DynamicBits& operator&= (uint64_t other) {
            this->bits &= other;
            return *this;
        }
        uint8_t pop() {
            uint8_t res = bits & 1;
            this->operator>>=(1);
            return res;
        }
        static DynamicBits fromStr(std::string str) {
            DynamicBits res;
            for (char ch : str) {
                if (ch == '0')
                    res <<= 1;
                else if (ch == '1') {
                    res <<= 1;
                    res |= 1;
                }
                else {
                    // TODO: Not implemented
                }
            }
            return res;
        }
        std::string getStr() {
            DynamicBits copy(*this);
            std::string str;
            for (uint8_t i = 0 ; i < size ; i++) {
                uint8_t one_bits = copy.pop();
                if (one_bits == 1)
                    str = "1" + str;
                else
                    str = "0" + str;
            }
            return str;
        }
        uint64_t get() { return bits; }
        uint8_t len() { return size; }
        ~DynamicBits() {};
};

#endif
