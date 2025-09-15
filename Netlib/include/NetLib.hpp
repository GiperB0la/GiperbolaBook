#pragma once

#ifdef NETLIB_EXPORTS
#define NET_LIB __declspec(dllexport)
#else
#define NET_LIB __declspec(dllimport)
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")

struct Client
{
    std::string ip_;
    uint16_t port_;
    std::string nickname_;
};

class NET_LIB NetLib
{
public:
    NetLib(const std::string& ip, uint16_t port, const std::string& nickname_);
    ~NetLib();

public:
    bool init();
    void run();

private:
    bool connectToServer();
    bool isReadyToWrite(int timeoutMs = 1000);
    bool isReadyToRead(int timeoutMs = 1000);
    bool sendMessage(const std::string& msg);
    void receiveMessage();

private:
    std::string ip_;
    uint16_t port_;
    SOCKET sock_;
    std::string nickname_;
    std::vector<Client> clients_;
};