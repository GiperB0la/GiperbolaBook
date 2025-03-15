#include "../include/Chat.h"
#include <windows.h>
#include <shellapi.h>


Chat::Chat(const std::string& client_name, const std::string& client_ip, int client_port, const sf::RectangleShape& subChat, sf::Font& font, const sf::RenderWindow& window)
	: client_name(client_name), client_ip(client_ip), client_port(client_port),
	scrollbarTrack(sf::Vector2f(6.f, 824.f), 5), scrollbarSlider(sf::Vector2f(6.f, 824.f), 5),
	down(0,
		sf::Vector2f(1840.f, 889.f),
		"Graphics\\Buttons\\Down\\Down.png",
		"Graphics\\Buttons\\Down\\Down.png",
		"Graphics\\Buttons\\Down\\Down.png",
		L"",
		font)
{
	scrollbarTrack.setFillColor(sf::Color(31, 31, 31));
	scrollbarTrack.setPosition(1889.f, 99.f);

	scrollbarSlider.setFillColor(sf::Color(80, 80, 80));
	scrollbarSlider.setPosition(1889.f, 99.f);

	view = sf::View(sf::FloatRect(0, 0, subChat.getSize().x, subChat.getSize().y));
	view.setCenter(subChat.getPosition().x + subChat.getSize().x / 2,
		subChat.getPosition().y + subChat.getSize().y / 2);
	view.setViewport(sf::FloatRect(
		subChat.getPosition().x / window.getSize().x,
		subChat.getPosition().y / window.getSize().y,
		subChat.getSize().x / window.getSize().x,
		subChat.getSize().y / window.getSize().y
	));
}

void Chat::addMessage(bool isMyMessage, const std::wstring& message, const std::wstring& time, sf::Font& font, sf::RectangleShape& subChat)
{
	float offsetYMessage = 10.f;

	if (!messages.empty()) {
		bool lastMessageIsMine = messages.back().isMyMessage;
		if (lastMessageIsMine == isMyMessage) {
			offsetYMessage = 5.f;
		}
	}

	sf::Text text;
	text.setCharacterSize(14);
	text.setString(message);
	text.setFont(font);
	text.setFillColor(sf::Color::White);

	float textWidth = text.getLocalBounds().width;

	float paddingX = 20.f;
	float paddingY = 10.f;

	sf::Vector2f frameSize(
		textWidth + paddingX * 2 + 14.f,
		37.f
	);

	RoundRectangle frame(frameSize, 10);
	frame.setFillColor(isMyMessage ? sf::Color(31, 31, 31) : sf::Color(80, 80, 80));

	float posX = subChat.getPosition().x;
	float posY;

	if (messages.empty()) {
		posY = subChat.getPosition().y + subChat.getSize().y - frameSize.y - 6.f;
	}
	else {
		posY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y + offsetYMessage;
	}

	frame.setPosition(posX, posY);
	text.setPosition(posX + paddingX, posY + paddingY);

	sf::Text time_;
	time_.setCharacterSize(10);
	time_.setString(time);
	time_.setFont(font);
	time_.setFillColor(sf::Color(160, 160, 160));

	float timePosX = posX + frameSize.x - time_.getLocalBounds().width - 6.f;
	float timePosY = posY + frameSize.y - time_.getLocalBounds().height - 5.f;
	time_.setPosition(timePosX, timePosY);

	messages.emplace_back(isMyMessage, text, time_, frame);

	updatePosition(subChat);
}

void Chat::addMessageFile(bool isMyMessage, const std::filesystem::path& message, const std::wstring& time, sf::Font& font, sf::RectangleShape& subChat)
{
	float offsetYMessage = 10.f;

	if (!messages.empty()) {
		bool lastMessageIsMine = messages.back().isMyMessage;
		if (lastMessageIsMine == isMyMessage) {
			offsetYMessage = 5.f;
		}
	}

	sf::Text text;
	text.setCharacterSize(14);
	text.setString(message.c_str());
	text.setFont(font);
	text.setFillColor(sf::Color::White);
	std::cout << text.getString().toAnsiString() << std::endl;

	float textWidth = text.getLocalBounds().width;

	float paddingX = 35.f;
	float paddingY = 10.f;

	sf::Vector2f frameSize(
		textWidth + paddingX * 2 + -1.f,
		37.f
	);

	RoundRectangle frame(frameSize, 10);
	frame.setFillColor(isMyMessage ? sf::Color(31, 31, 31) : sf::Color(80, 80, 80));

	float posX = subChat.getPosition().x;
	float posY;

	if (messages.empty()) {
		posY = subChat.getPosition().y + subChat.getSize().y - frameSize.y - 6.f;
	}
	else {
		posY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y + offsetYMessage;
	}

	frame.setPosition(posX, posY);
	text.setPosition(posX + paddingX, posY + paddingY);

	sf::Text time_;
	time_.setCharacterSize(10);
	time_.setString(time);
	time_.setFont(font);
	time_.setFillColor(sf::Color(160, 160, 160));

	float timePosX = posX + frameSize.x - time_.getLocalBounds().width - 6.f;
	float timePosY = posY + frameSize.y - time_.getLocalBounds().height - 5.f;
	time_.setPosition(timePosX, timePosY);

	messages.emplace_back(isMyMessage, text, time_, frame, true, Button(0, sf::Vector2f(posX + 6.f, posY + 6.f),
		Config::MainPath.string() + "\\Graphics\\Buttons\\File\\File.png", Config::MainPath.string() + "\\Graphics\\Buttons\\File\\File.png",
		Config::MainPath.string() + "\\Graphics\\Buttons\\File\\File.png", L"", font), (std::filesystem::path(Config::MainFolderPath) / message));

	updatePosition(subChat);
}

