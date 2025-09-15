#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <cctype>

// Widget

class Widget
{
public:
    virtual ~Widget() = default;
    virtual size_t get_category() const = 0;
    virtual bool set_category(const size_t category) = 0;
    virtual void handle_event(const sf::Event& event, const sf::RenderWindow& window) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::Vector2f get_position() const = 0;
    virtual bool set_position(const sf::Vector2f& position) = 0;
    virtual sf::FloatRect bounds() const = 0;
};


// Button

enum class State { Normal, Hovered, Pressed };

class Button : public Widget
{
public:
    Button(
        const size_t category,
        const sf::Vector2f& position,
        const std::string& texNormalPath,
        const std::string& texHoverPath,
        const std::string& texPressedPath
    );

public:
    size_t get_category() const override;
    bool set_category(const size_t category) override;
    virtual void handle_event(const sf::Event& event, const sf::RenderWindow& window) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f get_position() const override;
    bool set_position(const sf::Vector2f& position) override;
    sf::FloatRect bounds() const override;
    State get_state() const;
    bool set_state(State new_state);
    bool is_clicked();
    bool is_hovered() const;
    virtual void apply_texture_for_state();

protected:
    size_t category;
    sf::Sprite sprite_;
    sf::Texture texNormal_;
    sf::Texture texHover_;
    sf::Texture texPressed_;

    State state_{ State::Normal };
    bool pressedInside_{ false };
    bool clickedEdge_{ false };
};


// CheckButton

class CheckButton : public Button
{
public:
    CheckButton(
        const size_t category,
        const sf::Vector2f& position,
        const std::string& texNormalPath,
        const std::string& texHoverPath,
        const std::string& texPressedPath
    );

public:
    void handle_event(const sf::Event& event, const sf::RenderWindow& window) override;
    void apply_texture_for_state() override;
    bool is_checked() const noexcept;
    void set_checked(bool v);

protected:
    bool checked_{ false };
};