#ifndef _RC4_H_
#define _RC4_H_

#include <vector>
#include <array>
#include <cstdint>

class RC4
{
public:
    RC4(const std::vector<uint8_t> &d);

    ~RC4() = default;

    auto generateKeyChain(const std::vector<uint8_t>& key) -> RC4 &; // Pseudo-random generation algorithm (PRGA)

    auto encode() -> std::vector<uint8_t>;

private:
    auto permutatePossibleBytesFromKey(const std::vector<uint8_t>& key) -> void;

private:
    std::vector<uint8_t> data;
    std::array<uint8_t, 256> S;
    std::vector<uint8_t> keyChain;
};

#endif // _RC4_H_
