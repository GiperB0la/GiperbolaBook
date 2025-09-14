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

#pragma comment(lib, "ws2_32.lib")

class NET_LIB NetLib
{
public:
    NetLib(const std::string& ip, uint16_t port);
    ~NetLib();

    bool connectToServer();
    bool isReadyToWrite(int timeoutMs = 1000);
    bool isReadyToRead(int timeoutMs = 1000);
    bool sendMessage(const std::string& msg);
    std::string receiveMessage();

private:
    std::string ip_;
    uint16_t port_;
    SOCKET sock_;
};