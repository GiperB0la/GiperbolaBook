#pragma once
#include <winsock2.h>
#include <iostream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

enum class PacketType : uint8_t
{
    Message = 0x01, // обычное сообщение
    SetNickname = 0x02, // клиент сообщает свой ник
    CurrentUsers = 0x03, // все клиенты
    UserJoined = 0x04, // сервер уведомляет: кто-то вошёл
    UserLeft = 0x05, // сервер уведомляет: кто-то вышел
    Ping = 0x06, // пинг
    Pong = 0x07  // понг
};

struct Packet
{
    uint32_t length_;
    PacketType type_;
    std::vector<char> payload_;

    std::vector<char> serialize() const {
        std::vector<char> buffer;
        buffer.resize(sizeof(length_) + sizeof(type_) + payload_.size());

        uint32_t netLength = htonl(length_);
        memcpy(buffer.data(), &netLength, sizeof(netLength));
        buffer[sizeof(netLength)] = static_cast<uint8_t>(type_);
        memcpy(buffer.data() + sizeof(netLength) + sizeof(type_), payload_.data(), payload_.size());

        return buffer;
    }

    static Packet deserialize(const char* data, size_t size) {
        if (size < sizeof(uint32_t) + sizeof(uint8_t)) {
            throw std::runtime_error("Invalid Packet size");
        }

        Packet packet;
        uint32_t netLength;
        memcpy(&netLength, data, sizeof(netLength));
        packet.length_ = ntohl(netLength);

        packet.type_ = static_cast<PacketType>(*(uint8_t*)(data + sizeof(netLength)));

        if (packet.length_ > 0) {
            packet.payload_.resize(packet.length_);
            memcpy(packet.payload_.data(), data + sizeof(netLength) + sizeof(uint8_t), packet.length_);
        }

        return packet;
    }

    std::string asString() const {
        return std::string(payload_.begin(), payload_.end());
    }

    static Packet make(const std::string& payload, PacketType type) {
        Packet packet;
        packet.length_ = static_cast<uint32_t>(payload.size());
        packet.type_ = type;
        packet.payload_.assign(payload.begin(), payload.end());
        return packet;
    }

    static Packet make(const std::vector<char>& payload, PacketType type) {
        Packet packet;
        packet.length_ = static_cast<uint32_t>(payload.size());
        packet.type_ = type;
        packet.payload_ = payload;
        return packet;
    }
};

struct Message
{
    std::string ip_from;
    uint16_t port_from;
    std::string ip_to;
    uint16_t port_to;
    std::string message_;

    std::vector<char> serialize() const {
        std::vector<char> buffer;

        auto writeString = [&](const std::string& str) {
            uint32_t len = htonl(static_cast<uint32_t>(str.size()));
            buffer.insert(buffer.end(),
                reinterpret_cast<const char*>(&len),
                reinterpret_cast<const char*>(&len) + sizeof(len));
            buffer.insert(buffer.end(), str.begin(), str.end());
            };

        auto writeUint16 = [&](uint16_t value) {
            uint16_t netVal = htons(value);
            buffer.insert(buffer.end(),
                reinterpret_cast<const char*>(&netVal),
                reinterpret_cast<const char*>(&netVal) + sizeof(netVal));
            };

        writeString(ip_from);
        writeUint16(port_from);
        writeString(ip_to);
        writeUint16(port_to);
        writeString(message_);

        return buffer;
    }

    static Message deserialize(const char* data, size_t size) {
        Message msg;
        size_t offset = 0;

        auto readString = [&](std::string& out) {
            if (offset + sizeof(uint32_t) > size) throw std::runtime_error("Invalid Message");
            uint32_t len;
            memcpy(&len, data + offset, sizeof(len));
            offset += sizeof(len);
            len = ntohl(len);

            if (offset + len > size) throw std::runtime_error("Invalid Message payload");
            out.assign(data + offset, data + offset + len);
            offset += len;
            };

        auto readUint16 = [&]() -> uint16_t {
            if (offset + sizeof(uint16_t) > size) throw std::runtime_error("Invalid Message port");
            uint16_t netVal;
            memcpy(&netVal, data + offset, sizeof(netVal));
            offset += sizeof(netVal);
            return ntohs(netVal);
            };

        readString(msg.ip_from);
        msg.port_from = readUint16();
        readString(msg.ip_to);
        msg.port_to = readUint16();
        readString(msg.message_);

        return msg;
    }

    static Message make(const std::string& ip_from, uint16_t port_from,
        const std::string& ip_to, uint16_t port_to,
        const std::string& text) {
        Message m;
        m.ip_from = ip_from;
        m.port_from = port_from;
        m.ip_to = ip_to;
        m.port_to = port_to;
        m.message_ = text;
        return m;
    }
};