void Chat::changeWindow(bool isMaximum, sf::RectangleShape& subChat, sf::RenderWindow& window)
{
	if (isMaximum) {
		scrollbarTrack.setSize(sf::Vector2f(6.f, 824.f));
		scrollbarTrack.setPosition(sf::Vector2f(1889.f, 99.f));

		scrollbarSlider.setSize(sf::Vector2f(6.f, 824.f));
		scrollbarSlider.setPosition(sf::Vector2f(1889.f, 99.f));

		down.setPosition(sf::Vector2f(1840.f, 889.f));
	}
	else {
		scrollbarTrack.setSize(sf::Vector2f(6.f, 586.f));
		scrollbarTrack.setPosition(sf::Vector2f(1171.f, 99.f));

		scrollbarSlider.setSize(sf::Vector2f(6.f, 586.f));
		scrollbarSlider.setPosition(sf::Vector2f(1171.f, 99.f));

		down.setPosition(sf::Vector2f(1129.f, 651.f));
	}

	view = sf::View(sf::FloatRect(0, 0, subChat.getSize().x, subChat.getSize().y));
	view.setCenter(subChat.getPosition().x + subChat.getSize().x / 2,
		subChat.getPosition().y + subChat.getSize().y / 2);
	view.setViewport(sf::FloatRect(
		subChat.getPosition().x / window.getSize().x,
		subChat.getPosition().y / window.getSize().y,
		subChat.getSize().x / window.getSize().x,
		subChat.getSize().y / window.getSize().y
	));

	updatePosition(subChat);
}

void Chat::updatePosition(sf::RectangleShape& subChat)
{
	if (!messages.empty()) {
		float paddingX = 20.f;
		float paddingY = 10.f;
		float lastMessageY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y;
		float chatBottomY = subChat.getPosition().y + subChat.getSize().y;

		if (lastMessageY > chatBottomY) {
			float shift = lastMessageY - chatBottomY;

			for (auto& msg : messages) {
				sf::Vector2f newPos = msg.frame.getPosition();
				newPos.y -= shift;
				msg.frame.setPosition(newPos);
				if (msg.isFile) {
					msg.text.setPosition(newPos.x + 35.f, newPos.y + paddingY);
					msg.file->setPosition(newPos.x + 6.f, newPos.y + 6.f);
				}
				else {
					msg.text.setPosition(newPos.x + paddingX, newPos.y + paddingY);
				}
				msg.time.setPosition(newPos.x + msg.frame.getSize().x - msg.time.getLocalBounds().width - 6.f,
					newPos.y + msg.frame.getSize().y - msg.time.getLocalBounds().height - 5.f);
			}
		}
		else {
			float firstMessageY = messages.front().frame.getPosition().y;
			float shift = chatBottomY - lastMessageY;

			for (auto& msg : messages) {
				sf::Vector2f newPos = msg.frame.getPosition();
				newPos.y += shift;
				msg.frame.setPosition(newPos);
				if (msg.isFile) {
					msg.text.setPosition(newPos.x + 35.f, newPos.y + paddingY);
					msg.file->setPosition(newPos.x + 6.f, newPos.y + 6.f);
				}
				else {
					msg.text.setPosition(newPos.x + paddingX, newPos.y + paddingY);
				}
				msg.time.setPosition(newPos.x + msg.frame.getSize().x - msg.time.getLocalBounds().width - 6.f,
					newPos.y + msg.frame.getSize().y - msg.time.getLocalBounds().height - 5.f);
			}
		}

		updateScrollbar();
	}
}

