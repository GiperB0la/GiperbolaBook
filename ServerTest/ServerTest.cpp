#include <gtest/gtest.h>
#include <ServerLib.hpp>
#include <thread>
#include <chrono>


class TestClient 
{
public:
    TestClient(const char* ip, uint16_t port) : ip_(ip), port_(port) {}

    bool connectToServer() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;

        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ == INVALID_SOCKET) return false;

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port_);
        inet_pton(AF_INET, ip_, &addr.sin_addr);

        if (connect(sock_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(sock_);
            return false;
        }
        return true;
    }

    bool sendMessage(const std::string& msg) {
        int res = send(sock_, msg.c_str(), (int)msg.size(), 0);
        return res != SOCKET_ERROR;
    }

    std::string receiveMessage() {
        char buffer[1024];
        int res = recv(sock_, buffer, sizeof(buffer), 0);
        if (res > 0) return std::string(buffer, res);
        return {};
    }

    void close() {
        closesocket(sock_);
        WSACleanup();
    }

private:
    const char* ip_;
    uint16_t port_;
    SOCKET sock_;
};

TEST(ServerLibTest, EchoMessage) 
{
    ServerLib server(5555);
    ASSERT_TRUE(server.init());

    std::thread serverThread([&]() {
        server.run();
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    TestClient client("127.0.0.1", 5555);
    ASSERT_TRUE(client.connectToServer());

    ASSERT_TRUE(client.sendMessage("hello"));

    std::string reply = client.receiveMessage();
    EXPECT_EQ(reply, "Echo: hello");

    client.close();

    server.shutdown();
    serverThread.join();
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}