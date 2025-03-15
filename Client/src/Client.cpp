#include "../include/Client.h"
#include "../include/GuiEditor.h"

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <fstream>


std::wstring getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);

    struct tm timeInfo;
    localtime_s(&timeInfo, &in_time_t);

    std::wstringstream ss;
    ss << std::put_time(&timeInfo, L"%H:%M");
    return ss.str();
}

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

SOCKET clientSocket;

Client::Client(const std::string& ip, int port)
    : running(false), serverIp(ip), serverPort(port), currentChooseUser(nullptr)
{
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverAddr = new sockaddr_in();
}

Client::~Client()
{
    stop();
}

void Client::stop()
{
    running = false;
    // delete serverAddr;

    /*if (clientSocket != INVALID_SOCKET) {
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }*/

    WSACleanup();

    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

bool Client::connectToServer()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "[-] Socket creation error" << std::endl;
        return false;
    }

    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(serverPort);

    if (InetPtonA(AF_INET, serverIp.c_str(), &serverAddr->sin_addr) <= 0) {
        closesocket(clientSocket);
        std::cerr << RED << "[-] Invalid IP address format" << RESET << std::endl;
        return false;
    }

    if (connect(clientSocket, (sockaddr*)serverAddr, sizeof(*serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[-] Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        return false;
    }

    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    if (getsockname(clientSocket, (sockaddr*)&clientAddr, &addrLen) == 0) {
        char clientIpBuffer[INET_ADDRSTRLEN];
        InetNtopA(AF_INET, &clientAddr.sin_addr, clientIpBuffer, INET_ADDRSTRLEN);
        clientIp = clientIpBuffer;
        clientPort = ntohs(clientAddr.sin_port);
    }
    else {
        std::cerr << RED << "[-] Failed to get client address: " << WSAGetLastError() << RESET << std::endl;
    }

    sendName();

    std::cout << GREEN << "[+] Connected to server" << RESET << std::endl;
    return true;
}

void Client::start()
{
    while (!running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (running) {
        if (!connectToServer()) {
            std::cout << RED << "[-] Connection failed. Retrying in 3 seconds..." << RESET << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }

        receiveThread = std::thread(&Client::receiveMessage, this);

        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void Client::sendMessage(const std::string& message)
{
    if (message == "") {
        return;
    }
    if (currentChooseUser == nullptr) {
        std::cout << RED << "[Send Error] The recipient is not specified" << RESET << std::endl;
        return;
    }
    else if (currentChooseUser->ip == "" || currentChooseUser->port == 0 || currentChooseUser->name == "") {
        std::cout << RED << "[Send Error] Error in recipient's data" << RESET << std::endl;
        return;
    }

    std::string send_message = "[Message] {" + currentChooseUser->ip + ":" + std::to_string(currentChooseUser->port) 
        + " - " + currentChooseUser->name + "} " + "<" + message + ">";

    int bytesSent = send(clientSocket, send_message.c_str(), send_message.size(), 0);

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "[-] Error sending message" << std::endl;
    }

    guiEditor->addMessage(currentChooseUser->ip, currentChooseUser->port, true, message, getCurrentTime());
}

void Client::sendFile(std::unordered_set<std::string>& paths, std::optional<std::string> path_one)
{
    if (paths.empty() && !path_one.has_value()) {
        return;
    }
    if (currentChooseUser == nullptr) {
        std::cout << RED << "[Send Error] The recipient is not specified" << RESET << std::endl;
        return;
    }
    else if (currentChooseUser->ip == "" || currentChooseUser->port == 0 || currentChooseUser->name == "") {
        std::cout << RED << "[Send Error] Error in recipient's data" << RESET << std::endl;
        return;
    }

    if (path_one.has_value()) {
        blockFileSend(path_one.value());
        return;
    }

    for (auto& path : paths) {
        blockFileSend(path);
    }
}

void Client::blockFileSend(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << RED << "[-] Error: couldn't open the file " << path << RESET << std::endl;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize == 0) {
        std::cerr << "[-] Error: File " << path << " is empty." << std::endl;
        file.close();
    }

    std::filesystem::path filePath(path);
    std::string fileName = filePath.filename().string();

    std::string sizeMessage = "[File] {" + currentChooseUser->ip + ":" + std::to_string(currentChooseUser->port)
        + " - " + currentChooseUser->name + "} <" + std::to_string(fileSize) + "> (" + fileName + ")";
    send(clientSocket, sizeMessage.c_str(), sizeMessage.size(), 0);

    char buffer[BUFFER_SIZE];
    size_t totalSent = 0;

    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        send(clientSocket, buffer, file.gcount(), 0);
        totalSent += file.gcount();
    }

    std::cout << "[+] Successfully sent " << totalSent << " bytes from file: " << path << std::endl;

    guiEditor->addMessage(currentChooseUser->ip, currentChooseUser->port, true, fileName, getCurrentTime(), true);
}

void Client::receiveMessage()
{
    char buffer[256] = { 0 };

    while (running) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string message = buffer;
            // std::cout << "[Server]: " << message << std::endl;

            if (message.rfind("[Clients]", 0) == 0) {
                updateClientList(message);
            }

            else if (message.rfind("[Message]", 0) == 0) {
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

                    std::string senderName = message.substr(sep + 3, end - sep - 3);

                    std::string receivedMessage = message.substr(msgStart + 1, msgEnd - msgStart - 1);

                    std::cout << CYAN << "[Private] {" << targetIp << ":" << targetPort
                        << " - " << senderName << "} <" << receivedMessage << ">" << RESET << std::endl;

                    guiEditor->addMessage(targetIp, targetPort, false, receivedMessage, getCurrentTime());
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

                    if (!std::filesystem::exists(Config::MainPath.string() + "\\" + Config::MainFolderPath)) {
                        std::filesystem::create_directory(Config::MainPath.string() + "\\" + Config::MainFolderPath);
                    }

                    std::string savePath = Config::MainPath.string() + "\\" + Config::MainFolderPath + fileName;
                    std::ofstream file(savePath, std::ios::binary);
                    if (!file) {
                        std::cerr << "[-] Error: couldn't create file " << savePath << std::endl;
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

                    guiEditor->addMessage(targetIp, targetPort, false, fileName, getCurrentTime(), true);
                }
            }
        }
        else if (bytesReceived == 0) {
            std::cout << RED << "[-] Server closed the connection" << RESET << std::endl;
            running = false;
            break;
        }
        else {
            int error = WSAGetLastError();
            if (error != WSAETIMEDOUT) {
                std::cerr << "[-] Connection error: " << error << ". Reconnecting..." << std::endl;
                running = false;
                break;
            }
        }
    }
}