void Chat::handleEvent(const sf::Event& event, sf::Sprite& ChatFrameSprite, sf::RectangleShape& subChat, sf::RenderWindow& window)
{
	sf::FloatRect chatBounds = ChatFrameSprite.getGlobalBounds();
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	if (event.type == sf::Event::MouseWheelScrolled) {
		if (chatBounds.contains(mousePos) && !messages.empty()) {
			float messagesHeight = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - messages.front().frame.getPosition().y;

			if (messagesHeight <= subChat.getSize().y) {
				viewY = messages.front().frame.getPosition().y;
				isDown = false;
				return;
			}

			viewY += (event.mouseWheelScroll.delta > 0) ? -scrollSpeed : scrollSpeed;

			float minY = messages.front().frame.getPosition().y;
			float maxY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - subChat.getSize().y;
			viewY = std::clamp(viewY, minY, maxY);

			view.setCenter(view.getCenter().x, viewY + subChat.getSize().y / 2.0f);

			isDown = (viewY < maxY);
		}
	}

	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		if (scrollbarSlider.getGlobalBounds().contains(mousePos)) {
			draggingScrollbar = true;
			lastMousePos = mousePos;
		}
	}

	if (event.type == sf::Event::MouseMoved && draggingScrollbar) {
		float deltaY = mousePos.y - lastMousePos.y;
		lastMousePos = mousePos;

		float trackHeight = scrollbarTrack.getSize().y;
		float sliderHeight = scrollbarSlider.getSize().y;
		float scrollRange = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - messages.front().frame.getPosition().y - subChat.getSize().y;

		if (scrollRange > 0) {
			float scrollRatio = scrollRange / (trackHeight - sliderHeight);
			viewY += deltaY * scrollRatio;

			float minY = messages.front().frame.getPosition().y;
			float maxY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - subChat.getSize().y;
			viewY = std::clamp(viewY, minY, maxY);

			view.setCenter(view.getCenter().x, viewY + subChat.getSize().y / 2.0f);

			isDown = (viewY < maxY);
		}
	}

	if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		draggingScrollbar = false;
	}

	if (isDown) {
		down.handleEvent(event, window);

		if (down.cursor()) {
			static sf::Cursor handCursor;
			if (handCursor.loadFromSystem(sf::Cursor::Hand)) {
				window.setMouseCursor(handCursor);
			}
		}

		if (down.clicking()) {
			if (!messages.empty()) {
				float maxY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - subChat.getSize().y;
				viewY = maxY;
				view.setCenter(view.getCenter().x, viewY + subChat.getSize().y / 2.0f);

				isDown = false;
			}
		}
	}

	for (auto& msg : messages) {
		if (msg.file) {
			msg.file->handleEvent(event, window);
			if (msg.file->cursor()) {
				static sf::Cursor handCursor;
				if (handCursor.loadFromSystem(sf::Cursor::Hand)) {
					window.setMouseCursor(handCursor);
				}
			}

			if (msg.file->clicking()) {
				if (msg.pathFile.has_value() && !msg.pathFile->empty()) {
					std::wstring wFilePath = Config::MainPath.wstring() + L"\\" + msg.pathFile.value().wstring();
					std::wcout << wFilePath << std::endl;
					ShellExecuteW(NULL, L"open", wFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}
	}

	updateScrollbar();
}

void Chat::render(sf::RenderWindow& window)
{
	window.setView(view);
	for (auto& message : messages) {
		window.draw(message.frame);
		window.draw(message.text);
		window.draw(message.time);
		if (message.isFile) {
			message.file->render(window);
		}
	}
	window.setView(window.getDefaultView());
	window.draw(scrollbarTrack);
	window.draw(scrollbarSlider);
	if (isDown) {
		down.render(window);
	}
}

void Chat::updateScrollbar()
{
	if (messages.empty()) {
		scrollbarSlider.setPosition(scrollbarTrack.getPosition());
		return;
	}

	float messagesHeight = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - messages.front().frame.getPosition().y;

	if (messagesHeight <= scrollbarTrack.getSize().y) {
		return;
	}

	float scrollRange = messagesHeight - scrollbarTrack.getSize().y;
	float trackHeight = scrollbarTrack.getSize().y;
	float sliderHeight = std::max(trackHeight * (scrollbarTrack.getSize().y / messagesHeight), 20.f);

	scrollbarSlider.setSize({ scrollbarSlider.getSize().x, sliderHeight });

	float minViewY = messages.front().frame.getPosition().y;
	float maxViewY = messages.back().frame.getPosition().y + messages.back().frame.getSize().y - trackHeight;
	float clampedViewY = std::clamp(viewY, minViewY, maxViewY);

	float scrollPercentage = (clampedViewY - minViewY) / scrollRange;
	float sliderY = scrollbarTrack.getPosition().y + scrollPercentage * (trackHeight - sliderHeight);

	scrollbarSlider.setPosition(scrollbarTrack.getPosition().x, sliderY);
}