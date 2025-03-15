#include "../include/Server.h"


std::string wstring_to_utf8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

std::wstring utf8_to_wstring(const std::string& str)
{
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}


Server::Server(const std::string& ip, int port)
    : folderFiles("Giperbola Files\\")
{
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        throw std::runtime_error(RED "[-] Socket creation error" RESET);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (InetPtonA(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error(RED "[-] Invalid IP address format" RESET);
    }

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error(RED "[-] Socket binding error" RESET);
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error(RED "[-] Socket listening error" RESET);
    }

    std::cout << GREEN "[+] Server is listening on " << ip << ":" << port << "..." RESET << std::endl;
}

Server::~Server()
{
    closesocket(serverSocket);
    WSACleanup();

    std::lock_guard<std::mutex> lock(clientMutex);
    clients.clear();
}

void Server::start()
{
    std::thread commandThread(&Server::handleCommands, this);
    commandThread.detach();

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << RED "[-] Error accepting client connection" RESET << std::endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        std::cout << GREEN "[+] New client connected: " << clientIP << ":" << clientPort << RESET << std::endl;

        {
            std::lock_guard<std::mutex> lock(clientMutex);
            clients.push_back(std::make_unique<Client>(clientIP, clientPort, clientSocket));
        }

        std::thread clientThread(&Server::handleClient, this, clientSocket, clientIP, clientPort);
        clientThread.detach();
    }
}

void Server::handleClient(SOCKET clientSocket, std::string ip, int port)
{
    send(clientSocket, "Hello from server!", 18, 0);

    char buffer[256];
    std::string clientName = "Unknown";

    std::cout << GREEN << "[+] New client connected: " << ip << ":" << port << RESET << std::endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            std::wcout << RED << "[-] Client " << utf8_to_wstring(ip) << ":" << port << " (" << utf8_to_wstring(clientName) << ") disconnected" << RESET << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        std::string message = buffer;

        std::wcout << CYAN << "[" << utf8_to_wstring(ip) << ":" << port << " - " << utf8_to_wstring(clientName) << "]: " << utf8_to_wstring(message) << RESET << std::endl;

        if (message.rfind("[Name] <", 0) == 0) {
            size_t start = message.find('<');
            size_t end = message.find('>');

            if (start != std::string::npos && end != std::string::npos && start < end) {
                std::string clientNewName = message.substr(start + 1, end - start - 1);

                {
                    std::lock_guard<std::mutex> lock(clientMutex);
                    for (auto& client : clients) {
                        if (client->ip == ip && client->port == port) {
                            client->name = clientNewName;
                            clientName = clientNewName;
                            std::wcout << YELLOW << "[+] Client " << utf8_to_wstring(ip) << ":" << port << " set name: " << utf8_to_wstring(clientName) << RESET << std::endl;
                            break;
                        }
                    }
                    broadcastClientsList();
                }
            }
        }

        else if (message.rfind("[Message] {", 0) == 0) {
            size_t start = message.find('{');
            size_t sep = message.find(" - ", start);
            size_t end = message.find('}', sep);
            size_t msgStart = message.find('<', end);
            size_t msgEnd = message.find('>', msgStart);

            if (start != std::string::npos && sep != std::string::npos &&
                end != std::string::npos && msgStart != std::string::npos &&
                msgEnd != std::string::npos && start < sep && sep < end && end < msgStart && msgStart < msgEnd) {

                std::string ipPort = message.substr(start + 1, sep - start - 1);
                size_t colon = ipPort.find(':');
                std::string targetIp = ipPort.substr(0, colon);
                int targetPort = std::stoi(ipPort.substr(colon + 1));

                std::string targetName = message.substr(sep + 3, end - sep - 3);
                std::string clientMessage = message.substr(msgStart + 1, msgEnd - msgStart - 1);

                {
                    std::lock_guard<std::mutex> lock(clientMutex);
                    for (const auto& client : clients) {
                        if (client->ip == targetIp && client->port == targetPort) {
                            std::string send_message = "[Message] {" + ip + ":" + std::to_string(port)
                                + " - " + clientName + "} " + "<" + clientMessage + ">";
                            send(client->socket, send_message.c_str(), send_message.length(), 0);
                            break;
                        }
                    }
                }
            }
        }

        else if (message.rfind("[File]", 0) == 0) {
            size_t start = message.find('{');
            size_t sep = message.find(" - ", start);
            size_t end = message.find('}', sep);
            size_t msgStart = message.find('<', end);
            size_t msgEnd = message.find('>', msgStart);
            size_t fileNameStart = message.find("(", msgEnd);
            size_t fileNameEnd = message.find(")", fileNameStart);

            if (start != std::string::npos && sep != std::string::npos &&
                end != std::string::npos && msgStart != std::string::npos &&
                msgEnd != std::string::npos && start < sep && sep < end && end < msgStart && msgStart < msgEnd) {

                std::string ipPort = message.substr(start + 1, sep - start - 1);
                size_t colon = ipPort.find(':');
                std::string targetIp = ipPort.substr(0, colon);
                int targetPort = std::stoi(ipPort.substr(colon + 1));

                std::string targetName = message.substr(sep + 3, end - sep - 3);
                std::string fileSizeStr = message.substr(msgStart + 1, msgEnd - msgStart - 1);
                size_t fileSize = std::stoull(fileSizeStr);

                std::string fileName = message.substr(fileNameStart + 1, fileNameEnd - fileNameStart - 1);

                std::string folderPath = folderFiles + targetIp + std::to_string(targetPort);
                if (!std::filesystem::exists(folderPath)) {
                    std::error_code ec;
                    if (!std::filesystem::create_directories(folderPath, ec)) {
                        std::cerr << RED << "[-] Folder creation error: " << ec.message() << RESET << std::endl;
                    }
                }

                std::string savePath = folderPath + "\\" + fileName;
                std::ofstream file(savePath, std::ios::binary);
                if (!file) {
                    std::cerr << RED << "[-] Error: couldn't create file " << savePath << RESET << std::endl;
                    return;
                }

                char buffer[BUFFER_SIZE];
                size_t bytesReceived = 0;
                int received;

                while (bytesReceived < fileSize && (received = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
                    file.write(buffer, received);
                    bytesReceived += received;
                }

                file.close();
                std::cout << "[+] File saved to " << savePath << " (" << bytesReceived << " bytes)" << std::endl;

                {
                    std::lock_guard<std::mutex> lock(clientMutex);
                    for (const auto& client : clients) {
                        if (client->ip == targetIp && client->port == targetPort) {
                            send(client->socket, message.c_str(), message.size(), 0);

                            std::ifstream sendFile(savePath, std::ios::binary);
                            while (sendFile.read(buffer, sizeof(buffer)) || sendFile.gcount() > 0) {
                                send(client->socket, buffer, sendFile.gcount(), 0);
                            }
                            sendFile.close();
                            std::cout << "[+] File successfully relayed to client" << std::endl;
                            break;
                        }
                    }
                }
            }
        }

        else {
            send(clientSocket, "[+] The message has been delivered", 35, 0);
        }
    }

    {
        std::lock_guard<std::mutex> lock(clientMutex);
        clients.erase(std::remove_if(clients.begin(), clients.end(),
            [&](const std::unique_ptr<Client>& c) {
                if (c->ip == ip && c->port == port) {
                    closesocket(c->socket);
                    return true;
                }
                return false;
            }),
            clients.end());
    }

    std::cout << MAGENTA << "[!] Client " << ip << ":" << port << " removed from the list" << RESET << std::endl;

    broadcastClientsList();
}

