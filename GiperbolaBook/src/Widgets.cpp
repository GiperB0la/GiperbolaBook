#include "../include/Widgets.hpp"


// Button

Button::Button(
    const size_t category,
    const sf::Vector2f& position,
    const std::string& texNormalPath,
    const std::string& texHoverPath,
    const std::string& texPressedPath)
    : category(category), pressedInside_(false), clickedEdge_(false)
{
    if (!texNormal_.loadFromFile(texNormalPath)) {
        throw std::runtime_error("Failed to load " + texNormalPath);
    }
    if (!texHover_.loadFromFile(texHoverPath)) {
        throw std::runtime_error("Failed to load " + texHoverPath);
    }
    if (!texPressed_.loadFromFile(texPressedPath)) {
        throw std::runtime_error("Failed to load " + texPressedPath);
    }

    state_ = State::Normal;
    sprite_.setPosition(position);
    sprite_.setTexture(texNormal_, true);
}

size_t Button::get_category() const
{
    return category;
}

bool Button::set_category(const size_t category)
{
    this->category = category;
    return true;
}

void Button::handle_event(const sf::Event& event, const sf::RenderWindow& window)
{
    clickedEdge_ = false;

    auto containsMouse = [&](sf::Vector2f pt) {
        return sprite_.getGlobalBounds().contains(pt);
        };

    switch (event.type)
    {
    case sf::Event::MouseMoved:
    {
        if (!pressedInside_) {
            auto mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            set_state(containsMouse(mp) ? State::Hovered : State::Normal);
        }
        break;
    }
    case sf::Event::MouseButtonPressed:
    {
        if (event.mouseButton.button == sf::Mouse::Left) {
            auto mp = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
            if (containsMouse(mp)) {
                pressedInside_ = true;
                set_state(State::Pressed);
            }
        }
        break;
    }
    case sf::Event::MouseButtonReleased:
    {
        if (event.mouseButton.button == sf::Mouse::Left) {
            auto mp = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
            bool inside = containsMouse(mp);
            if (pressedInside_) {
                if (inside) clickedEdge_ = true;
                set_state(inside ? State::Hovered : State::Normal);
            }
            else {
                set_state(inside ? State::Hovered : State::Normal);
            }
            pressedInside_ = false;
        }
        break;
    }
    default: break;
    }
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(sprite_);
}

sf::Vector2f Button::get_position() const
{
    return sprite_.getPosition();
}

bool Button::set_position(const sf::Vector2f& position)
{
    sprite_.setPosition(position);
    return true;
}

State Button::get_state() const
{
    return state_;
}

bool Button::set_state(State new_state)
{
    if (state_ == new_state) return true;
    state_ = new_state;
    apply_texture_for_state();
    return true;
}

bool Button::is_clicked()
{
    bool out = clickedEdge_;
    clickedEdge_ = false;
    return out;
}

bool Button::is_hovered() const
{
    return state_ == State::Hovered;
}

sf::FloatRect Button::bounds() const
{
    return sprite_.getGlobalBounds();
}

void Button::apply_texture_for_state()
{
    switch (state_)
    {
    case State::Normal:     sprite_.setTexture(texNormal_, true); break;
    case State::Hovered:    sprite_.setTexture(texHover_, true); break;
    case State::Pressed:    sprite_.setTexture(texPressed_, true); break;
    }
}


// CheckButton

CheckButton::CheckButton(
    const size_t category,
    const sf::Vector2f& position,
    const std::string& texNormalPath,
    const std::string& texHoverPath,
    const std::string& texPressedPath
)
    : Button(category, position, texNormalPath, texHoverPath, texPressedPath) {
}

void CheckButton::handle_event(const sf::Event& event, const sf::RenderWindow& window)
{
    Button::handle_event(event, window);

    if (is_clicked()) {
        checked_ = !checked_;
        apply_texture_for_state();
    }
}

void CheckButton::apply_texture_for_state()
{
    if (checked_) {
        switch (get_state()) {
        case State::Normal:  sprite_.setTexture(texPressed_, true); break;
        case State::Hovered: sprite_.setTexture(texHover_, true);  break;
        case State::Pressed: sprite_.setTexture(texPressed_, true); break;
        }
        return;
    }

    switch (get_state()) {
    case State::Normal:  sprite_.setTexture(texNormal_, true); break;
    case State::Hovered: sprite_.setTexture(texHover_, true); break;
    case State::Pressed: sprite_.setTexture(texPressed_, true); break;
    }
}

bool CheckButton::is_checked() const noexcept
{
    return checked_;
}

void CheckButton::set_checked(bool v)
{
    if (checked_ == v) return;
    checked_ = v;
    apply_texture_for_state();
}