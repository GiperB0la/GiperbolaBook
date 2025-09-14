#include "../include/NetLib.hpp"


NetLib::NetLib(const std::string& ip, uint16_t port)
    : ip_(ip), port_(port), sock_(INVALID_SOCKET) {
}

NetLib::~NetLib()
{
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
    }
    WSACleanup();
}

bool NetLib::connectToServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ == INVALID_SOCKET) {
        std::cerr << "socket() failed" << std::endl;
        return false;
    }

    u_long mode = 1;
    ioctlsocket(sock_, FIONBIO, &mode);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);

    int res = connect(sock_, (sockaddr*)&addr, sizeof(addr));
    if (res == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) {
            std::cerr << "connect() failed with error " << err << std::endl;
            closesocket(sock_);
            return false;
        }
    }

    std::cout << "Connecting (non-blocking)..." << std::endl;
    return true;
}

bool NetLib::isReadyToWrite(int timeoutMs)
{
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(sock_, &writeSet);

    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int res = select(0, nullptr, &writeSet, nullptr, &tv);
    return (res > 0 && FD_ISSET(sock_, &writeSet));
}

bool NetLib::isReadyToRead(int timeoutMs)
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock_, &readSet);

    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    int res = select(0, &readSet, nullptr, nullptr, &tv);
    return (res > 0 && FD_ISSET(sock_, &readSet));
}

bool NetLib::sendMessage(const std::string& msg)
{
    if (!isReadyToWrite()) return false;

    int res = send(sock_, msg.c_str(), (int)msg.size(), 0);
    if (res == SOCKET_ERROR) {
        std::cerr << "send() failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

std::string NetLib::receiveMessage()
{
    if (!isReadyToRead()) return {};

    char buffer[1024];
    int res = recv(sock_, buffer, sizeof(buffer), 0);
    if (res > 0) {
        return std::string(buffer, res);
    }
    else if (res == 0) {
        std::cerr << "Server closed connection" << std::endl;
    }
    else {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cerr << "recv() failed: " << err << std::endl;
        }
    }
    return {};
}