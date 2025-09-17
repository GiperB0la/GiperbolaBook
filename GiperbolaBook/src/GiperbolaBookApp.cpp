#include "../include/GiperbolaBookApp.hpp"


GiperbolaBookApp::GiperbolaBookApp(const std::string& ip, uint16_t port)
    : ip_(ip), port_(port), 
    net_(std::make_shared<NetLib>(ip, port)) {
}

void GiperbolaBookApp::run()
{
    {
        WelcomeWindow welcome_window;

        if (!welcome_window.init()) {
            std::cerr << "Welcome Window init failed" << std::endl;
            return;
        }

        nickname_ = welcome_window.run();
    }

    if (!main_window_.init(net_, nickname_)) {
        std::cerr << "Main Window init failed" << std::endl;
        return;
    }

    std::thread thread_net([this]() {
        if (!net_->init(nickname_)) {
            std::cerr << "Net init failed" << std::endl;
            return;
        }
        net_->run();
        });

    main_window_.run();

    thread_net.join();
}