void Server::broadcastClientsList()
{
    std::wstring clientListW = L"[Clients]";
    for (const auto& client : clients) {
        std::wstring name = client->name.empty() ? L"Unknown" : utf8_to_wstring(client->name);
        clientListW += L" [" + utf8_to_wstring(client->ip) + L":" + std::to_wstring(client->port) + L" - " + name + L"]";
    }
    std::wcout << BLUE "[*] Broadcasting clients list: " << clientListW << RESET << std::endl;

    std::string clientList = "[Clients]";
    for (const auto& client : clients) {
        std::string name = client->name.empty() ? "Unknown" : client->name;
        clientList += " [" + client->ip + ":" + std::to_string(client->port) + " - " + name + "]";
    }
    for (const auto& client : clients) {
        send(client->socket, clientList.c_str(), clientList.size(), 0);
    }
}

void Server::handleCommands()
{
    std::string command;
    while (true) {
        std::getline(std::cin, command);

        if (command == "list" || command == "LIST") {
            std::lock_guard<std::mutex> lock(clientMutex);
            if (clients.empty()) {
                std::cout << YELLOW << "[Server] No connected clients." << RESET << std::endl;
            }
            else {
                std::cout << GREEN << "[Server] Connected clients:" << RESET << std::endl;
                for (const auto& client : clients) {
                    std::string name = client->name.empty() ? "Unknown" : client->name;
                    std::wcout << YELLOW << "  - " << utf8_to_wstring(client->ip) << ":" << client->port << " (" << utf8_to_wstring(name) << ")" << RESET << std::endl;
                }
            }
        }
        else if (command == "exit" || command == "EXIT") {
            std::cout << RED << "[Server] Shutting down..." << RESET << std::endl;
            exit(0);
        }
        else {
            std::cout << YELLOW << "[Server] Unknown command: " << command << RESET << std::endl;
        }
    }
}