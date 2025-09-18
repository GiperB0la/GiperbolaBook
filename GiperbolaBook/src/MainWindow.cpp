#include "../include/MainWindow.hpp"
#include <windows.h>


MainWindow::MainWindow()
    : main_path_(std::filesystem::current_path().string()) {
}

bool MainWindow::init(std::shared_ptr<NetLib>& net, std::string& nickname)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    window_ = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(1200, 800),
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

    windowT_.loadFromFile(main_path_ + "/Graphics/Textures/Background/MainWindow.png");
    windowS_.setTexture(windowT_);

    font_.loadFromFile(main_path_ + "/Graphics/Fonts/Inter/Inter-Regular.otf");

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
    buttons_.emplace_back(
        static_cast<size_t>(CategoryButton::Send),
        sf::Vector2f(1133.f, 739.f),
        main_path_ + "/Graphics/Textures/Buttons/Send/Send.png",
        main_path_ + "/Graphics/Textures/Buttons/Send/Send.png",
        main_path_ + "/Graphics/Textures/Buttons/Send/Send.png"
    );
    buttons_.emplace_back(
        static_cast<size_t>(CategoryButton::CloseChat),
        sf::Vector2f(1139.f, 86.f),
        main_path_ + "/Graphics/Textures/Buttons/CloseChat/CloseChat1.png",
        main_path_ + "/Graphics/Textures/Buttons/CloseChat/CloseChat2.png",
        main_path_ + "/Graphics/Textures/Buttons/CloseChat/CloseChat3.png"
    );

    for (auto& button : buttons_) {
        button.apply_texture_for_state();
    }

    user_nick_.setFont(font_);
    user_nick_.setFillColor(sf::Color::White);
    user_nick_.setCharacterSize(20);
    user_nick_.setString("");
    user_nick_.setPosition(sf::Vector2f(284.f, 84.f));

    net_ = net;

    input_field_ = std::make_unique<InputField>(sf::Vector2f(286.f, 737.f), sf::Vector2f(827.f, 34.f),
        font_, 20, sf::Color(49, 51, 56), "Write a message...");

    arrowCursor_.loadFromSystem(sf::Cursor::Arrow);
    handCursor_.loadFromSystem(sf::Cursor::Hand);

    return true;
}

void MainWindow::run()
{
    while (window_->isOpen()) {
        processNetEvents();
        processEvents();
        render();
    }
}

void MainWindow::processEvents()
{
    sf::Event event{};
    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            window_->close();
        }

        draggingWindow(event);
        processEventsButtons(event);
        processEventsChats(event);

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            sendMessage();
        }

        input_field_->handle_event(event, *window_);

        if (current_chat_) {
            current_chat_->handleEvent(event, *window_);
        }
    }
}

void MainWindow::processEventsButtons(const sf::Event& event)
{
    for (auto& button : buttons_) {
        if (button.get_category() == static_cast<size_t>(CategoryButton::Send)) {
            if (button.is_hovered()) {
                window_->setMouseCursor(handCursor_);
            }
            else {
                window_->setMouseCursor(arrowCursor_);
            }
        }
    }

    for (auto& button : buttons_) {
        if ((button.get_category() == static_cast<size_t>(CategoryButton::Send) || 
            button.get_category() == static_cast<size_t>(CategoryButton::CloseChat)) && 
            !current_chat_) {
            return;
        }

        button.handle_event(event, *window_);
        if (button.is_clicked()) {
            if (button.get_category() == static_cast<size_t>(CategoryButton::Close)) {
                window_->close();
            }
            else if (button.get_category() == static_cast<size_t>(CategoryButton::RollUp)) {
                ShowWindow(window_->getSystemHandle(), SW_MINIMIZE);
            }
            else if (button.get_category() == static_cast<size_t>(CategoryButton::Send)) {
                sendMessage();
            }
            else if (button.get_category() == static_cast<size_t>(CategoryButton::CloseChat)) {
                current_chat_->getUserField().deselect();
                current_chat_ = nullptr;
                button.set_state(State::Normal);
            }
        }
    }
}

void MainWindow::processEventsChats(const sf::Event& event)
{
    for (auto& chat : chats_) {
        chat->getUserField().handle_event(event, *window_);
        if (chat->getUserField().is_selected_once()) {
            if (current_chat_) {
                current_chat_->getUserField().deselect();
            }

            current_chat_ = chat;
            user_nick_.setString("Messages from " + (current_chat_->getNickname()));
        }
    }
}

