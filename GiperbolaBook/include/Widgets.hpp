#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <array>
#include <string>
#include <sstream>
#include <cctype>

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


// SelectButton

class SelectButton : public Button
{
public:
    SelectButton(
        const size_t category,
        const sf::Vector2f& position,
        const std::string& texNormalPath,
        const std::string& texHoverPath,
        const std::string& texPressedPath
    );

public:
    void handle_event(const sf::Event& event, const sf::RenderWindow& window) override;
    void apply_texture_for_state() override;
    bool is_selected_once();
    bool is_selected() const noexcept;
    void deselect();

private:
    bool selected_{ false };
    bool just_selected_{ false };
};


// InputField

class InputField : public Widget
{
public:
    InputField(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Font& font, 
        unsigned int charSize, sf::Color color, const std::string& placeholder = "");

public:
    size_t get_category() const override;
    bool set_category(const size_t category) override;
    bool set_position(const sf::Vector2f& position) override;
    void handle_event(const sf::Event& event, const sf::RenderWindow& window) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f get_position() const override;
    sf::FloatRect bounds() const override;
    std::string get_value() const;
    bool clear();

private:
    void set_placeholder(const std::string& str);
    std::string getVisibleText();
    void adjustView();
    void updateCursor();
    float getCursorX();

private:
    sf::RectangleShape rect_;
    sf::Text text_;
    sf::Text placeholderText_;
    sf::RectangleShape cursor_;

    std::string input_;
    size_t cursorIndex_;
    size_t viewOffset_;
    size_t category_;

    std::string placeholder_;
};


// RoundRectangle

class RoundRectangle : public sf::Shape
{
public:
    RoundRectangle(sf::Vector2f size = { 0, 0 }, float radius = 0.f);

public:
    void setSize(sf::Vector2f size);
    sf::Vector2f getSize() const;
    void setRadius(float radius);
    float getRadius() const;
    std::size_t getPointCount() const override;
    sf::Vector2f getPoint(std::size_t index) const override;

private:
    sf::Vector2f m_size;
    float m_radius;

private:
    static constexpr std::size_t corner_points = 16;
    static constexpr float pi = 3.141592654;
};