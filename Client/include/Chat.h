#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <filesystem>
#include "Widgets.h"


class Config 
{
public:
	static inline std::string MainFolderPath = "Giperbola Files\\";
	const static inline std::filesystem::path MainPath = std::filesystem::current_path();
};

class Chat
{
public:
	Chat(const std::string& client_name, const std::string& client_ip, int client_port, const sf::RectangleShape& subChat, sf::Font& font, const sf::RenderWindow& window);

public:
	void handleEvent(const sf::Event& event, sf::Sprite& ChatFrameSprite, sf::RectangleShape& subChat, sf::RenderWindow& window);
	void render(sf::RenderWindow& window);
	void addMessage(bool isMyMessage, const std::wstring& message, const std::wstring& time, sf::Font& font, sf::RectangleShape& subChat);
	void addMessageFile(bool isMyMessage, const std::filesystem::path& message, const std::wstring& time, sf::Font& font, sf::RectangleShape& subChat);
	void changeWindow(bool isMaximum, sf::RectangleShape& subChat, sf::RenderWindow& window);

private:
	void updateScrollbar();
	void updatePosition(sf::RectangleShape& subChat);

private:
	struct Message
	{
		bool isMyMessage;
		bool isFile;
		sf::Text text;
		sf::Text time;
		RoundRectangle frame;
		std::optional<Button> file;
		std::optional<std::filesystem::path> pathFile;

		Message(bool isMyMessage, const sf::Text& text, const sf::Text& time, const RoundRectangle& frame, 
			bool isFile = false, std::optional<Button> file = std::nullopt, std::optional<std::filesystem::path> pathFile = std::nullopt)
			: isMyMessage(isMyMessage), text(text), time(time), frame(frame), isFile(isFile), file(file), pathFile(pathFile) {
		}
	};

public:
	std::string client_name;
	std::string client_ip;
	int client_port;
	std::vector<Message> messages;
	RoundRectangle scrollbarTrack;
	RoundRectangle scrollbarSlider;
	sf::View view;
	float scrollSpeed = 20.0f;
	float viewY = 0.0f;
	bool draggingScrollbar = false;
	sf::Vector2f lastMousePos;
	Button down;
	bool isDown = false;
};

struct User
{
	RadioButton* radioButton;
	Chat* chat;

	User(RadioButton* radioButton, Chat* chat)
		: radioButton(radioButton), chat(chat) {
	};
};