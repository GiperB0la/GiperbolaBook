#include <ServerLib.hpp>


int main(int argc, char* argv[])
{
    std::string ip = "127.0.0.1";
    uint16_t port = 8888;

    if (argc >= 2) {
        ip = argv[1];
    }

    if (argc >= 3) {
        try {
            port = static_cast<uint16_t>(std::stoi(argv[2]));
        }
        catch (...) {
            std::cerr << "Invalid port, using default: " << port << std::endl;
        }
    }

    ServerLib server(ip, port);
    if (!server.init()) {
        std::cerr << "Server init failed" << std::endl;
        return 1;
    }

    std::cout << "Server started on " << ip << ":" << port << std::endl;
    server.run();

    return 0;
}