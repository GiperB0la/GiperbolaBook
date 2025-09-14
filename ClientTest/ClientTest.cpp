#include <gtest/gtest.h>
#include <NetLib.hpp>
#include <thread>
#include <chrono>


void startEchoServer(uint16_t port) 
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenSock, (sockaddr*)&addr, sizeof(addr));
    listen(listenSock, 1);

    SOCKET clientSock = accept(listenSock, nullptr, nullptr);

    char buffer[1024];
    int r = recv(clientSock, buffer, sizeof(buffer), 0);
    if (r > 0) {
        std::string reply = std::string("Echo: ") + std::string(buffer, r);
        send(clientSock, reply.c_str(), (int)reply.size(), 0);
    }

    closesocket(clientSock);
    closesocket(listenSock);
    WSACleanup();
}

TEST(NetLibTest, EchoServerInteraction) 
{
    constexpr uint16_t testPort = 7777;

    std::thread serverThread(startEchoServer, testPort);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    NetLib client("127.0.0.1", testPort);
    ASSERT_TRUE(client.connectToServer());

    ASSERT_TRUE(client.isReadyToWrite(1000));

    ASSERT_TRUE(client.sendMessage("hello"));

    std::string reply = client.receiveMessage();
    EXPECT_EQ(reply, "Echo: hello");

    serverThread.join();
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}