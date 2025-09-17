#include "../include/ServerLib.hpp"


ClientSession::ClientSession(SOCKET socket, const std::string& ip, uint16_t port, ServerLib& server)
    : socket_(socket), ip_(ip), port_(port), server_(server) {
}

SOCKET ClientSession::getSocket() const
{
    return socket_;
}

void ClientSession::postRecv()
{
    auto* ov = new OverlappedEx(OperationType::Recv);
    DWORD flags = 0, bytes = 0;
    int res = WSARecv(socket_, &ov->wsaBuf, 1, &bytes, &flags, &ov->overlapped, nullptr);
    if (res == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "WSARecv error" << std::endl;
        delete ov;
    }
}

void ClientSession::handleRecv(OverlappedEx* ov, DWORD bytes)
{
    try {
        Packet packet = Packet::deserialize(ov->buffer, bytes);

        if (packet.type_ == PacketType::SetNickname) {
            nickname_ = packet.asString();
            std::cout << "Client [" << ip_ << ":" << port_ << "] set nickname: " << nickname_ << std::endl;
            {
                std::lock_guard<std::mutex> lock(server_.sessions_mutex_);
                std::ostringstream oss;
                for (auto& s : server_.sessions_) {
                    oss << s->getIP() << ":" << s->getPort() << ":" << s->getNickName() << "\n";
                }
                Packet usersListMsg = Packet::make(oss.str(), PacketType::CurrentUsers);
                send(usersListMsg);

                Packet joinedMsg = Packet::make(
                    getIP() + ":" + std::to_string(getPort()) + ":" + getNickName(),
                    PacketType::UserJoined
                );
                for (auto& s : server_.sessions_) {
                    if (s.get() != this) {
                        s->send(joinedMsg);
                    }
                }
            }
        }

        else if (packet.type_ == PacketType::Message) {
            try {
                Message msg = Message::deserialize(packet.payload_.data(), packet.payload_.size());

                std::cout
                    << "[" << msg.ip_from << ":" << msg.port_from << "] "
                    << " -> [" << msg.ip_to << ":" << msg.port_to << "] "
                    << msg.message_
                    << std::endl;

                // Собираем пакет для пересылки
                Packet forwardPacket = Packet::make(msg.serialize(), PacketType::Message);

                // Ищем получателя
                std::shared_ptr<ClientSession> target;
                {
                    std::lock_guard<std::mutex> lock(server_.sessions_mutex_);
                    for (auto& s : server_.sessions_) {
                        if (s->getIP() == msg.ip_to && s->getPort() == msg.port_to) {
                            target = s;
                            break;
                        }
                    }
                }

                if (target) {
                    target->send(forwardPacket);
                }
                else {
                    std::cerr << "Target client not found: "
                        << msg.ip_to << ":" << msg.port_to << std::endl;
                }
            }
            catch (const std::exception& ex) {
                std::cerr << "Failed to parse Message: " << ex.what() << std::endl;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Failed to parse message: " << ex.what() << std::endl;
    }

    delete ov;
    postRecv();
}

void ClientSession::send(const Packet& packet)
{
    auto buffer = packet.serialize();

    auto* ov = new OverlappedEx(OperationType::Send);
    memcpy(ov->buffer, buffer.data(), buffer.size());
    ov->wsaBuf.len = static_cast<ULONG>(buffer.size());

    DWORD bytes = 0;
    int res = WSASend(socket_, &ov->wsaBuf, 1, &bytes, 0, &ov->overlapped, nullptr);
    if (res == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "WSASend error" << std::endl;
        delete ov;
    }
}

std::string ClientSession::getIP() const
{
    return ip_;
}

uint16_t ClientSession::getPort() const
{
    return port_;
}

std::string ClientSession::getNickName() const
{
    return nickname_;
}

ServerLib::ServerLib(std::string ip, uint16_t port)
    : ip_(ip), port_(port) {
}

void ServerLib::shutdown()
{
    running_ = false;
    closesocket(listen_sock_);

    for (size_t i = 0; i < workers_.size(); i++) {
        PostQueuedCompletionStatus(iocp_, 0, 0, nullptr);
    }
}

bool ServerLib::init()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;

    listen_sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock_ == INVALID_SOCKET) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip_.c_str(), &addr.sin_addr);
    addr.sin_port = htons(port_);

    if (bind(listen_sock_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) return false;
    if (listen(listen_sock_, SOMAXCONN) == SOCKET_ERROR) return false;

    iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (!iocp_) return false;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int nThreads = sysInfo.dwNumberOfProcessors * 2;
    for (int i = 0; i < nThreads; i++) {
        workers_.emplace_back(&ServerLib::workerLoop, this);
    }

    accept_thread_ = std::thread(&ServerLib::acceptLoop, this);

    return true;
}

void ServerLib::run()
{
    std::cout << "Server running on " << ip_ << ":" << port_ << std::endl;
    accept_thread_.join();
    for (auto& t : workers_) {
        t.join();
    }
}

void ServerLib::acceptLoop()
{
    while (running_) {
        sockaddr_in clientAddr;
        int len = sizeof(clientAddr);
        SOCKET clientSock = accept(listen_sock_, (sockaddr*)&clientAddr, &len);

        if (clientSock == INVALID_SOCKET) {
            if (!running_) break;
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));
        uint16_t port = ntohs(clientAddr.sin_port);

        std::cout << "Client [" << ip << ":" << port << "] connected" << std::endl;

        auto session = std::make_shared<ClientSession>(clientSock, ip, port, *this);
        CreateIoCompletionPort((HANDLE)clientSock, iocp_, (ULONG_PTR)session.get(), 0);
        session->postRecv();

        {
            std::lock_guard<std::mutex> lock(sessions_mutex_);
            sessions_.push_back(session);
        }
    }
}

void ServerLib::workerLoop()
{
    DWORD bytes;
    ULONG_PTR key;
    LPOVERLAPPED overlapped;

    while (true) {
        BOOL res = GetQueuedCompletionStatus(iocp_, &bytes, &key, &overlapped, INFINITE);

        if (key == 0 && overlapped == nullptr && bytes == 0) {
            break;
        }

        auto* session = reinterpret_cast<ClientSession*>(key);
        auto* ov = reinterpret_cast<OverlappedEx*>(overlapped);

        if (!res || bytes == 0) {
            if (session) {
                std::cerr << "Client [" << session->getIP() << ":" << session->getPort() << "] disconnect" << std::endl;
                closesocket(session->getSocket());
            }
            if (ov) delete ov;

            {
                std::lock_guard<std::mutex> lock(sessions_mutex_);
                sessions_.erase(std::remove_if(sessions_.begin(), sessions_.end(),
                    [&](const std::shared_ptr<ClientSession>& s) { return s.get() == session; }),
                    sessions_.end());
            }
            continue;
        }

        if (ov->type == OperationType::Recv) {
            session->handleRecv(ov, bytes);
        }
        else if (ov->type == OperationType::Send) {
            delete ov;
        }
    }
}