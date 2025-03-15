﻿#include "../include/Widgets.h"


// Button

Button::Button(
	const size_t category,
	const sf::Vector2f& position,
	const std::string& normal_condition,
	const std::string& hovered_condition,
	const std::string& pressed_condition,
	const std::wstring& text,
	const sf::Font& font,
	size_t size_text,
	const sf::Color color_text_normal,
	const sf::Color color_text_hovered,
	const sf::Color color_text_pressed,
	const sf::Vector2f& position_text
)
	: category(category),
	position(position),
	text(text, font, size_text),
	font(font), size_text(size_text),
	color_text_normal(color_text_normal),
	color_text_hovered(color_text_hovered),
	color_text_pressed(color_text_pressed),
	position_text(position_text)
{
	normal_condition_texture.loadFromFile(normal_condition);
	hovered_condition_texture.loadFromFile(hovered_condition);
	pressed_condition_texture.loadFromFile(pressed_condition);

	button.setPosition(position);
	button.setTexture(normal_condition_texture);

	this->text.setFillColor(color_text_normal);

	sf::FloatRect buttonBounds = button.getGlobalBounds();
	sf::FloatRect textBounds = this->text.getLocalBounds();

	if (position_text != sf::Vector2f(0.f, 0.f)) {
		this->text.setPosition(position + position_text);
	}
	else {
		this->text.setPosition(
			buttonBounds.left + (buttonBounds.width - textBounds.width) / 2 - textBounds.left,
			buttonBounds.top + (buttonBounds.height - textBounds.height) / 2 - textBounds.top
		);
	}

	state = State::Normal;
}

void Button::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
	bool isMouseOver = button.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	isCursor = isMouseOver;

	if (event.type == sf::Event::MouseMoved) {
		// Наведение мыши
		if (isMouseOver && state != State::Pressed) {
			state = State::Hovered;
		}
		else if (!isMouseOver) {
			state = State::Normal;
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		// Нажатие левой кнопки мыши
		if (isMouseOver) {
			state = State::Pressed;
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		// Отпускание левой кнопки мыши
		if (isMouseOver && state == State::Pressed) {
			click = true;
		}
		if (isMouseOver) {
			state = State::Hovered;
		}
		else {
			state = State::Normal;
		}
	}
}

void Button::render(sf::RenderWindow& window)
{
	switch (state) {
	case State::Normal:
		button.setTexture(normal_condition_texture);
		text.setFillColor(color_text_normal);
		break;
	case State::Hovered:
		button.setTexture(hovered_condition_texture);
		text.setFillColor(color_text_hovered);
		break;
	case State::Pressed:
		button.setTexture(pressed_condition_texture);
		text.setFillColor(color_text_pressed);
		break;
	}

	window.draw(button);
	window.draw(text);
}

bool Button::clicking()
{
	if (click) {
		click = false;
		return true;
	}
	return false;
}

int Button::getCategory()
{
	return category;
}

sf::Vector2f Button::getPosition()
{
	return position;
}

void Button::setPosition(const sf::Vector2f& position)
{
	this->position = position;
	button.setPosition(position);
}

void Button::setPosition(float positionX, float positionY)
{
	this->position = sf::Vector2f(positionX, positionY);
	button.setPosition(position);
}

bool Button::cursor()
{
	return isCursor;
}

void Button::setTexture(int state, const std::string& newTexture)
{
	switch (state)
	{
	case 0:
		normal_condition_texture.loadFromFile(newTexture);
		break;
	case 1:
		hovered_condition_texture.loadFromFile(newTexture);
		break;
	case 2:
		pressed_condition_texture.loadFromFile(newTexture);
		break;
	default:
		break;
	}
}

void Button::updateCursor(sf::RenderWindow& window)
{
	sf::Cursor cursor;

	if (isCursor) {
		if (cursor.loadFromSystem(sf::Cursor::Hand)) {
			window.setMouseCursor(cursor);
		}
	}
	else {
		if (cursor.loadFromSystem(sf::Cursor::Arrow)) {
			window.setMouseCursor(cursor);
		}
	}
}

void Button::setScale(float scaleX, float scaleY)
{
	button.setScale(scaleX, scaleY);

	text.setScale(scaleX, scaleY);

	sf::FloatRect buttonBounds = button.getGlobalBounds();
	sf::FloatRect textBounds = text.getLocalBounds();

	text.setPosition(
		buttonBounds.left + (buttonBounds.width - textBounds.width * scaleX) / 2 - textBounds.left * scaleX,
		buttonBounds.top + (buttonBounds.height - textBounds.height * scaleY) / 2 - textBounds.top * scaleY
	);
}


// CheckButton

CheckButton::CheckButton(
	const size_t category,
	const sf::Vector2f& position,
	const std::string& normal_condition,
	const std::string& hovered_condition,
	const std::string& pressed_condition,
	const std::wstring& text,
	const sf::Font& font,
	size_t size_text,
	const sf::Color color_text_normal,
	const sf::Color color_text_hovered,
	const sf::Color color_text_pressed,
	const sf::Vector2f& position_text
)
	: Button(
		category,
		position,
		normal_condition,
		hovered_condition,
		pressed_condition,
		text,
		font,
		size_text,
		color_text_normal,
		color_text_hovered,
		color_text_pressed,
		position_text
	) {
}

void CheckButton::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
	bool isMouseOver = button.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	isCursor = isMouseOver;

	if (event.type == sf::Event::MouseMoved) {
		// Наведение мыши
		if (!click) {
			state = isMouseOver ? State::Hovered : State::Normal;
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		// Нажатие кнопки
		if (isMouseOver) {
			state = State::Pressed;
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		// Отпускание кнопки
		if (isMouseOver && state == State::Pressed) {
			click = !click;
		}
		state = click ? State::Pressed : (isMouseOver ? State::Hovered : State::Normal);
	}
}

bool CheckButton::clicking()
{
	return click;
}


// RadioButton

RadioButton::RadioButton(
	std::vector<RadioButton*>& group,
	const size_t category,
	const sf::Vector2f& position,
	const std::string& normal_condition,
	const std::string& hovered_condition,
	const std::string& pressed_condition,
	const std::wstring& text,
	const sf::Font& font,
	size_t size_text,
	const sf::Color color_text_normal,
	const sf::Color color_text_hovered,
	const sf::Color color_text_pressed,
	const sf::Vector2f& position_text)
	: Button(category, position, normal_condition, hovered_condition, pressed_condition, text, font, size_text,
		color_text_normal, color_text_hovered, color_text_pressed, position_text),
	group(group), text(text) {
};

void RadioButton::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
	bool isMouseOver = button.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
	isCursor = isMouseOver;

	if (event.type == sf::Event::MouseMoved) {
		if (isMouseOver && !selected) {
			state = State::Hovered;
		}
		else if (!isMouseOver && !selected) {
			state = State::Normal;
		}
	}
	else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		if (isMouseOver) {
			for (auto* radio : group) {
				radio->setSelected(false);
			}
			setSelected(true);
			click = true;
		}
	}
}

