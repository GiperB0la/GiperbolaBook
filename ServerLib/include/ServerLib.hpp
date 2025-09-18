#pragma once

#ifdef SERVERLIB_EXPORTS
#define SERVER_LIB __declspec(dllexport)
#else
#define SERVER_LIB __declspec(dllimport)
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")

namespace Color 
{
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";

    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* WHITE = "\033[37m";

    constexpr const char* BRIGHT_RED = "\033[91m";
    constexpr const char* BRIGHT_GREEN = "\033[92m";
    constexpr const char* BRIGHT_YELLOW = "\033[93m";
    constexpr const char* BRIGHT_BLUE = "\033[94m";
    constexpr const char* BRIGHT_MAGENTA = "\033[95m";
    constexpr const char* BRIGHT_CYAN = "\033[96m";
    constexpr const char* BRIGHT_WHITE = "\033[97m";
}


enum class OperationType
{
    Accept,
    Recv,
    Send
};

struct OverlappedEx
{
    OVERLAPPED overlapped{};
    OperationType type;
    WSABUF wsaBuf;
    char buffer[1024];

    OverlappedEx(OperationType type) : type(type) {
        ZeroMemory(&overlapped, sizeof(overlapped));
        wsaBuf.buf = buffer;
        wsaBuf.len = sizeof(buffer);
    }
};

class ServerLib;
class SERVER_LIB ClientSession
{
public:
    explicit ClientSession(SOCKET socket, const std::string& ip, uint16_t port, ServerLib& server);

public:
    SOCKET getSocket() const;
    void postRecv();
    void handleRecv(OverlappedEx* ov, DWORD bytes);
    void send(const Packet& packet);
    std::string getIP() const;
    uint16_t getPort() const;
    std::string getNickName() const;

private:
    std::string ip_;
    uint16_t port_;
    SOCKET socket_;
    std::string nickname_ = "";
    ServerLib& server_;
};

class SERVER_LIB ServerLib
{
    friend ClientSession;
public:
    ServerLib(uint16_t port);

public:
    void shutdown();

public:
    bool init();
    void run();

private:
    void acceptLoop();
    void workerLoop();

private:
    std::atomic<bool> running_{ true };
    std::string ip_;
    uint16_t port_;
    SOCKET listen_sock_;
    HANDLE iocp_;
    std::vector<std::thread> workers_;
    std::thread accept_thread_;
    std::vector<std::shared_ptr<ClientSession>> sessions_;
    std::mutex sessions_mutex_;
};