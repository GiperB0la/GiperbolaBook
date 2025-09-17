#include "../include/ChatEntity.hpp"


ChatEntity::ChatEntity(const std::string& ip, uint16_t port, const std::string& nickname,
    const std::string& main_path, const sf::Font& font, bool self)
    : ip_(ip),
    port_(port),
    nickname_(nickname),
    view_(sf::FloatRect(0.f, 0.f, 908.f, 606.f)),
    user_field_(
        counter_++,
        sf::Vector2f(40.f, 142.f + counter_ * (42.f + 10.f)),
        main_path + "/Graphics/Textures/Buttons/User/User1.png",
        main_path + "/Graphics/Textures/Buttons/User/User2.png",
        main_path + "/Graphics/Textures/Buttons/User/User3.png"),
    scrollbar_track_(sf::Vector2f(6.f, 576.f), 3),
    scrollbar_slider_(sf::Vector2f(0.f, 0.f), 3),
    down_(
        1,
        sf::Vector2f(1133.f, 678.f),
        main_path + "/Graphics/Textures/Buttons/Down/Down1.png",
        main_path + "/Graphics/Textures/Buttons/Down/Down2.png",
        main_path + "/Graphics/Textures/Buttons/Down/Down3.png"
    )
{
    nickname_text_.setFont(font);
    nickname_text_.setCharacterSize(20);
    nickname_text_.setFillColor(sf::Color::White);
    nickname_text_.setPosition(
        user_field_.bounds().left + 16.f,
        user_field_.bounds().top + 9.f
    );

    auto shorten = [](const std::string& s, std::size_t max_len) {
        if (s.size() <= max_len) return s;
        return s.substr(0, max_len) + "...";
        };

    if (self) {
        std::string nick_to_show = shorten(nickname_, 10);
        nickname_text_.setString(nick_to_show + " (you)");
    }
    else {
        std::string nick_to_show = shorten(nickname_, 14);
        nickname_text_.setString(nick_to_show);
    }

    view_.setViewport(sf::FloatRect(
        271.f / 1200.f,
        122.f / 800.f,
        908.f / 1200.f,
        606.f / 800.f
    ));

    scrollbar_track_.setPosition(sf::Vector2f(1168.f, 132.f));
    scrollbar_track_.setFillColor(sf::Color(49, 51, 56));

    scrollbar_slider_.setPosition(sf::Vector2f(1168.f, 132.f));
    scrollbar_slider_.setFillColor(sf::Color(75, 85, 99));
}

