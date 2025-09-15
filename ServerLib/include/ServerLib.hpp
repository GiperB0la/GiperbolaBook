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
#include "Protocol.hpp"

#pragma comment(lib, "ws2_32.lib")

// --- Перечисление типов операций ---
enum class OperationType
{
    Accept,
    Recv,
    Send
};

// --- OVERLAPPED-расширение для IOCP ---
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

class SERVER_LIB ClientSession
{
public:
    explicit ClientSession(SOCKET socket, const std::string& ip, uint16_t port);

public:
    SOCKET getSocket() const;
    void postRecv();
    void handleRecv(OverlappedEx* ov, DWORD bytes);
    void send(const Message& msg);
    std::string getIP() const;
    uint16_t getPort() const;

private:
    std::string ip_;
    uint16_t port_;
    SOCKET socket_;
    std::string nickname_;
};

class SERVER_LIB ServerLib
{
public:
    ServerLib(std::string ip, uint16_t port);

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