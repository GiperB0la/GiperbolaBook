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
#include <sstream>
#include <mutex>
#include <queue>
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
    NetLib(const std::string& ip, uint16_t port);
    ~NetLib();

public:
    bool init(const std::string& nickname);
    void run();
    std::string getIP() const;
    uint16_t getPort() const;
    std::string getNickName() const;
    bool sendMessage(const Packet& packet);
    std::mutex& getMutexEvents();
    std::queue<Packet>& getEvents();

private:
    bool connectToServer();
    bool isReadyToWrite(int timeoutMs = 1000);
    bool isReadyToRead(int timeoutMs = 1000);
    void receiveMessage();
    bool sendNickname();

private:
    std::string ip_server_, ip_;
    uint16_t port_server_, port_;
    SOCKET sock_;
    std::string nickname_;
    std::vector<Client> clients_;
    std::mutex events_mtx_;
    std::queue<Packet> events_;
};