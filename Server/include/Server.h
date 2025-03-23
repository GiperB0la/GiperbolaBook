#pragma once
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <fstream>
#include <filesystem>

#pragma comment(lib, "ws2_32.lib")

#define RESET   "\033[0m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define CYAN    "\033[1;36m"
#define YELLOW  "\033[1;33m"
#define MAGENTA "\033[1;35m"
#define BLUE    "\033[1;34m"


std::string wstring_to_utf8(const std::wstring& wstr);
std::wstring utf8_to_wstring(const std::string& str);

#define BUFFER_SIZE 4096  // 4KB

struct Client
{
    std::string ip;
    int port;
    std::string name;
    SOCKET socket;

    Client(const std::string& ip, int port, SOCKET socket)
        : ip(ip), port(port), name("Unknown"), socket(socket) {
    }
};

class Server
{
public:
    Server(const std::string& ip, int port);
    ~Server();

public:
    void start();

private:
    void handleClient(SOCKET clientSocket, std::string ip, int port);
    void broadcastClientsList();
    void handleCommands();

private:
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::vector<std::unique_ptr<Client>> clients;
    std::mutex clientMutex;
    const std::string folderFiles;
};