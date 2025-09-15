#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <filesystem>
#include "Widgets.hpp"

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
    RollUp
};

class ClientUI
{
public:
    ClientUI();

public:
    bool init();
    void run();

private:
    void processEvents();
    void processEventsButtons(const sf::Event& event);
    void render();
    void draggingWindow(const sf::Event& event);

private:
    std::unique_ptr<sf::RenderWindow> window_;
    sf::Texture windowT_;
    sf::Sprite windowS_;

    const std::string main_path_;
    bool dragging_window_ = false;
    sf::Vector2i dragOffset_;

    std::vector<Button> buttons_;
};