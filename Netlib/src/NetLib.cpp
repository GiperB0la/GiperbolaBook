#include "../include/NetLib.hpp"


NetLib::NetLib(const std::string& ip, uint16_t port, const std::string& nickname_)
    : ip_(ip), port_(port), sock_(INVALID_SOCKET), nickname_(nickname_) {
}

NetLib::~NetLib()
{
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
    }
    WSACleanup();
}

bool NetLib::init()
{
    if (!connectToServer()) return false;

    if (!isReadyToWrite()) {
        std::cerr << "Timeout waiting for connect" << std::endl;
        return false;
    }

    return true;
}

void NetLib::run()
{
    while (true) {
        if (isReadyToRead()) {
            receiveMessage();
        }
    }
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

    if (!isReadyToWrite(2000)) {
        std::cerr << "Timeout waiting for connect" << std::endl;
        return false;
    }

    Message msg;
    msg.length = static_cast<uint32_t>(nickname_.size());
    msg.type = MessageType::SetNickname;
    msg.payload.assign(nickname_.begin(), nickname_.end());

    auto buffer = msg.serialize();
    if (!sendMessage(std::string(buffer.begin(), buffer.end()))) {
        std::cerr << "Failed to send nickname" << std::endl;
        return false;
    }

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

void NetLib::receiveMessage()
{
    if (!isReadyToRead()) return;

    char buffer[1024];
    int res = recv(sock_, buffer, sizeof(buffer), 0);
    if (res > 0) {
        try {
            Message msg = Message::deserialize(buffer, res);

            switch (msg.type) {
            case MessageType::UserJoined: {
                std::string data = msg.asString(); // например "127.0.0.1:12345"
                Client c;
                c.ip_ = data.substr(0, data.find(':'));
                c.port_ = static_cast<uint16_t>(std::stoi(data.substr(data.find(':') + 1)));
                c.nickname_ = ""; // пока пусто, позже можно передавать сразу ник
                clients_.push_back(c);
                std::cout << ">>> User joined: " << data << std::endl;
                break;
            }
            case MessageType::UserLeft: {
                std::string data = msg.asString();
                clients_.erase(
                    std::remove_if(clients_.begin(), clients_.end(),
                        [&](const Client& cl) { return cl.ip_ + ":" + std::to_string(cl.port_) == data; }),
                    clients_.end());
                std::cout << ">>> User left: " << data << std::endl;
                break;
            }
            case MessageType::TextMessage:
                std::cout << msg.asString() << std::endl;
                break;

            default:
                std::cerr << "Unknown message type: " << (int)msg.type << std::endl;
                break;
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Failed to parse message: " << ex.what() << std::endl;
        }
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
}