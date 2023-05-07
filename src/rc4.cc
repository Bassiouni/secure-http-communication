#include "rc4.h"
#include "zip.h"

#include <cassert>

RC4::RC4(const std::vector<uint8_t> &d)
    : data{d}, S{{}}, keyChain{{}}
{
    for (uint16_t i = 0; i <= 255; i++)
    {
        this->S[i] = i; // initialize permutation array
    }
}

auto RC4::generateKeyChain(const std::vector<uint8_t> &key) -> RC4 & // Pseudo-random generation algorithm (PRGA)
{
    assert(1 <= key.size() && key.size() <= 256);

    // Key-scheduling algorithm (KSA)
    this->permutatePossibleBytesFromKey(key);

    std::size_t i, j;
    i = j = 0;

    for (std::vector<uint8_t>::iterator it = this->data.begin(); it != this->data.end(); it++)
    {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        uint8_t &K = S[(S[i] + S[j]) % 256];
        keyChain.push_back(K);
    }

    return *this;
}

auto RC4::encode() -> std::vector<uint8_t>
{
    std::vector<uint8_t> byteStream;

    for (auto &&[e, K] : zip(this->data, this->keyChain))
    {
        byteStream.push_back(e ^ K);
    }

    return byteStream;
}

auto RC4::permutatePossibleBytesFromKey(const std::vector<uint8_t> &key) -> void
{
    std::size_t j = 0;
    for (uint16_t i = 0; i <= 255; i++)
    {
        j = (j + this->S[i] + key[i % key.size()]) % 256;
        std::swap(this->S[i], this->S[j]);
    }
}
