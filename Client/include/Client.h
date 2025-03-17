#pragma once
#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <unordered_set>

#pragma comment(lib, "ws2_32.lib")

#define RESET   "\033[0m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define CYAN    "\033[1;36m"
#define YELLOW  "\033[1;33m"
#define MAGENTA "\033[1;35m"
#define BLUE    "\033[1;34m"


std::wstring getCurrentTime();
std::string wstring_to_utf8(const std::wstring& wstr);
std::wstring utf8_to_wstring(const std::string& str);

#define BUFFER_SIZE 4096  // 4KB

struct ClientEntry
{
    std::string name;
    std::string ip;
    int port;
};

struct sockaddr_in;

class GuiEditor;

class Client
{
public:
    friend GuiEditor;

public:
    Client(const std::string& ip, int port);
    ~Client();

public:
    void start();
    void stop();
    void sendMessage(const std::string& message);
    void sendFile(std::unordered_set<std::string>& paths, std::optional<std::string> path_one = std::nullopt);
    void chooseUser(const std::string name, const std::string ip, int port);
    void setGuiEditor(GuiEditor* guiEditor);

private:
    bool connectToServer();
    void receiveMessage();
    void receiveFile();
    void updateClientList(const std::string& clientListMessage);
    void sendName();
    void blockFileSend(const std::string& path);

private:
    sockaddr_in* serverAddr = nullptr;
    std::atomic<bool> running;
    std::string serverIp, clientIp;
    int serverPort, clientPort;
    std::string name;
    std::thread receiveThread;
    std::vector<ClientEntry> clients;
    bool updateUsers = false;
    ClientEntry* currentChooseUser;
    GuiEditor* guiEditor = nullptr;
    bool noSignal = false;
};