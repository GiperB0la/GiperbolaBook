#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <filesystem>
#include "Widgets.hpp"

class WelcomeWindow
{
public:
	WelcomeWindow();

public:
	bool init();
	std::string run();

private:
	void processEvents();
	void render();
	void draggingWindow(const sf::Event& event);
	void readyNickname();

private:
	std::unique_ptr<sf::RenderWindow> window_;
	sf::Font font_;
	sf::Texture windowT_;
	sf::Sprite windowS_;
	sf::Cursor arrowCursor_, handCursor_;

	const std::string main_path_;
	bool dragging_window_ = false;
	sf::Vector2i dragOffset_;

	std::unique_ptr<InputField> input_field_;
	std::unique_ptr<Button> send_;

	std::string nickname_;
	bool go_ = false;
};