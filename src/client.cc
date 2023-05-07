#include "client.h"
#include "rc4.h"

#include <vector>
#include <iostream>
#include <cstring>

static std::vector<Client *> clientManager;
uint64_t internalIdManager = 0;

Client::Client(uint64_t localPrivateNumber, uint64_t PrimeNumberLimiterOfEllipticCurve)
    : m_PrimeNumberLimiter{PrimeNumberLimiterOfEllipticCurve}, m_localPrivateNum{localPrivateNumber}, m_recievedMessages{{}}
{
    clientManager.emplace_back(this);
    this->m_id = internalIdManager++;
    std::cout << "Client(" << this->m_id << ") was created" << std::endl;
}

auto Client::recieveMessageExchange(uint64_t n) -> uint64_t
{
    auto agreedUponSecretKey = this->m_localPrivateNum * n;

    uint8_t result[sizeof(agreedUponSecretKey)];

    std::memcpy(result, &agreedUponSecretKey, sizeof(agreedUponSecretKey));

    this->secretKeyChain = std::vector<uint8_t>(std::begin(result), std::end(result));

    return agreedUponSecretKey;
}

auto Client::getFirstMessageExchange() const -> uint64_t
{
    return this->m_localPrivateNum * this->m_PrimeNumberLimiter;
}

auto Client::sendMessage(std::size_t clientId) -> void
{
    auto encodedMessage = RC4(this->m_messageBuffer).generateKeyChain(this->secretKeyChain).encode();

    clientManager.at(clientId)->recieveMessage(encodedMessage);
}

auto Client::recieveMessage(const std::vector<uint8_t> &message) -> void
{
    auto encodedMessage = RC4(message).generateKeyChain(this->secretKeyChain).encode();

    this->m_recievedMessages.push(encodedMessage);
    std::cout << "Client(" << this->m_id << ") recieved a new message" << std::endl;
}

auto Client::writeMessage(const std::vector<uint8_t> &message) -> void
{
    this->m_messageBuffer = message;
    std::cout << "A message was written to Client(" << this->m_id << ")" << std::endl;
}

auto Client::consumeMessage() -> std::vector<uint8_t>
{
    std::vector<uint8_t> m = this->m_recievedMessages.front();
    this->m_recievedMessages.pop();
    return m;
}

auto makeHandShakeBetween2Clients(std::size_t firstClientId, std::size_t secondClientId) -> bool
{
    auto c1 = clientManager.at(firstClientId);
    auto c2 = clientManager.at(secondClientId);

    auto m1 = c1->getFirstMessageExchange();
    auto m2 = c2->getFirstMessageExchange();

    return c1->recieveMessageExchange(m2) == c2->recieveMessageExchange(m1);
}
