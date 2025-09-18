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
        try {
            Packet p = Packet::deserialize(buffer, r);
            std::string data = p.asString();

            Packet reply = Packet::make("Echo: " + data, PacketType::Message);
            auto buf = reply.serialize();
            send(clientSock, buf.data(), (int)buf.size(), 0);
        }
        catch (const std::exception& ex) {
            std::cerr << "Server failed to parse Packet: " << ex.what() << std::endl;
        }
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

    NetLib client("127.0.0.1", testPort, 0);
    ASSERT_TRUE(client.init("TestUser"));

    Packet testPacket = Packet::make("Hello", PacketType::Message);
    ASSERT_TRUE(client.sendPacket(testPacket));

    ASSERT_TRUE(client.isReadyToRead(2000)) << "Timeout waiting for reply";

    char buffer[1024];
    int res = recv(client.getSocket(), buffer, sizeof(buffer), 0);
    ASSERT_GT(res, 0) << "recv() failed with error: " << WSAGetLastError();

    Packet reply = Packet::deserialize(buffer, res);
    EXPECT_EQ(reply.asString(), "Echo: TestUser");

    serverThread.join();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}