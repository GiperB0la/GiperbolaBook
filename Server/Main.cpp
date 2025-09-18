#include <ServerLib.hpp>


int main(int argc, char* argv[])
{
    uint16_t port = 8888;

    if (argc >= 2) {
        try {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }
        catch (...) {
            std::cerr << "Invalid port, using default: " << port << std::endl;
        }
    }

    ServerLib server(port);
    if (!server.init()) {
        std::cerr << "Server init failed" << std::endl;
        return 1;
    }

    server.run();

    return 0;
}