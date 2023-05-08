#include "client.h"
#include "rc4.h"
#include "json.hpp"

#include <cstring>

static std::vector<Client *> clientManager;
uint64_t internalIdManager = 0;

Client::Client(uint64_t localPrivateNumber, uint64_t PrimeNumberLimiterOfEllipticCurve)
    : m_PrimeNumberLimiter{PrimeNumberLimiterOfEllipticCurve}, m_localPrivateNum{localPrivateNumber}, m_recievedMessages{{}}
{
    clientManager.emplace_back(this);
    this->m_id = internalIdManager++;

    this->internalNetworkServer.Post("/recieve", [this](const httplib::Request &req, httplib::Response &res)
    {
        this->recieveMessage(std::vector<uint8_t>(req.body.begin(), req.body.end()));
    });

    this->internalNetworkServer.Get("/handshake", [this](const httplib::Request &req, httplib::Response &res)
    {
        res.set_content(std::to_string(this->getFirstMessageExchange()), "text/plain");
    });

    this->internalNetworkServer.Post("/handshake", [this](const httplib::Request &req, httplib::Response &res)
    {
        auto data = nlohmann::json::parse(req.body);
        this->recieveMessageExchange(data["message"].get<uint64_t>());
        this->remoteHost = data["host"].get<std::string_view>();
        this->remotePort = data["port"].get<uint32_t>();

        this->internalNetworkClient = std::make_unique<httplib::Client>(this->remoteHost, this->remotePort);
    });
}

auto Client::recieveMessageExchange(uint64_t n) -> uint64_t
{
    auto agreedUponSecretKey = this->m_localPrivateNum * n;

    uint8_t result[sizeof(agreedUponSecretKey)];

    std::memcpy(result, &agreedUponSecretKey, sizeof(agreedUponSecretKey));

    this->m_secretKeyChain = std::vector<uint8_t>(std::begin(result), std::end(result));

    return agreedUponSecretKey;
}

auto Client::getFirstMessageExchange() const -> uint64_t
{
    return this->m_localPrivateNum * this->m_PrimeNumberLimiter;
}

auto Client::sendMessageToInternalClient(std::size_t clientId) -> void
{
    auto encodedMessage = RC4(this->m_messageBuffer).generateKeyChain(this->m_secretKeyChain).encode();

    clientManager.at(clientId)->recieveMessage(encodedMessage);
}

auto Client::sendMessageOverNetwork() -> void
{
    auto encodedMessage = RC4(this->m_messageBuffer).generateKeyChain(this->m_secretKeyChain).encode();

    auto body = std::string(encodedMessage.begin(), encodedMessage.end());

    this->internalNetworkClient->Post("/recieve", body, "text/plain");
}

auto Client::recieveMessage(const std::vector<uint8_t> &message) -> void
{
    auto encodedMessage = RC4(message).generateKeyChain(this->m_secretKeyChain).encode();

    this->m_recievedMessages.push(encodedMessage);
}

auto Client::writeMessage(const std::vector<uint8_t> &message) -> void
{
    this->m_messageBuffer = message;
}

auto Client::consumeMessage() -> std::vector<uint8_t>
{
    std::vector<uint8_t> m = this->m_recievedMessages.front();
    this->m_recievedMessages.pop();
    return m;
}

auto Client::connect(std::string_view remoteHost, uint32_t remotePort, std::string_view localHost, uint32_t localPort) -> void
{
    this->remoteHost = remoteHost;
    this->remotePort = remotePort;

    this->internalHost = localHost;
    this->internalPort = localPort;

    this->internalNetworkClient = std::make_unique<httplib::Client>(this->remoteHost, this->remotePort);

    auto m = this->internalNetworkClient->Get("/handshake")->body;

    nlohmann::json body = {
        {"message", this->getFirstMessageExchange()},
        {"host", localHost},
        {"port", localPort}
    };

    this->internalNetworkClient->Post("/handshake", body.dump(), "text/plain");

    this->recieveMessageExchange(std::stoull(m));
}

auto Client::isThereUnreadMessages() -> bool
{
    return !this->m_recievedMessages.empty();
}

auto Client::listen(std::string_view host, uint32_t port) -> void
{
    this->internalPort = port;
    this->internalHost = host;
    this->internalNetworkServer.listen(this->internalHost, this->internalPort);
}

auto makeHandShakeBetween2InternalClients(std::size_t firstClientId, std::size_t secondClientId) -> bool
{
    auto c1 = clientManager.at(firstClientId);
    auto c2 = clientManager.at(secondClientId);

    auto m1 = c1->getFirstMessageExchange();
    auto m2 = c2->getFirstMessageExchange();

    return c1->recieveMessageExchange(m2) == c2->recieveMessageExchange(m1);
}
