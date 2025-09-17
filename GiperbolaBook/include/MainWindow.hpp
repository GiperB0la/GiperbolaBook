#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <filesystem>
#include "Widgets.hpp"
#include "ChatEntity.hpp"
#include <NetLib.hpp>

#ifdef _DEBUG
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-audio-d.lib")
#else
#pragma comment(lib, "sfml-graphics.lib")
#pragma comment(lib, "sfml-window.lib")
#pragma comment(lib, "sfml-system.lib")
#pragma comment(lib, "sfml-audio.lib")
#endif

enum class CategoryButton
{
    Close,
    RollUp,
    Send,
    CloseChat
};

class MainWindow
{
public:
    MainWindow();

public:
    bool init(std::shared_ptr<NetLib>& net, std::string& nickname);
    void run();
    
private:
    void processEvents();
    void processEventsButtons(const sf::Event& event);
    void processEventsChats(const sf::Event& event);
    void render();
    void draggingWindow(const sf::Event& event);
    void sendMessage();
    void handlePacket(const Packet& packet);
    void processNetEvents();

private:
    std::unique_ptr<sf::RenderWindow> window_;
    sf::Texture windowT_;
    sf::Sprite windowS_;
    sf::Font font_;
    sf::Text user_nick_;
    sf::Cursor arrowCursor_, handCursor_;

    const std::string main_path_;
    bool dragging_window_ = false;
    sf::Vector2i dragOffset_;

    std::vector<Button> buttons_;
    std::unique_ptr<InputField> input_field_;

    std::shared_ptr<NetLib> net_;
    std::vector<std::shared_ptr<ChatEntity>> chats_;
    std::shared_ptr<ChatEntity> current_chat_;

    std::queue<Packet> events_;
};