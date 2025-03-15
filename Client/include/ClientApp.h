#pragma once
#include "GuiEditor.h"
#include <thread>


class ClientApp 
{
public:
    ClientApp(const std::string& ip, unsigned short port);
    ~ClientApp();

public:
    void start();

private:
    Client client;
    GuiEditor guiEditor;
    std::thread clientThread;
};