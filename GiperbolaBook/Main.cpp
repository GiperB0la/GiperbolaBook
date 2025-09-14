#include <NetLib.hpp>


int main()
{
    NetLib client("127.0.0.1", 8888);
    if (!client.connectToServer()) return 1;

    if (!client.isReadyToWrite()) {
        std::cerr << "Timeout waiting for connect" << std::endl;
        return 1;
    }

    std::cout << "Connected!" << std::endl;

    while (true) {
        std::string msg;
        std::cout << "Enter message (or 'exit'): ";
        std::getline(std::cin, msg);

        if (msg == "exit") break;

        client.sendMessage(msg);

        std::string reply = client.receiveMessage();
        if (!reply.empty()) {
            std::cout << "Server replied: " << reply << std::endl;
        }
    }

    return 0;
}