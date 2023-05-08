#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <bits/c++config.h>
#include <bits/stdint-uintn.h>
#include <vector>
#include <queue>
#include <memory>

#include "httplib.h"

class Client
{
public:
    Client(uint64_t localPrivateNumber = 0, uint64_t PrimeNumberLimiterOfEllipticCurve = 11);

    auto sendMessageToInternalClient(std::size_t clientId) -> void;
    auto sendMessageOverNetwork() -> void;

    auto writeMessage(const std::vector<uint8_t> &message) -> void;

    auto consumeMessage() -> std::vector<uint8_t>;

    auto connect(std::string_view remoteHost, uint32_t remotePort, std::string_view localHost, uint32_t localPort) -> void;

    auto isThereUnreadMessages() -> bool;

    auto listen(std::string_view host, uint32_t port) -> void;

    friend auto makeHandShakeBetween2InternalClients(std::size_t firstClient, std::size_t secondClient) -> bool;

    ~Client() = default;

private:
    auto recieveMessageExchange(uint64_t n) -> uint64_t;

    auto getFirstMessageExchange() const -> uint64_t;

    auto recieveMessage(const std::vector<uint8_t> &message) -> void;

private:
    uint64_t m_PrimeNumberLimiter;
    uint64_t m_localPrivateNum;
    std::vector<uint8_t> m_secretKeyChain;
    uint64_t m_id;
    std::queue<std::vector<uint8_t>> m_recievedMessages;
    std::vector<uint8_t> m_messageBuffer;

private:
    httplib::Server internalNetworkServer;
    std::unique_ptr<httplib::Client> internalNetworkClient;
    std::string remoteHost;
    uint32_t remotePort;
    std::string internalHost;
    uint32_t internalPort;
};

auto makeHandShakeBetween2InternalClients(std::size_t firstClient, std::size_t secondClient) -> bool;

#endif // _CLIENT_H_
