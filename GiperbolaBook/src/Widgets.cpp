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
        sprite_.setTexture(texPressed_, true);
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


// SelectButton

SelectButton::SelectButton(
    const size_t category,
    const sf::Vector2f& position,
    const std::string& texNormalPath,
    const std::string& texHoverPath,
    const std::string& texPressedPath
)
    : Button(category, position, texNormalPath, texHoverPath, texPressedPath) {
}

void SelectButton::handle_event(const sf::Event& event, const sf::RenderWindow& window)
{
    Button::handle_event(event, window);

    if (is_clicked()) {
        if (!selected_) {
            selected_ = true;
            just_selected_ = true;
            apply_texture_for_state();
        }
    }
}

void SelectButton::apply_texture_for_state()
{
    if (selected_) {
        sprite_.setTexture(texPressed_, true);
        return;
    }

    switch (get_state()) {
    case State::Normal:  sprite_.setTexture(texNormal_, true); break;
    case State::Hovered: sprite_.setTexture(texHover_, true);  break;
    case State::Pressed: sprite_.setTexture(texPressed_, true); break;
    }
}

bool SelectButton::is_selected_once()
{
    if (just_selected_) {
        just_selected_ = false;
        return true;
    }
    return false;
}

bool SelectButton::is_selected() const noexcept
{
    return selected_;
}

void SelectButton::deselect()
{
    selected_ = false;
    apply_texture_for_state();
}


// InputField

InputField::InputField(const sf::Vector2f& pos,
    const sf::Vector2f& size,
    const sf::Font& font,
    unsigned int charSize,
    sf::Color color,
    const std::string& placeholder)
    : rect_(size),
    cursorIndex_(0),
    viewOffset_(0),
    category_(0),
    placeholder_(placeholder)
{
    rect_.setPosition(pos);
    rect_.setFillColor(color);

    text_.setFont(font);
    text_.setCharacterSize(charSize);
    text_.setFillColor(sf::Color::White);
    text_.setPosition(pos.x + 5, pos.y + 5);

    placeholderText_.setFont(font);
    placeholderText_.setCharacterSize(charSize);
    placeholderText_.setFillColor(sf::Color(150, 150, 150));
    placeholderText_.setString(placeholder_);
    placeholderText_.setPosition(pos.x + 5, pos.y + 5);

    cursor_.setSize({ 2.f, (float)charSize });
    cursor_.setFillColor(sf::Color::White);
    updateCursor();
}

size_t InputField::get_category() const { return category_; }

bool InputField::set_category(const size_t category) 
{
    category_ = category;
    return true;
}

void InputField::handle_event(const sf::Event& event, const sf::RenderWindow& window)
{
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode >= 32 && event.text.unicode < 128) {
            input_.insert(input_.begin() + cursorIndex_, (char)event.text.unicode);
            cursorIndex_++;
            adjustView();
        }
    }
    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::BackSpace && cursorIndex_ > 0) {
            input_.erase(input_.begin() + cursorIndex_ - 1);
            cursorIndex_--;
            adjustView();
        }
        else if (event.key.code == sf::Keyboard::Left && cursorIndex_ > 0) {
            cursorIndex_--;
            adjustView();
        }
        else if (event.key.code == sf::Keyboard::Right && cursorIndex_ < input_.size()) {
            cursorIndex_++;
            adjustView();
        }
    }
}

void InputField::draw(sf::RenderWindow & window) 
{
    window.draw(rect_);

    if (input_.empty()) {
        window.draw(placeholderText_);
    }
    else {
        text_.setString(getVisibleText());
        window.draw(text_);
        window.draw(cursor_);
    }
}

void InputField::set_placeholder(const std::string& str)
{
    placeholder_ = str;
    placeholderText_.setString(str);
}

sf::Vector2f InputField::get_position() const
{
    return rect_.getPosition();
}
bool InputField::set_position(const sf::Vector2f& position)
{
    rect_.setPosition(position);
    text_.setPosition(position.x + 5,
        position.y + (rect_.getSize().y - text_.getCharacterSize()) / 2);
    updateCursor();
    return true;
}

sf::FloatRect InputField::bounds() const
{
    return rect_.getGlobalBounds();
}

std::string InputField::get_value() const { return input_; }

bool InputField::clear()
{
    input_.clear();
    cursorIndex_ = 0;
    viewOffset_ = 0;
    updateCursor();
    return true;
}

std::string InputField::getVisibleText() 
{
    std::string visible;
    sf::Text tmp(text_);
    float maxWidth = rect_.getSize().x - 10.f;

    for (size_t i = viewOffset_; i < input_.size(); i++) {
        tmp.setString(visible + input_[i]);
        if (tmp.getLocalBounds().width > maxWidth) break;
        visible += input_[i];
    }
    return visible;
}

void InputField::adjustView()
{
    if (cursorIndex_ > input_.size())
        cursorIndex_ = input_.size();

    if (viewOffset_ > cursorIndex_)
        viewOffset_ = cursorIndex_;

    while (getCursorX() > rect_.getSize().x - 10 && viewOffset_ < input_.size()) {
        viewOffset_++;
    }
    while (cursorIndex_ < viewOffset_) {
        if (viewOffset_ > 0) viewOffset_--;
    }

    updateCursor();
}

float InputField::getCursorX() 
{
    size_t localIndex = (cursorIndex_ > viewOffset_) ? cursorIndex_ - viewOffset_ : 0;
    std::string part = input_.substr(viewOffset_, localIndex);
    sf::Text tmp(text_);
    tmp.setString(part);
    return tmp.getLocalBounds().width + 5.f;
}

void InputField::updateCursor() 
{
    cursor_.setPosition(rect_.getPosition().x + getCursorX(),
        rect_.getPosition().y + (rect_.getSize().y - cursor_.getSize().y) / 2);
}


// RoundRectangle

RoundRectangle::RoundRectangle(sf::Vector2f size, float radius)
    : m_size(size)
    , m_radius(radius)
{
    update();
}

void RoundRectangle::setSize(sf::Vector2f size)
{
    m_size = size;
    update();
}

sf::Vector2f RoundRectangle::getSize() const
{
    return m_size;
}

void RoundRectangle::setRadius(float radius)
{
    m_radius = radius;
    update();
}

float RoundRectangle::getRadius() const
{
    return m_radius;
}

std::size_t RoundRectangle::getPointCount() const
{
    return corner_points * 4;
}

sf::Vector2f RoundRectangle::getPoint(std::size_t index) const
{
    float da = 90.f / (corner_points - 1);
    auto centre_i = index / corner_points;

    sf::Vector2f centre{ centre_i == 1 || centre_i == 2 ? m_radius : m_size.x - m_radius,
                        centre_i == 0 || centre_i == 1 ? m_radius : m_size.y - m_radius };

    return { m_radius * std::cos(da * (index - centre_i) * pi / 180) + centre.x,
           -m_radius * std::sin(da * (index - centre_i) * pi / 180) + centre.y };
}