void MainWindow::render()
{
    window_->clear(sf::Color(30, 30, 30));

    window_->draw(windowS_);

    for (auto& button : buttons_) {
        if ((button.get_category() == static_cast<size_t>(CategoryButton::Send) || button.get_category() == static_cast<size_t>(CategoryButton::CloseChat)) && 
            !current_chat_) {
            continue;
        }
        button.draw(*window_);
    }

    for (auto& chat : chats_) {
        chat->draw(*window_, current_chat_);
    }

    if (current_chat_) {
        if (!user_nick_.getString().isEmpty()) {
            window_->draw(user_nick_);
        }
        input_field_->draw(*window_);
    }

    window_->display();
}

void MainWindow::draggingWindow(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition();

        if (event.mouseButton.y >= 0 && event.mouseButton.y <= 71) {
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

void MainWindow::sendMessage()
{
    if (!current_chat_) {
        input_field_->clear();
        return;
    }

    std::string msg = input_field_->get_value();
    if (msg.empty()) {
        return;
    }

    input_field_->clear();

    Message message = Message::make(
        net_->getIP(),
        net_->getPort(),
        current_chat_->getIP(),
        current_chat_->getPort(),
        msg
    );

    Packet packet = Packet::make(message.serialize(), PacketType::Message);

    if (!net_->sendPacket(packet)) {
        std::cerr << "Failed to send message" << std::endl;
    }

    current_chat_->addMessage(msg, font_);
}

void MainWindow::handlePacket(const Packet& packet)
{
    if (packet.type_ == PacketType::CurrentUsers) {
        std::string data = packet.asString();
        std::istringstream iss(data);
        std::string line;

        while (std::getline(iss, line)) {
            if (line.empty()) continue;

            size_t p1 = line.find(':');
            size_t p2 = line.find(':', p1 + 1);

            if (p1 == std::string::npos || p2 == std::string::npos) {
                std::cerr << "Bad CurrentUsers format: " << line << std::endl;
                continue;
            }

            std::string ip = line.substr(0, p1);
            uint16_t port = static_cast<uint16_t>(std::stoi(line.substr(p1 + 1, p2 - (p1 + 1))));
            std::string nick = line.substr(p2 + 1);

            bool is_self = (net_->getIP() == ip && net_->getPort() == port);

            auto it = std::find_if(chats_.begin(), chats_.end(),
                [&](const std::shared_ptr<ChatEntity>& chat) {
                    return chat->getIP() == ip && chat->getPort() == port;
                });

            if (it == chats_.end()) {
                chats_.emplace_back(std::make_shared<ChatEntity>(
                    ip, port, nick, main_path_, font_, is_self
                ));
            }
        }
    }

    else if (packet.type_ == PacketType::UserJoined) {
        std::string data = packet.asString();

        size_t p1 = data.find(':');
        size_t p2 = data.find(':', p1 + 1);

        if (p1 == std::string::npos || p2 == std::string::npos) {
            std::cerr << "Bad UserJoined format: " << data << std::endl;
            return;
        }

        std::string ip = data.substr(0, p1);
        uint16_t port = static_cast<uint16_t>(std::stoi(data.substr(p1 + 1, p2 - (p1 + 1))));
        std::string nick = data.substr(p2 + 1);

        auto it = std::find_if(chats_.begin(), chats_.end(),
            [&](const std::shared_ptr<ChatEntity>& chat) {
                return chat->getIP() == ip && chat->getPort() == port;
            });

        if (it == chats_.end()) {
            chats_.emplace_back(std::make_shared<ChatEntity>(
                ip, port, nick, main_path_, font_
            ));
        }
    }

    else if (packet.type_ == PacketType::UserLeft) {
        std::string data = packet.asString();

        size_t p1 = data.find(':');
        size_t p2 = data.find(':', p1 + 1);

        if (p1 == std::string::npos || p2 == std::string::npos) {
            std::cerr << "Bad UserLeft format: " << data << std::endl;
            return;
        }

        std::string ip = data.substr(0, p1);
        uint16_t port = static_cast<uint16_t>(std::stoi(data.substr(p1 + 1, p2 - (p1 + 1))));

        chats_.erase(
            std::remove_if(chats_.begin(), chats_.end(),
                [&](const std::shared_ptr<ChatEntity>& chat) {
                    return chat->getIP() == ip && chat->getPort() == port;
                }),
            chats_.end()
        );
    }

    else if (packet.type_ == PacketType::Message) {
        Message msg = Message::deserialize(packet.payload_.data(), packet.payload_.size());

        for (auto& chat : chats_) {
            if (chat->getIP() == msg.ip_from && chat->getPort() == msg.port_from) {
                chat->addMessage(msg.message_, font_, true);
            }
        }
    }
}

void MainWindow::processNetEvents()
{
    {
        std::lock_guard<std::mutex> lock(net_->getMutexEvents());
        std::swap(events_, net_->getEvents());
    }

    while (!events_.empty()) {
        auto packet = events_.front();
        events_.pop();

        handlePacket(packet);
    }
}