void RadioButton::render(sf::RenderWindow& window)
{
	Button::render(window);
}

void RadioButton::setSelected(bool value)
{
	selected = value;
	state = selected ? State::Pressed : State::Normal;

	if (selected) {
		for (auto* radio : group) {
			if (radio != this) {
				radio->click = false;
			}
		}
	}
}

void RadioButton::resetSelection() 
{
	for (auto* radio : group) {
		radio->setSelected(false);
	}
}

std::wstring RadioButton::getText()
{
	return text;
}


// InputField

InputField::InputField(
	const sf::Vector2f& position, 
	const std::string& texture_path, 
	const sf::Font& font, 
	const std::wstring& text,
	size_t limitation,
	sf::RenderWindow& window
)
	: isActive(false), defaultText(text), limitation(limitation), position(position)
{
	texture.loadFromFile(texture_path);
	sprite.setTexture(texture);
	sprite.setPosition(position);

	this->text.setFont(font);
	this->text.setFillColor(sf::Color(148, 148, 148));
	this->text.setCharacterSize(16);
	this->text.setString(defaultText);
	this->text.setPosition(position.x + 36, position.y + 10);

	arrowCursor.loadFromSystem(sf::Cursor::Arrow);
	textCursor.loadFromSystem(sf::Cursor::Text);

	cursor.setSize({ 1.f, static_cast<float>(this->text.getCharacterSize()) });
	cursor.setFillColor(sf::Color::White);
	cursor.setPosition(sf::Vector2f(position.x + 36, position.y + 12));
	cursorVisible = true;
	cursorTimer.restart();
	cursorPosition = 0;
}

void InputField::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
	updateCursor(window);

	if (event.type == sf::Event::MouseButtonPressed) {
		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		isActive = sprite.getGlobalBounds().contains(mousePos);

		if (isActive) {
			float minDist = std::numeric_limits<float>::max();
			size_t newCursorPos = 0;

			for (size_t i = 0; i <= fullText.size(); ++i) {
				std::wstring substring = fullText.substr(0, i);
				sf::Text tempText = text;
				tempText.setString(substring);

				float charX = text.getPosition().x + tempText.getLocalBounds().width;
				float dist = std::abs(mousePos.x - charX);

				if (dist < minDist) {
					minDist = dist;
					newCursorPos = i;
				}
			}

			cursorPosition = newCursorPos;
			updateCursorPosition();
		}
	}

	if (isActive && event.type == sf::Event::TextEntered) {
		sf::Uint32 unicode = event.text.unicode;

		if (fullText == defaultText) {
			fullText.clear();
			text.setFillColor(sf::Color::White);
		}

		if (unicode == 8) { // Backspace
			if (!fullText.empty() && cursorPosition > 0) {
				fullText.erase(cursorPosition - 1, 1);
				--cursorPosition;
			}
		}
		else if (unicode == 13) { // Enter
			if (!fullText.empty()) {
				submitted = true;
				cursorPosition = 0;
			}
		}
		else if ((unicode >= 32 && unicode <= 126) || (unicode >= 1040 && unicode <= 1103) || unicode == 1025 || unicode == 1105) {
			if (fullText.size() < limitation) {
				fullText.insert(cursorPosition, 1, static_cast<wchar_t>(unicode));
				++cursorPosition;
			}
		}

		updateDisplayedText();
	}

	if (isActive && event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Left && cursorPosition > 0) {
			--cursorPosition;
		}
		else if (event.key.code == sf::Keyboard::Right && cursorPosition < fullText.size()) {
			++cursorPosition;
		}
		updateCursorPosition();
	}
}