void Client::setGuiEditor(GuiEditor* guiEditor)
{
    if (guiEditor != nullptr) {
        this->guiEditor = guiEditor;
    }
}

void Client::sendName()
{
    std::string send_name = "[Name] <" + name + ">";
    int bytesSent = send(clientSocket, send_name.c_str(), send_name.size(), 0);

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "[-] Error sending message" << std::endl;
    }
}

void Client::updateClientList(const std::string& clientListMessage)
{
    // std::lock_guard<std::mutex> lock(clientListMutex);

    std::istringstream stream(clientListMessage);
    std::string token;

    std::getline(stream, token, ']');

    std::vector<ClientEntry> newClients;

    while (std::getline(stream, token, '[')) {
        if (token.empty()) continue;
        size_t pos1 = token.find(":");
        size_t pos2 = token.find(" - ");
        size_t pos3 = token.find("]");

        if (pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos)
            continue;

        std::string ip = token.substr(0, pos1);
        int port = std::stoi(token.substr(pos1 + 1, pos2 - pos1 - 1));
        std::string name = token.substr(pos2 + 3, pos3 - pos2 - 3);

        newClients.push_back({ name, ip, port });
    }

    for (auto it = clients.begin(); it != clients.end();) {
        auto found = std::find_if(newClients.begin(), newClients.end(), [&](const ClientEntry& newClient) {
            return newClient.ip == it->ip && newClient.port == it->port;
            });

        if (found == newClients.end()) {
            it = clients.erase(it);
        }
        else {
            ++it;
        }
    }

    for (const auto& newClient : newClients) {
        auto found = std::find_if(clients.begin(), clients.end(), [&](const ClientEntry& existingClient) {
            return existingClient.ip == newClient.ip && existingClient.port == newClient.port;
            });

        if (found == clients.end()) {
            clients.push_back(newClient);
        }
    }

    updateUsers = true;
    std::cout << GREEN << "[*] Updated client list:" << RESET << std::endl;
    for (const auto& client : clients) {
        std::wcout << YELLOW << " - [" << utf8_to_wstring(client.ip) << ":" << client.port << " - " << utf8_to_wstring(client.name) << "]" << RESET << std::endl;
    }
}

void Client::chooseUser(const std::string name, const std::string ip, int port)
{
    for (auto& client : clients) {
        if (name == client.name && ip == client.ip && port == client.port) {
            currentChooseUser = &client;
        }
    }
    std::wcout << YELLOW << "[Choose] {" << utf8_to_wstring(currentChooseUser->ip) << ":" << currentChooseUser->port 
        << " - " << utf8_to_wstring(currentChooseUser->name) << "}" << RESET << std::endl;
}