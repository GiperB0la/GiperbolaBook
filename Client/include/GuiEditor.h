#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <filesystem>
#include "Widgets.h"
#include "Client.h"
#include "Chat.h"


enum class CategoryButton 
{
	Close = 0,
	Unwrap = 1,
	RollUp = 2,
	User = 3,
	Send = 4,
	CloseUser = 5,
	Clip = 6
};

class Client;

class GuiEditor
{
public:
	friend Client;

public:
	GuiEditor(Client* client);
	~GuiEditor() = default;

public:
	void start();

private:
	void init();
	void createButtons();
	void processEvents();
	void render();
	void unwrap();
	void processEventsHello();
	void renderHello();
	void runMainWindow();
	void updateUsersFrames();
	void addMessage(const std::string& client_ip, int& client_port, bool isMyMessage, const std::string& message, const std::wstring& time, bool isFile = false);
	void dragAndDrop();
	std::optional<std::filesystem::path> openFileDialog();

private:
	sf::RenderWindow window;
	sf::Font font;
	sf::ContextSettings ctx;

private:
	sf::Texture EmblemTexture;
	sf::Sprite EmblemSprite;
	sf::Texture WindowFrameHelloTexture, HelloFrameTexture;
	sf::Sprite WindowFrameHelloSprite, HelloFrameSprite;
	sf::Texture WindowFrameMaximumTexture, StartChatFrameMaximumTexture, ChatFrameMaximumTexture, UsersFrameMaximumTexture, 
		UserFrameTopMaximumTexture, UserFrameLowerMaximumTexture, DragAndDropMaximumTexture, NoSignalMaximumTexture;
	sf::Sprite WindowFrameMaximumSprite, StartChatFrameMaximumSprite, ChatFrameMaximumSprite, UsersFrameMaximumSprite, 
		UserFrameTopMaximumSprite, UserFrameLowerMaximumSprite, DragAndDropMaximumSprite, NoSignalMaximumSprite;
	sf::Texture WindowFrameMinimalTexture, StartChatFrameMinimalTexture, ChatFrameMinimalTexture, UsersFrameMinimalTexture, 
		UserFrameTopMinimalTexture, UserFrameLowerMinimalTexture, DragAndDropMinimalTexture, NoSignalMinimalTexture;
	sf::Sprite WindowFrameMinimalSprite, StartChatFrameMinimalSprite, ChatFrameMinimalSprite, UsersFrameMinimalSprite, 
		UserFrameTopMinimalSprite, UserFrameLowerMinimalSprite, DragAndDropMinimalSprite, NoSignalMinimalSprite;

private:
	std::vector<Button> buttonsWindowPanel;
	std::vector<Button> buttons;
	std::vector<User> usersFrames;
	std::vector<RadioButton*> usersFramesButtons;
	InputField inputFieldMaximumMain;
	InputField inputFieldMinimalMain;
	InputField inputFieldHello;
	sf::Text nameUser;
	sf::RectangleShape subChatMaximal;
	sf::RectangleShape subChatMinimal;

private:
	bool run = false;
	bool isDraggingHello = false;
	bool isMaximum = true;
	bool isStart = true;
	sf::Vector2i dragOffsetHello;
	Client* client = nullptr;
	std::vector<std::shared_ptr<Chat>> chats;
	bool isDraggingMinimal = false;
	sf::Vector2i dragOffsetMinimal;
	sf::Vector2i lastMinimalWindowPos;
};