#include "include/GiperbolaBookApp.hpp"


int main(int argc, char* argv[])
{
    std::string ip = "127.0.0.1";
    uint16_t port = 8888;

    if (argc == 2) {
        try {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        }
        catch (...) {
            std::cerr << "Invalid port, using default: " << port << std::endl;
        }
    }

    GiperbolaBookApp app(ip, port);
    app.run();

    return 0;
}