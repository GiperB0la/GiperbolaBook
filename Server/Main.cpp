#include <ServerLib.hpp>
#include <iostream>


int main()
{
    ServerLib server("127.0.0.1", 8888);
    if (!server.init()) {
        std::cerr << "Server init failed" << std::endl;
        return 1;
    }
    server.run();

    return 0;
}