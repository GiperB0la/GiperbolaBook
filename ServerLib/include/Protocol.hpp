#pragma once
#include <winsock2.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

enum class MessageType : uint8_t
{
    TextMessage = 0x01, // обычное сообщение
    SetNickname = 0x02, // клиент сообщает свой ник
    UserJoined = 0x03, // сервер уведомляет: кто-то вошёл
    UserLeft = 0x04, // сервер уведомляет: кто-то вышел
    Ping = 0x05, // пинг
    Pong = 0x06  // понг
};

struct Message
{
    uint32_t length;
    MessageType type;
    std::vector<char> payload;

    std::vector<char> serialize() const {
        std::vector<char> buffer;
        buffer.resize(sizeof(length) + sizeof(type) + payload.size());

        uint32_t netLength = htonl(length);
        memcpy(buffer.data(), &netLength, sizeof(netLength));
        buffer[sizeof(netLength)] = static_cast<uint8_t>(type);
        memcpy(buffer.data() + sizeof(netLength) + sizeof(type), payload.data(), payload.size());

        return buffer;
    }

    static Message deserialize(const char* data, size_t size) {
        if (size < sizeof(uint32_t) + sizeof(uint8_t)) {
            throw std::runtime_error("Invalid message size");
        }

        Message msg;
        uint32_t netLength;
        memcpy(&netLength, data, sizeof(netLength));
        msg.length = ntohl(netLength);

        msg.type = static_cast<MessageType>(*(uint8_t*)(data + sizeof(netLength)));

        if (msg.length > 0) {
            msg.payload.resize(msg.length);
            memcpy(msg.payload.data(), data + sizeof(netLength) + sizeof(uint8_t), msg.length);
        }

        return msg;
    }

    static Message makeMessage(const std::string& text, MessageType type = MessageType::TextMessage) {
        Message msg;
        msg.length = static_cast<uint32_t>(text.size());
        msg.type = type;
        msg.payload.assign(text.begin(), text.end());
        return msg;
    }

    std::string asString() const {
        return std::string(payload.begin(), payload.end());
    }
};