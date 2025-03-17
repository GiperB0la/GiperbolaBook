#include "include/Server.h"


int main(int argc, char* argv[]) 
{
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    std::string ip = "127.0.0.1";
    int port = 5555;

    if (argc >= 2) {
        ip = argv[1];
    }
    if (argc >= 3) {
        try {
            port = std::stoi(argv[2]);
        }
        catch (...) {
            std::cerr << RED "[-] Invalid port number. Using default: " << port << RESET << std::endl;
        }
    }

    try {
        Server server(ip, port);
        server.start();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}