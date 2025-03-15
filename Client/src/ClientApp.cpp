#include "../include/ClientApp.h"


ClientApp::ClientApp(const std::string& ip, unsigned short port)
    : client(ip, port), guiEditor(&client) 
{}

ClientApp::~ClientApp()
{
    client.stop();
    if (clientThread.joinable()) {
        clientThread.join();
    }
}

void ClientApp::start()
{
    try {
        client.setGuiEditor(&guiEditor);
        clientThread = std::thread([this] { client.start(); });
        guiEditor.start();
    }
    catch (const std::exception& e) {
        std::cerr << "[-] Error: " << e.what() << std::endl;
    }
}