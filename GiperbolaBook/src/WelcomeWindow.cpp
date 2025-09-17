#include "../include/WelcomeWindow.hpp"
#include <windows.h>


WelcomeWindow::WelcomeWindow()
	: main_path_(std::filesystem::current_path().string())
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    window_ = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(400, 300),
        "GiperbolaBook",
        sf::Style::None,
        settings
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

bool WelcomeWindow::init()
{
    windowT_.loadFromFile(main_path_ + "/Graphics/Textures/Background/WelcomeWindow.png");
    windowS_.setTexture(windowT_);

    font_.loadFromFile(main_path_ + "/Graphics/Fonts/Inter/Inter-Regular.otf");

    input_field_ = std::make_unique<InputField>(sf::Vector2f(17.f, 257.f), sf::Vector2f(317.f, 24.f),
        font_, 20, sf::Color(49, 51, 56), "Write your name...");

    send_ = std::make_unique<Button>(0,
        sf::Vector2f(354.f, 259.f), 
        main_path_ + "/Graphics/Textures/Buttons/Send/Send.png", 
        main_path_ + "/Graphics/Textures/Buttons/Send/Send.png", 
        main_path_ + "/Graphics/Textures/Buttons/Send/Send.png");

    arrowCursor_.loadFromSystem(sf::Cursor::Arrow);
    handCursor_.loadFromSystem(sf::Cursor::Hand);

    return true;
}

std::string WelcomeWindow::run()
{
    while (window_->isOpen() && !go_) {
        processEvents();
        render();
    }

    return nickname_;
}

void WelcomeWindow::processEvents()
{
    sf::Event event{};
    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            window_->close();
        }

        draggingWindow(event);

        if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)) {
            readyNickname();
        }

        send_->handle_event(event, *window_);
        if (send_->is_clicked()) {
            readyNickname();
        }

        input_field_->handle_event(event, *window_);
    }
}

void WelcomeWindow::render()
{
    window_->clear(sf::Color(30, 30, 30));

    window_->draw(windowS_);
    input_field_->draw(*window_);
    send_->draw(*window_);

    window_->display();
}

void WelcomeWindow::draggingWindow(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition();

        if (event.mouseButton.y >= 0 && event.mouseButton.y <= 248) {
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

void WelcomeWindow::readyNickname()
{
    std::string data = input_field_->get_value();
    if (!data.empty()) {
        nickname_ = data;
        go_ = true;
    }
}