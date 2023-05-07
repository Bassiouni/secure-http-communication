#include <iostream>
#include <random>

#include "client.h"

auto main() -> int32_t
{
    // Providing a seed value
    srand((unsigned)time(nullptr));

    Client client0(100'000'000'000'000ULL + (rand() % 999'999'999'999'999'999ULL), 11999);
    Client client1(100'000'000'000'000ULL + (rand() % 999'999'999'999'999'999ULL), 11999);

    if (!makeHandShakeBetween2Clients(0, 1))
    {
        return 1;
    }

    std::cout << "Connection established between Client(0) and Client(1)" << std::endl;

    std::string message = "LESGO";
    std::vector<uint8_t> msgVec(message.begin(), message.end());

    client0.writeMessage(msgVec);
    client0.sendMessage(1);

    auto m = client1.consumeMessage();

    std::cout << "Sent message is: " << std::string(m.begin(), m.end()) << std::endl;

    return 0;
}
