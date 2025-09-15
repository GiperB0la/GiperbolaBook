#include "../include/ClientUI.hpp"
#include <windows.h>


ClientUI::ClientUI()
    : main_path_(std::filesystem::current_path().string())
{
    window_ = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(1200, 800),
        "GiperbolaBook",
        sf::Style::None
    );

    HWND hwnd = window_->getSystemHandle();
    RECT rect;
    GetClientRect(hwnd, &rect);
    HRGN region = CreateRoundRectRgn(
        rect.left, rect.top, rect.right, rect.bottom,
        30, 30
    );
    SetWindowRgn(hwnd, region, TRUE);
    DeleteObject(region);

    window_->setFramerateLimit(60);
}

bool ClientUI::init()
{
    windowT_.loadFromFile(main_path_ + "/Graphics/Textures/Background/Window.png");
    windowS_.setTexture(windowT_);

    buttons_.emplace_back(
        static_cast<size_t>(CategoryButton::Close),
        sf::Vector2f(1126.f, 16.f),
        main_path_ + "/Graphics/Textures/Buttons/Close/Close1.png",
        main_path_ + "/Graphics/Textures/Buttons/Close/Close2.png",
        main_path_ + "/Graphics/Textures/Buttons/Close/Close3.png"
    );
    buttons_.emplace_back(
        static_cast<size_t>(CategoryButton::RollUp),
        sf::Vector2f(1068.f, 16.f),
        main_path_ + "/Graphics/Textures/Buttons/RollUp/RollUp1.png",
        main_path_ + "/Graphics/Textures/Buttons/RollUp/RollUp2.png",
        main_path_ + "/Graphics/Textures/Buttons/RollUp/RollUp3.png"
    );

    for (auto& button : buttons_) {
        button.apply_texture_for_state();
    }

    return true;
}

void ClientUI::run() 
{
    while (window_->isOpen()) {
        processEvents();
        render();
    }
}

void ClientUI::processEvents()
{
    sf::Event event{};
    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            window_->close();
        }

        draggingWindow(event);
        processEventsButtons(event);
    }
}

void ClientUI::processEventsButtons(const sf::Event& event)
{
    for (auto& button : buttons_) {
        button.handle_event(event, *window_);
        if (button.is_clicked()) {
            if (button.get_category() == static_cast<size_t>(CategoryButton::Close)) {
                window_->close();
            }
            else if (button.get_category() == static_cast<size_t>(CategoryButton::RollUp)) {
                ShowWindow(window_->getSystemHandle(), SW_MINIMIZE);
            }
        }
    }
}

void ClientUI::render()
{
    window_->clear(sf::Color(30, 30, 30));

    window_->draw(windowS_);

    for (auto& button : buttons_) {
        button.draw(*window_);
    }

    window_->display();
}

void ClientUI::draggingWindow(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        for (auto& button : buttons_) {
            button.handle_event(event, *window_);
            if (button.get_state() == State::Hovered || button.get_state() == State::Pressed) {
                return;
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition();
        if (event.mouseButton.y + 71) {
            dragging_window_ = true;
            dragOffset_ = mousePos - window_->getPosition();
        }
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
        dragging_window_ = false;
    }

    if (event.type == sf::Event::MouseMoved && dragging_window_) {
        sf::Vector2i mousePos = sf::Mouse::getPosition();
        window_->setPosition(mousePos - dragOffset_);
    }
}