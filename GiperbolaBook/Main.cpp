#include "include/ClientUI.hpp"
#include <NetLib.hpp>
#include <iostream>
#include <thread>


int main()
{
    NetLib client("127.0.0.1", 8888, "Egor");
    if (!client.init()) {
        std::cerr << "Client init failed" << std::endl;
        return 1;
    }

    ClientUI client_ui;
    if (!client_ui.init()) {
        std::cerr << "Client UI init failed" << std::endl;
        return 1;
    }

    std::thread thread_net(&NetLib::run, &client);

    client_ui.run();

    thread_net.detach();

    return 0;
}