void ChatEntity::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    float mouseX = 0.f, mouseY = 0.f;
    if (event.type == sf::Event::MouseMoved) {
        mouseX = static_cast<float>(event.mouseMove.x);
        mouseY = static_cast<float>(event.mouseMove.y);
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        mouseX = static_cast<float>(event.mouseButton.x);
        mouseY = static_cast<float>(event.mouseButton.y);
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
        float step = event.mouseWheelScroll.delta * 30.f;
        float contentHeight = getContentHeight();
        float viewHeight = view_.getSize().y;

        if (contentHeight <= viewHeight) {
            scroll_offset_ = 0.f;
        }
        else {
            scroll_offset_ += step;
            float maxOffset = std::max(0.f, contentHeight - viewHeight) + 20.f;
            if (scroll_offset_ < 0.f) scroll_offset_ = 0.f;
            if (scroll_offset_ > maxOffset) scroll_offset_ = maxOffset;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (scrollbar_slider_.getGlobalBounds().contains(mouseX, mouseY)) {
            dragging_ = true;
            dragOffsetY_ = mouseY - scrollbar_slider_.getPosition().y;
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        dragging_ = false;
    }

    if (event.type == sf::Event::MouseMoved && dragging_) {
        float newY = mouseY - dragOffsetY_;
        handleScrollbarDrag(newY);
    }

    down_.handle_event(event, window);
    if (down_.is_clicked()) {
        scroll_offset_ = 0.f;
    }
}

void ChatEntity::handleScrollbarDrag(float mouseY)
{
    float contentHeight = getContentHeight();
    float viewHeight = view_.getSize().y;
    float trackHeight = scrollbar_track_.getSize().y;
    float trackTop = scrollbar_track_.getPosition().y;
    float sliderHeight = scrollbar_slider_.getSize().y;

    if (contentHeight <= viewHeight) {
        scroll_offset_ = 0.f;
        return;
    }

    if (mouseY < trackTop) mouseY = trackTop;
    if (mouseY > trackTop + trackHeight - sliderHeight)
        mouseY = trackTop + trackHeight - sliderHeight;

    float progress = (mouseY - trackTop) / (trackHeight - sliderHeight);

    scroll_offset_ = (1.f - progress) * (contentHeight - viewHeight);
}

void ChatEntity::updateScrollbar()
{
    float contentHeight = getContentHeight();
    float viewHeight = view_.getSize().y;
    float trackHeight = scrollbar_track_.getSize().y;
    float trackTop = scrollbar_track_.getPosition().y;

    if (contentHeight <= viewHeight) {
        scrollbar_slider_.setSize({ 6.f, trackHeight });
        scrollbar_slider_.setPosition(scrollbar_track_.getPosition());
        return;
    }

    float sliderHeight = viewHeight / contentHeight * trackHeight;
    scrollbar_slider_.setSize({ 6.f, sliderHeight });

    float scrollProgress = scroll_offset_ / (contentHeight - viewHeight);
    scrollProgress = 1.f - scrollProgress;

    float sliderY = trackTop + scrollProgress * (trackHeight - sliderHeight);

    if (sliderY < trackTop) sliderY = trackTop;
    if (sliderY > trackTop + trackHeight - sliderHeight)
        sliderY = trackTop + trackHeight - sliderHeight;

    scrollbar_slider_.setPosition({ scrollbar_track_.getPosition().x, sliderY });
}

float ChatEntity::getContentHeight() const
{
    if (messages_.empty()) return 0.f;

    float total = 0.f;
    float gap = 8.f;

    for (auto& m : messages_) {
        sf::FloatRect bounds = m.text_.getLocalBounds();
        float h = bounds.height + 20.f;
        total += h + gap;
    }

    return total;
}

void ChatEntity::draw(sf::RenderWindow& window, std::shared_ptr<ChatEntity>& current_chat)
{
    user_field_.draw(window);
    window.draw(nickname_text_);

    if (this != current_chat.get()) {
        return;
    }

    window.setView(view_);
    drawMessages(window);
    window.setView(window.getDefaultView());

    float contentHeight = getContentHeight();
    float viewHeight = view_.getSize().y;

    if (contentHeight > viewHeight) {
        updateScrollbar();
        window.draw(scrollbar_track_);
        window.draw(scrollbar_slider_);

        if (scroll_offset_ > 0.f) {
            down_.draw(window);
        }
    }
}

void ChatEntity::drawMessages(sf::RenderWindow& window)
{
    float viewHeight = view_.getSize().y;
    float bottom = viewHeight - 20.f + scroll_offset_;
    float gap = 8.f;

    for (int i = static_cast<int>(messages_.size()) - 1; i >= 0; --i) {
        auto& m = messages_[i];

        sf::FloatRect bounds = m.text_.getLocalBounds();
        sf::Vector2f frameSize(bounds.width + 32.f, bounds.height + 20.f);

        float frameY = bottom - frameSize.y;
        m.frame_.setSize(frameSize);
        m.frame_.setPosition(20.f, frameY);

        m.text_.setPosition(
            m.frame_.getPosition().x + 16.f,
            m.frame_.getPosition().y + 10.f - bounds.top
        );

        window.draw(m.frame_);
        window.draw(m.text_);

        bottom = frameY - gap;
    }
}

void ChatEntity::addMessage(const std::string& msg, const sf::Font& font, bool other)
{
    MessageChat mc;
    mc.text_.setFont(font);
    mc.text_.setCharacterSize(16);
    mc.text_.setFillColor(sf::Color::White);
    mc.text_.setString(msg);

    sf::FloatRect bounds = mc.text_.getLocalBounds();
    sf::Vector2f frameSize(bounds.width + 32.f, bounds.height + 20.f);

    mc.frame_.setSize(frameSize);
    mc.frame_.setRadius(8.f);
    if (other) {
        mc.frame_.setFillColor(sf::Color(75, 85, 99));
    }
    else {
        mc.frame_.setFillColor(sf::Color(59, 130, 246));
    }

    mc.text_.setPosition(
        mc.frame_.getPosition().x + 16.f,
        mc.frame_.getPosition().y + 10.f - bounds.top
    );

    messages_.push_back(std::move(mc));
}

std::string ChatEntity::getIP() const
{
    return ip_;
}

uint16_t ChatEntity::getPort() const
{
    return port_;
}

std::string ChatEntity::getNickname() const
{
    return nickname_;
}

SelectButton& ChatEntity::getUserField()
{
    return user_field_;
}