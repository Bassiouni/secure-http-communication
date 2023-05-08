#include "httplib.h"
#include "client.h"

#include <iostream>
#include <ctime>
#include <future>
#include <string>

int main(int argc, char **argv)
{
    const auto internalHost = argv[1];
    const auto internalPort = std::stoi(argv[2]);

    srand((unsigned)time(nullptr));

    Client client(100'000'000'000'000ULL + (rand() % 999'999'999'999'999'999ULL), 11);

    auto f = std::async(std::launch::async, &Client::listen, &client, argv[1], std::stoi(argv[2]));

    while (true)
    {
        std::string inputBuffer;
        std::cout << "command> ";
        std::getline(std::cin >> std::ws, inputBuffer);
        if (inputBuffer == "connect")
        {
            std::string remoteHost;
            uint32_t remotePort;

            std::cout << "Host: ";
            std::cin >> remoteHost;

            std::cout << "Port: ";
            std::cin >> remotePort;

            client.connect(remoteHost, remotePort, internalHost, internalPort);
        }
        else if (inputBuffer == "send message")
        {
            std::string msg;
            std::cout << "Message: ";
            std::getline(std::cin >> std::ws, msg);

            client.writeMessage(std::vector<uint8_t>(msg.begin(), msg.end()));
            client.sendMessageOverNetwork();
        }
        else if (inputBuffer == "show messages")
        {
            if (client.isThereUnreadMessages())
            {
                std::cout << "Unread Messages: " << std::endl;
            }

            while (client.isThereUnreadMessages())
            {
                auto m = client.consumeMessage();
                std::cout << "- " << std::string(m.begin(), m.end()) << std::endl;
            }

            std::cout << "All caught up." << std::endl;
        }
        else if (inputBuffer == "exit")
        {
            exit(0);
        }
        else
        {
            std::cerr << "Error: Unknown Command!" << std::endl;
        }
    }
}