void InputField::render(sf::RenderWindow& window)
{
	window.draw(sprite);
	window.draw(text);

	if (isActive && cursorTimer.getElapsedTime().asSeconds() > 0.5f) {
		cursorVisible = !cursorVisible;
		cursorTimer.restart();
	}

	if (isActive && cursorVisible) {
		window.draw(cursor);
	}
}

void InputField::updateCursor(sf::RenderWindow& window)
{
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	float leftBound = sprite.getPosition().x + 36.f;
	float rightBound = leftBound + static_cast<float>(limitation);

	if (sprite.getGlobalBounds().contains(mousePos)) {
		if (mousePos.x >= leftBound && mousePos.x <= rightBound) {
			window.setMouseCursor(textCursor);
		}
	}
	else {
		window.setMouseCursor(arrowCursor);
	}
}

sf::Vector2f InputField::getPosition()
{
	return sprite.getPosition();
}

void InputField::setPosition(const sf::Vector2f& position)
{
	sprite.setPosition(position);
	text.setPosition(position.x + 10.f, position.y + (sprite.getGlobalBounds().height * 0.15f));
}

std::wstring InputField::getText() const
{
	if (text.getString() == defaultText) {
		return std::wstring();
	}
	return text.getString();
}

void InputField::setText(const std::string& str)
{
	text.setString(str);
}

bool InputField::isSubmitted() const
{ 
	return submitted;
}

void InputField::resetSubmission()
{
	submitted = false;
	fullText.clear();
	text.setString(defaultText);
	text.setFillColor(sf::Color(148, 148, 148));
	cursorPosition = 0;
	cursor.setPosition(sf::Vector2f(position.x + 36, position.y + 12));
}

void InputField::updateDisplayedText()
{
	if (fullText.empty()) {
		text.setString(defaultText);
		text.setFillColor(sf::Color(148, 148, 148));
		return;
	}

	std::wstring displayedText = fullText;
	text.setString(displayedText);
	text.setFillColor(sf::Color::White);

	while (text.getGlobalBounds().width > limitation && !displayedText.empty()) {
		displayedText.erase(0, 1);
		text.setString(displayedText);
	}

	updateCursorPosition();
}

void InputField::updateCursorPosition()
{
	if (cursorPosition > fullText.size()) {
		cursorPosition = fullText.size();
	}

	float cursorX = text.getPosition().x;

	std::wstring substring = fullText.substr(0, cursorPosition);
	sf::Text tempText = text;
	tempText.setString(substring);

	float textWidth = tempText.getLocalBounds().width;
	cursor.setPosition(cursorX + textWidth, text.getPosition().y);
}


// Squircle

Squircle::Squircle(float radius, float n, std::size_t resolution)
	: m_radius(radius)
	, m_n(n)
	, m_resolution(resolution) 
{
	update();
}

void Squircle::setRadius(float radius) 
{
	m_radius = radius;
	update();
}

float Squircle::getRadius() const 
{
	return m_radius;
}

void Squircle::setN(float p) 
{
	m_n = p;
	update();
}

float Squircle::getN() const 
{
	return m_n;
}

void Squircle::setResolution(std::size_t resolution) 
{
	m_resolution = resolution;
	update();
}

std::size_t Squircle::getResolution() const {
	return m_resolution * 4;
}

namespace {
	static constexpr float pi = 3.141592654;

	static constexpr float map(float value, float a, float b, float c, float d) {
		return (value - a) / (b - a) * (d - c) + c;
	}

	static constexpr float sign(float value) {
		if (value > 0) return 1;
		if (value < 0) return -1;
		return 0;
	}
}

std::size_t Squircle::getPointCount() const 
{
	return m_resolution * 4;
}

sf::Vector2f Squircle::getPoint(std::size_t index) const
{
	auto local = index % (m_resolution * 2);
	auto bottom = (index >= m_resolution * 2) ? -1.f : 1.f;

	auto x = map(local, 0, m_resolution * 2, -1, 1);
	x = (1 - std::pow(1 - std::abs(x), 3)) * sign(x) * bottom;
	x = map(x, -1, 1, -m_radius, m_radius);

	auto y = m_radius * std::pow(1.f - std::pow(std::abs(x / m_radius), m_n), 1.f / m_n);

	return { x, y * bottom };
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