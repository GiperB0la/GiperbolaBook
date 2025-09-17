#include "../include/NetLib.hpp"


NetLib::NetLib(const std::string& ip_server, uint16_t port_server)
    : ip_server_(ip_server), port_server_(port_server), sock_(INVALID_SOCKET) {
}

NetLib::~NetLib()
{
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
    }
    WSACleanup();
}

bool NetLib::init(const std::string& nickname)
{
    nickname_ = nickname;

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
    addr.sin_port = htons(port_server_);
    inet_pton(AF_INET, ip_server_.c_str(), &addr.sin_addr);

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

    sockaddr_in localAddr{};
    int addrLen = sizeof(localAddr);
    if (getsockname(sock_, reinterpret_cast<sockaddr*>(&localAddr), &addrLen) == 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &localAddr.sin_addr, ipStr, sizeof(ipStr));
        ip_ = ipStr;
        port_ = ntohs(localAddr.sin_port);

        std::cout << "Local client address: " << ip_ << ":" << port_ << std::endl;
    }
    else {
        std::cerr << "getsockname() failed with error " << WSAGetLastError() << std::endl;
    }

    if (!sendNickname()) {
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

bool NetLib::sendMessage(const Packet& packet)
{
    if (!isReadyToWrite()) return false;

    auto buffer = packet.serialize();
    std::string send_buffer = std::string(buffer.begin(), buffer.end());
    int res = send(sock_, send_buffer.c_str(), (int)send_buffer.size(), 0);
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
            Packet packet = Packet::deserialize(buffer, res);

            if (packet.type_ == PacketType::CurrentUsers) {
                std::string data = packet.asString();
                std::istringstream iss(data);
                std::string line;

                while (std::getline(iss, line)) {
                    if (line.empty()) continue;

                    size_t p1 = line.find(':');
                    size_t p2 = line.find(':', p1 + 1);

                    if (p1 == std::string::npos || p2 == std::string::npos) {
                        std::cerr << "Bad CurrentUsers format: " << line << std::endl;
                        continue;
                    }

                    Client c;
                    c.ip_ = line.substr(0, p1);
                    c.port_ = static_cast<uint16_t>(std::stoi(line.substr(p1 + 1, p2 - (p1 + 1))));
                    c.nickname_ = line.substr(p2 + 1);

                    clients_.push_back(c);

                    std::cout << ">>> Current user: " << c.ip_ << ":" << c.port_ << " (" << c.nickname_ << ")" << std::endl;
                }

                {
                    std::lock_guard<std::mutex> lock(events_mtx_);
                    events_.push(packet);
                }
            }

            else if (packet.type_ == PacketType::UserJoined) {
                std::string data = packet.asString();

                size_t p1 = data.find(':');
                size_t p2 = data.find(':', p1 + 1);

                if (p1 == std::string::npos || p2 == std::string::npos) {
                    std::cerr << "Bad UserJoined format: " << data << std::endl;
                    return;
                }

                Client c;
                c.ip_ = data.substr(0, p1);
                c.port_ = static_cast<uint16_t>(std::stoi(data.substr(p1 + 1, p2 - (p1 + 1))));
                c.nickname_ = data.substr(p2 + 1);

                clients_.push_back(c);

                {
                    std::lock_guard<std::mutex> lock(events_mtx_);
                    events_.push(packet);
                }

                std::cout << ">>> User joined: " << c.ip_ << ":" << c.port_ << " (" << c.nickname_ << ")" << std::endl;
            }

            else if (packet.type_ == PacketType::UserLeft) {
                std::string data = packet.asString();

                size_t p1 = data.find(':');
                size_t p2 = data.find(':', p1 + 1);

                if (p1 == std::string::npos || p2 == std::string::npos) {
                    std::cerr << "Bad UserLeft format: " << data << std::endl;
                    return;
                }

                std::string ip = data.substr(0, p1);
                uint16_t port = static_cast<uint16_t>(std::stoi(data.substr(p1 + 1, p2 - (p1 + 1))));
                std::string nick = data.substr(p2 + 1);

                clients_.erase(
                    std::remove_if(clients_.begin(), clients_.end(),
                        [&](const Client& cl) {
                            return cl.ip_ == ip && cl.port_ == port && cl.nickname_ == nick;
                        }),
                    clients_.end()
                );

                {
                    std::lock_guard<std::mutex> lock(events_mtx_);
                    events_.push(packet);
                }

                std::cout << ">>> User left: " << ip << ":" << port << " (" << nick << ")" << std::endl;
            }

            else if (packet.type_ == PacketType::Message) {
                std::lock_guard<std::mutex> lock(events_mtx_);
                events_.push(packet);
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

std::string NetLib::getIP() const
{
    return ip_;
}

uint16_t NetLib::getPort() const
{
    return port_;
}

std::string NetLib::getNickName() const
{
    return nickname_;
}

bool NetLib::sendNickname()
{
    Packet packet = Packet::make(nickname_, PacketType::SetNickname);

    if (!sendMessage(packet)) {
        std::cerr << "Failed to send nickname" << std::endl;
        return false;
    }

    return true;
}

std::mutex& NetLib::getMutexEvents()
{
    return events_mtx_;
}

std::queue<Packet>& NetLib::getEvents()
{
    return events_;
}