#pragma once
#include <SFML/Graphics.hpp>
#include "Widgets.hpp"

class ChatEntity
{
public:
    ChatEntity(const std::string& ip, uint16_t port, const std::string& nickname,
        const std::string& main_path, const sf::Font& font, bool self = false);

public:
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window, std::shared_ptr<ChatEntity>& current_chat);
    void addMessage(const std::string& msg, const sf::Font& font, bool other = false);
    std::string getIP() const;
    uint16_t getPort() const;
    std::string getNickname() const;
    SelectButton& getUserField();

private:
    void drawMessages(sf::RenderWindow& window);
    void updateScrollbar();
    void handleScrollbarDrag(float mouseY);
    float getContentHeight() const;

private:
    struct MessageChat {
        sf::Text text_;
        RoundRectangle frame_;
    };

private:
    std::string ip_;
    uint16_t port_;
    std::string nickname_;
    SelectButton user_field_;
    inline static size_t counter_;
    sf::Text nickname_text_;
    sf::View view_;
    std::vector<MessageChat> messages_;
    float scroll_offset_ = 0.f;
    RoundRectangle scrollbar_track_;
    RoundRectangle scrollbar_slider_;
    bool dragging_ = false;
    float dragOffsetY_ = 0.f;
    Button down_;
};