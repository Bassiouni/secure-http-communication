#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <bits/c++config.h>
#include <bits/stdint-uintn.h>
#include <vector>
#include <queue>

class Client
{
public:
    Client(uint64_t localPrivateNumber = 0, uint64_t PrimeNumberLimiterOfEllipticCurve = 11);

    auto sendMessage(std::size_t clientId) -> void;

    auto writeMessage(const std::vector<uint8_t> &message) -> void;

    auto consumeMessage() -> std::vector<uint8_t>;

    friend auto makeHandShakeBetween2Clients(std::size_t firstClient, std::size_t secondClient) -> bool;

private:
    auto recieveMessageExchange(uint64_t n) -> uint64_t;

    auto getFirstMessageExchange() const -> uint64_t;

    auto recieveMessage(const std::vector<uint8_t> &message) -> void;

private:
    uint64_t m_PrimeNumberLimiter;
    uint64_t m_localPrivateNum;
    std::vector<uint8_t> secretKeyChain;
    uint64_t m_id;
    std::queue<std::vector<uint8_t>> m_recievedMessages;
    std::vector<uint8_t> m_messageBuffer;
};

auto makeHandShakeBetween2Clients(std::size_t firstClient, std::size_t secondClient) -> bool;

#endif // _CLIENT_H_
