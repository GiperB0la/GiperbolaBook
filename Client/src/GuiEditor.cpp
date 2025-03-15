#include "../include/GuiEditor.h"
#include "../include/DropTarget.h"
#include <windows.h>
#include <commdlg.h>


GuiEditor::GuiEditor(Client* client)
	: client(client),
	inputFieldHello(sf::Vector2f(60.f, 281.f), "Graphics\\Frames\\Hello\\InputFrameHello.png", font, L"Введи своё имя...", 394, window),
	inputFieldMaximumMain(sf::Vector2f(294.f, 959.f), "Graphics\\Frames\\Maximum\\InputFrameMain.png", font, L"Написать сообщение...", 1514, window),
	inputFieldMinimalMain(sf::Vector2f(292.f, 723.f), "Graphics\\Frames\\Minimal\\InputFrameMain.png", font, L"Написать сообщение...", 799, window)
{
	ctx.antialiasingLevel = 8;

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	int screenWidth = desktop.width;
	int screenHeight = desktop.height;

	int windowWidth = 600;
	int windowHeight = 400;

	window.create(sf::VideoMode(windowWidth, windowHeight), "GiperbolaBook", sf::Style::None, ctx);

	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	window.setPosition(sf::Vector2i(posX, posY));
	window.setFramerateLimit(60);

	windowWidth = 1200;
	windowHeight = 800;

	lastMinimalWindowPos = sf::Vector2i((desktop.width - windowWidth) / 2,
		(desktop.height - windowHeight) / 2);

	subChatMaximal.setSize(sf::Vector2f(1570.f, 824.f));
	subChatMaximal.setPosition(sf::Vector2f(313.f, 99.f));
	subChatMaximal.setFillColor(sf::Color::Transparent);
	subChatMaximal.setOutlineColor(sf::Color::White);
	subChatMaximal.setOutlineThickness(2.f);

	subChatMinimal.setSize(sf::Vector2f(850.f, 586.f));
	subChatMinimal.setPosition(sf::Vector2f(313.f, 99.f));
	subChatMinimal.setFillColor(sf::Color::Transparent);
	subChatMinimal.setOutlineColor(sf::Color::White);
	subChatMinimal.setOutlineThickness(2.f);
}

void GuiEditor::init()
{
	font.loadFromFile("Graphics\\Fonts\\Inter\\Inter-Regular.otf");

	EmblemTexture.loadFromFile("Graphics\\Emblem\\Emblem.png");

	WindowFrameHelloTexture.loadFromFile("Graphics\\Frames\\Hello\\Window.png");
	HelloFrameTexture.loadFromFile("Graphics\\Frames\\Hello\\HelloFrame.png");

	WindowFrameMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\Window.png");
	StartChatFrameMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\StartChatFrame.png");
	ChatFrameMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\ChatFrame.png");
	UsersFrameMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\UsersFrame.png");
	UserFrameTopMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\UserFrameTop.png");
	UserFrameLowerMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\UserFrameLower.png");
	DragAndDropMaximumTexture.loadFromFile("Graphics\\Frames\\Maximum\\DragAndDrop.png");

	WindowFrameMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\Window.png");
	StartChatFrameMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\StartChatFrame.png");
	ChatFrameMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\ChatFrame.png");
	UsersFrameMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\UsersFrame.png");
	UserFrameTopMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\UserFrameTop.png");
	UserFrameLowerMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\UserFrameLower.png");
	DragAndDropMinimalTexture.loadFromFile("Graphics\\Frames\\Minimal\\DragAndDrop.png");

	EmblemSprite.setTexture(EmblemTexture);

	WindowFrameHelloSprite.setTexture(WindowFrameHelloTexture);
	HelloFrameSprite.setTexture(HelloFrameTexture);

	WindowFrameMaximumSprite.setTexture(WindowFrameMaximumTexture);
	StartChatFrameMaximumSprite.setTexture(StartChatFrameMaximumTexture);
	ChatFrameMaximumSprite.setTexture(ChatFrameMaximumTexture);
	UsersFrameMaximumSprite.setTexture(UsersFrameMaximumTexture);
	UserFrameTopMaximumSprite.setTexture(UserFrameTopMaximumTexture);
	UserFrameLowerMaximumSprite.setTexture(UserFrameLowerMaximumTexture);
	DragAndDropMaximumSprite.setTexture(DragAndDropMaximumTexture);

	WindowFrameMinimalSprite.setTexture(WindowFrameMinimalTexture);
	StartChatFrameMinimalSprite.setTexture(StartChatFrameMinimalTexture);
	ChatFrameMinimalSprite.setTexture(ChatFrameMinimalTexture);
	UsersFrameMinimalSprite.setTexture(UsersFrameMinimalTexture);
	UserFrameTopMinimalSprite.setTexture(UserFrameTopMinimalTexture);
	UserFrameLowerMinimalSprite.setTexture(UserFrameLowerMinimalTexture);
	DragAndDropMinimalSprite.setTexture(DragAndDropMinimalTexture);

	EmblemSprite.setPosition(sf::Vector2f(15.f, 0.f));

	WindowFrameHelloSprite.setPosition(sf::Vector2f(0.f, 0.f));
	HelloFrameSprite.setPosition(sf::Vector2f(13.f, 39.f));

	WindowFrameMaximumSprite.setPosition(sf::Vector2f(0.f, 0.f));
	StartChatFrameMaximumSprite.setPosition(sf::Vector2f(279.f, 41.f));
	ChatFrameMaximumSprite.setPosition(sf::Vector2f(283.f, 41.f));
	UsersFrameMaximumSprite.setPosition(sf::Vector2f(15.f, 41.f));
	UserFrameTopMaximumSprite.setPosition(sf::Vector2f(285.f, 43.f));
	UserFrameLowerMaximumSprite.setPosition(sf::Vector2f(285.f, 939.f));
	DragAndDropMaximumSprite.setPosition(sf::Vector2f(279.f, 41.f));

	WindowFrameMinimalSprite.setPosition(sf::Vector2f(0.f, 0.f));
	StartChatFrameMinimalSprite.setPosition(sf::Vector2f(275.f, 41.f));
	ChatFrameMinimalSprite.setPosition(sf::Vector2f(275.f, 41.f));
	UsersFrameMinimalSprite.setPosition(sf::Vector2f(15.f, 41.f));
	UserFrameTopMinimalSprite.setPosition(sf::Vector2f(281.f, 43.f));
	UserFrameLowerMinimalSprite.setPosition(sf::Vector2f(281.f, 701.f));
	DragAndDropMinimalSprite.setPosition(sf::Vector2f(275.f, 41.f));

	nameUser.setFont(font);
	nameUser.setFillColor(sf::Color::White);
	nameUser.setCharacterSize(14);
	nameUser.setPosition(sf::Vector2f(299.f, 57.f));

	createButtons();
}

void GuiEditor::createButtons()
{
	// Close
	buttonsWindowPanel.emplace_back(
		static_cast<size_t>(CategoryButton::Close),
		sf::Vector2f(545.f, 1.f),
		"Graphics\\Buttons\\Window\\Close\\CloseNormal.png",
		"Graphics\\Buttons\\Window\\Close\\CloseHovered.png",
		"Graphics\\Buttons\\Window\\Close\\ClosePressed.png",
		L"",
		font
	);

	// Unwrap
	buttonsWindowPanel.emplace_back(
		static_cast<size_t>(CategoryButton::Unwrap),
		sf::Vector2f(491.f, 1.f),
		"Graphics\\Buttons\\Window\\Unwrap\\Expanded\\UnwrapNormalExpanded.png",
		"Graphics\\Buttons\\Window\\Unwrap\\Expanded\\UnwrapHoveredExpanded.png",
		"Graphics\\Buttons\\Window\\Unwrap\\Expanded\\UnwrapPressedExpanded.png",
		L"",
		font
	);

	// RollUp
	buttonsWindowPanel.emplace_back(
		static_cast<size_t>(CategoryButton::RollUp),
		sf::Vector2f(437.f, 1.f),
		"Graphics\\Buttons\\Window\\RollUp\\RollUpNormal.png",
		"Graphics\\Buttons\\Window\\RollUp\\RollUpHovered.png",
		"Graphics\\Buttons\\Window\\RollUp\\RollUpPressed.png",
		L"",
		font
	);

	// Send
	buttons.emplace_back(
		static_cast<size_t>(CategoryButton::Send),
		sf::Vector2f(490.f, 288.f),
		"Graphics\\Buttons\\Send\\Send.png",
		"Graphics\\Buttons\\Send\\Send.png",
		"Graphics\\Buttons\\Send\\Send.png",
		L"",
		font
	);

	// CloseUser
	buttons.emplace_back(
		static_cast<size_t>(CategoryButton::CloseUser),
		sf::Vector2f(1867.f, 54.f),
		"Graphics\\Buttons\\CloseUser\\CloseUserNormal.png",
		"Graphics\\Buttons\\CloseUser\\CloseUserHovered.png",
		"Graphics\\Buttons\\CloseUser\\CloseUserPressed.png",
		L"",
		font
	);

	// Clip
	buttons.emplace_back(
		static_cast<size_t>(CategoryButton::Clip),
		sf::Vector2f(302.f, 971.f),
		"Graphics\\Buttons\\Clip\\Clip.png",
		"Graphics\\Buttons\\Clip\\Clip.png",
		"Graphics\\Buttons\\Clip\\Clip.png",
		L"",
		font
	);
}

void GuiEditor::start()
{
	init();

	while (window.isOpen()) {
		if (run) {
			if (client->currentChooseUser == nullptr) {
				isStart = true;
			}
			else {
				isStart = false;
			}
			dragAndDrop();
			updateUsersFrames();
			processEvents();
			render();
		}
		else {
			processEventsHello();
			renderHello();
		}
	}
}

void GuiEditor::processEvents()
{
	sf::Event event;
	static sf::Clock clickClock;
	static bool firstClick = false;

	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		bool click = false;
		for (auto& button : buttonsWindowPanel) {
			if (button.cursor()) {
				click = true;
				break;
			}
		}

		if (!click) {
			sf::Vector2i mousePos = sf::Mouse::getPosition();
			sf::Vector2i windowPos = window.getPosition();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				if (mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + 33) {
					if (firstClick && clickClock.getElapsedTime().asMilliseconds() < 300) {
						unwrap();
						firstClick = false;
					}
					else {
						if (!isMaximum) {
							isDraggingMinimal = true;
							dragOffsetMinimal = mousePos - windowPos;
						}
						firstClick = true;
						clickClock.restart();
					}
				}
			}

			if (!isMaximum) {
				if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
					isDraggingMinimal = false;
				}

				if (event.type == sf::Event::MouseMoved && isDraggingMinimal) {
					window.setPosition(mousePos - dragOffsetMinimal);
					lastMinimalWindowPos = window.getPosition();
				}
			}
		}

		for (auto& button : buttonsWindowPanel) {
			button.handleEvent(event, window);
			if (button.clicking()) {
				// Close
				if (button.getCategory() == static_cast<int>(CategoryButton::Close)) {
					window.close();
				}
				// Unwrap
				else if (button.getCategory() == static_cast<int>(CategoryButton::Unwrap)) {
					unwrap();
				}
				// RollUp
				else if (button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
					ShowWindow(window.getSystemHandle(), SW_MINIMIZE);
				}
			}
		}

		for (auto& button : buttons) {
			if (isStart) {
				if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
					continue;
				}
			}

			button.handleEvent(event, window);
			// Send
			if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
				button.updateCursor(window);
			}
			// Clip
			if (button.getCategory() == static_cast<int>(CategoryButton::Clip)) {
				button.updateCursor(window);
			}

			if (button.clicking()) {
				// Send
				if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
					if (isMaximum) {
						client->sendMessage(wstring_to_utf8(inputFieldMaximumMain.getText()));
						inputFieldMaximumMain.resetSubmission();
					}
					else {
						client->sendMessage(wstring_to_utf8(inputFieldMinimalMain.getText()));
						inputFieldMinimalMain.resetSubmission();
					}
				}
				// CloseUser
				if (button.getCategory() == static_cast<int>(CategoryButton::CloseUser)) {
					client->currentChooseUser = nullptr;
					for (auto& usersFramesButton : usersFramesButtons) {
						usersFramesButton->resetSelection();
					}
				}
				// Clip
				if (button.getCategory() == static_cast<int>(CategoryButton::Clip)) {
					auto selectedFile = openFileDialog();
					if (selectedFile.has_value()) {
						std::wcout << L"Выбран файл: " << selectedFile.value().wstring() << std::endl;
						client->sendFile(pathsDragAndDrop, selectedFile.value().string());
					}
					else {
						std::wcout << L"Файл не был выбран." << std::endl;
					}
				}
			}
		}

		if (!isStart) {
			if (isMaximum) {
				inputFieldMaximumMain.handleEvent(event, window);
				if (inputFieldMaximumMain.isSubmitted()) {
					client->sendMessage(wstring_to_utf8(inputFieldMaximumMain.getText()));
					inputFieldMaximumMain.resetSubmission();
				}
			}
			else {
				inputFieldMinimalMain.handleEvent(event, window);
				if (inputFieldMinimalMain.isSubmitted()) {
					client->sendMessage(wstring_to_utf8(inputFieldMinimalMain.getText()));
					inputFieldMinimalMain.resetSubmission();
				}
			}
		}

		for (auto& userFrames : usersFrames) {
			if (userFrames.radioButton->isSelected()) {
				if (isMaximum) {
					userFrames.chat->handleEvent(event, ChatFrameMaximumSprite, subChatMaximal, window);
				}
				else {
					userFrames.chat->handleEvent(event, ChatFrameMinimalSprite, subChatMinimal, window);
				}
			}
		}
	}

	for (auto& userFrames : usersFrames) {
		userFrames.radioButton->handleEvent(event, window);
		if (userFrames.radioButton->clicking()) {
			client->chooseUser(userFrames.chat->client_name, userFrames.chat->client_ip, userFrames.chat->client_port);
			nameUser.setString(utf8_to_wstring(client->currentChooseUser->name));
		}
	}
}

void GuiEditor::render()
{
	window.clear(sf::Color(24, 24, 24));

	if (isMaximum) {
		window.draw(WindowFrameMaximumSprite);
		window.draw(UsersFrameMaximumSprite);
		if (!isStart) {
			window.draw(ChatFrameMaximumSprite);
		}
		else {
			window.draw(StartChatFrameMaximumSprite);
		}
	}
	else {
		window.draw(WindowFrameMinimalSprite);
		window.draw(UsersFrameMinimalSprite);
		if (!isStart) {
			window.draw(ChatFrameMinimalSprite);
		}
		else {
			window.draw(StartChatFrameMinimalSprite);
		}
	}

	if (!isStart) {
		for (auto& userFrames : usersFrames) {
			if (userFrames.radioButton->isSelected()) {
				if (isMaximum) {
					userFrames.chat->render(window);
				}
				else {
					userFrames.chat->render(window);
				};
			}
		}
	}

	if (isMaximum) {
		if (!isStart) {
			window.draw(UserFrameTopMaximumSprite);
			window.draw(UserFrameLowerMaximumSprite);
		}
	}
	else {
		if (!isStart) {
			window.draw(UserFrameTopMinimalSprite);
			window.draw(UserFrameLowerMinimalSprite);
		}
	}

	// window.draw(EmblemSprite);

	if (!isStart) {
		if (isMaximum) {
			inputFieldMaximumMain.render(window);
		}
		else {
			inputFieldMinimalMain.render(window);
		}
	}

	if (!isStart) {
		window.draw(nameUser);
	}

	for (auto& buttonWindowPanel : buttonsWindowPanel) {
		buttonWindowPanel.render(window);
	}

	for (auto& button : buttons) {
		if (isStart) {
			if (button.getCategory() == static_cast<int>(CategoryButton::Send) ||
				button.getCategory() == static_cast<int>(CategoryButton::CloseUser) ||
				button.getCategory() == static_cast<int>(CategoryButton::Clip)) {
				continue;
			}
		}
		button.render(window);
	}

	for (auto& userFrames : usersFrames) {
		userFrames.radioButton->render(window);
	}

	if (isDragAndDrop) {
		if (isMaximum) {
			window.draw(DragAndDropMaximumSprite);
		}
		else {
			window.draw(DragAndDropMinimalSprite);
		}
	}

	/*if (isMaximum) {
		window.draw(subChatMaximal);
	}
	else {
		window.draw(subChatMinimal);
	}*/

	window.display();
}

void GuiEditor::processEventsHello()
{
	sf::Event event;
	while (window.pollEvent(event) && !run) {
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		bool click = false;
		for (auto& button : buttonsWindowPanel) {
			if (button.cursor()) {
				click = true;
				break;
			}
		}

		if (!click) {
			sf::Vector2i mousePos = sf::Mouse::getPosition();
			sf::Vector2i windowPos = window.getPosition();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				if (mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + 33) {
					isDraggingHello = true;
					dragOffsetHello = mousePos - windowPos;
				}
			}

			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				isDraggingHello = false;
			}

			if (event.type == sf::Event::MouseMoved && isDraggingHello) {
				window.setPosition(mousePos - dragOffsetHello);
			}
		}

		for (auto& button : buttonsWindowPanel) {
			button.handleEvent(event, window);
			if (button.clicking()) {
				// Close
				if (button.getCategory() == static_cast<int>(CategoryButton::Close)) {
					window.close();
				}
				// Unwrap
				else if (button.getCategory() == static_cast<int>(CategoryButton::Unwrap)) {

				}
				// RollUp
				else if (button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
					ShowWindow(window.getSystemHandle(), SW_MINIMIZE);
				}
			}
		}

		for (auto& button : buttons) {
			button.handleEvent(event, window);
			// Send
			if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
				button.updateCursor(window);
			}

			if (button.clicking()) {
				// Send
				if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
					runMainWindow();
				}
			}
		}

		inputFieldHello.handleEvent(event, window);
		if (inputFieldHello.isSubmitted()) {
			runMainWindow();
		}
	}
}

void GuiEditor::runMainWindow()
{
	std::wstring user_name = inputFieldHello.getText();
	if (user_name == L"") {
		return;
	}

	client->name = wstring_to_utf8(user_name);
	run = true;
	client->running = true;

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	window.create(sf::VideoMode(width, height), "GiperbolaBook", sf::Style::None, ctx);
	window.setPosition(sf::Vector2i(rect.left, rect.top));
	window.setFramerateLimit(60);

	OleInitialize(nullptr);
	HWND hwnd = window.getSystemHandle();
	DropTarget* dropTarget = new DropTarget(hwnd);
	RegisterDragDrop(hwnd, dropTarget);

	for (auto& button : buttonsWindowPanel) {
		// Close
		if (button.getCategory() == static_cast<int>(CategoryButton::Close)) {
			button.setPosition(sf::Vector2f(1866.0f, 0.0f));
		}
		// Unwrap
		else if (button.getCategory() == static_cast<int>(CategoryButton::Unwrap)) {
			button.setTexture(0, "Graphics\\Buttons\\Window\\Unwrap\\Full\\UnwrapNormalFull.png");
			button.setTexture(1, "Graphics\\Buttons\\Window\\Unwrap\\Full\\UnwrapHoveredFull.png");
			button.setTexture(2, "Graphics\\Buttons\\Window\\Unwrap\\Full\\UnwrapPressedFull.png");
			button.setPosition(sf::Vector2f(1812.0f, 0.0f));
		}
		// RollUp
		else if (button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
			button.setPosition(sf::Vector2f(1758.0f, 0.0f));
		}
	}

	for (auto& button : buttons) {
		// Send
		if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
			button.setPosition(sf::Vector2f(1844.0f, 966.0f));
		}
	}
}

void GuiEditor::renderHello()
{
	window.clear(sf::Color(24, 24, 24));

	window.draw(WindowFrameHelloSprite);
	window.draw(HelloFrameSprite);
	inputFieldHello.render(window);

	for (auto& buttonWindowPanel : buttonsWindowPanel) {
		buttonWindowPanel.render(window);
	}

	for (auto& button : buttons) {
		if (button.getCategory() == static_cast<int>(CategoryButton::CloseUser)) {
			continue;
		}
		button.render(window);
	}

	window.display();
}

void GuiEditor::unwrap()
{
	if (isMaximum) {
		ctx.antialiasingLevel = 8;

		int windowWidth = 1200;
		int windowHeight = 800;

		window.create(sf::VideoMode(windowWidth, windowHeight), "GiperbolaBook", sf::Style::None, ctx);
		window.setPosition(lastMinimalWindowPos);
		window.setFramerateLimit(60);

		OleInitialize(nullptr);
		HWND hwnd = window.getSystemHandle();
		DropTarget* dropTarget = new DropTarget(hwnd);
		RegisterDragDrop(hwnd, dropTarget);

		for (auto& button : buttonsWindowPanel) {
			// Close
			if (button.getCategory() == static_cast<int>(CategoryButton::Close)) {
				button.setPosition(sf::Vector2f(1145.0f, 1.0f));
			}
			// Unwrap
			else if (button.getCategory() == static_cast<int>(CategoryButton::Unwrap)) {
				button.setTexture(0, "Graphics\\Buttons\\Window\\Unwrap\\Expanded\\UnwrapNormalExpanded.png");
				button.setTexture(1, "Graphics\\Buttons\\Window\\Unwrap\\Expanded\\UnwrapHoveredExpanded.png");
				button.setTexture(2, "Graphics\\Buttons\\Window\\Unwrap\\Expanded\\UnwrapPressedExpanded.png");
				button.setPosition(sf::Vector2f(1091.0f, 1.0f));
			}
			// RollUp
			else if (button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
				button.setPosition(sf::Vector2f(1037.0f, 1.0f));
			}
		}

		for (auto& button : buttons) {
			// Send
			if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
				button.setPosition(sf::Vector2f(1124.0f, 728.0f));
			}
			// CloseUser
			if (button.getCategory() == static_cast<int>(CategoryButton::CloseUser)) {
				button.setPosition(sf::Vector2f(1147.0f, 54.0f));
			}
			// Clip
			if (button.getCategory() == static_cast<int>(CategoryButton::Clip)) {
				button.setPosition(sf::Vector2f(302.0f, 733.0f));
			}
		}
	}

	else {
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		window.create(sf::VideoMode(width, height), "GiperbolaBook", sf::Style::None, ctx);
		window.setPosition(sf::Vector2i(rect.left, rect.top));
		window.setFramerateLimit(60);

		OleInitialize(nullptr);
		HWND hwnd = window.getSystemHandle();
		DropTarget* dropTarget = new DropTarget(hwnd);
		RegisterDragDrop(hwnd, dropTarget);

		for (auto& button : buttonsWindowPanel) {
			// Close
			if (button.getCategory() == static_cast<int>(CategoryButton::Close)) {
				button.setPosition(sf::Vector2f(1866.0f, 0.0f));
			}
			// UnwrapFull
			else if (button.getCategory() == static_cast<int>(CategoryButton::Unwrap)) {
				button.setTexture(0, "Graphics\\Buttons\\Window\\Unwrap\\Full\\UnwrapNormalFull.png");
				button.setTexture(1, "Graphics\\Buttons\\Window\\Unwrap\\Full\\UnwrapHoveredFull.png");
				button.setTexture(2, "Graphics\\Buttons\\Window\\Unwrap\\Full\\UnwrapPressedFull.png");
				button.setPosition(sf::Vector2f(1812.0f, 0.0f));
			}
			// RollUp
			else if (button.getCategory() == static_cast<int>(CategoryButton::RollUp)) {
				button.setPosition(sf::Vector2f(1758.0f, 0.0f));
			}
		}

		for (auto& button : buttons) {
			// Send
			if (button.getCategory() == static_cast<int>(CategoryButton::Send)) {
				button.setPosition(sf::Vector2f(1844.0f, 966.0f));
			}
			// CloseUser
			if (button.getCategory() == static_cast<int>(CategoryButton::CloseUser)) {
				button.setPosition(sf::Vector2f(1867.0f, 54.0f));
			}
			// Clip
			if (button.getCategory() == static_cast<int>(CategoryButton::Clip)) {
				button.setPosition(sf::Vector2f(302.0f, 971.0f));
			}
		}
	}

	isMaximum = !isMaximum;
	for (auto& chat_ptr : chats) {
		if (isMaximum) {
			chat_ptr->changeWindow(isMaximum, subChatMaximal, window);
		}
		else {
			chat_ptr->changeWindow(isMaximum, subChatMinimal, window);
		}
	}
}

void GuiEditor::updateUsersFrames()
{
	if (client->updateUsers) {
		const float startX = 25.0f;
		const float startY = 109.0f;
		const float spacing = 14.0f;
		const float frameHeight = 36.0f;

		static std::vector<ClientEntry> previousUsers;
		const auto& newUsers = client->clients;
		client->updateUsers = false;

		for (auto it = previousUsers.begin(); it != previousUsers.end();) {
			auto found = std::find_if(newUsers.begin(), newUsers.end(), [&](const ClientEntry& user) {
				return user.ip == it->ip && user.port == it->port;
				});

			if (found == newUsers.end()) {
				auto frameIt = std::find_if(usersFrames.begin(), usersFrames.end(), [&](const User& frame) {
					return frame.chat->client_ip == it->ip && frame.chat->client_port == it->port;
					});
				if (frameIt != usersFrames.end()) {
					delete frameIt->radioButton;
					usersFrames.erase(frameIt);
				}

				auto chatIt = std::find_if(chats.begin(), chats.end(), [&](const std::shared_ptr<Chat>& chat) {
					return chat->client_ip == it->ip && chat->client_port == it->port;
					});
				if (chatIt != chats.end()) {
					chats.erase(chatIt);
				}

				it = previousUsers.erase(it);
			}
			else {
				++it;
			}
		}

		std::vector<ClientEntry> sortedUsers;
		ClientEntry* selfUser = nullptr;

		for (const auto& user : newUsers) {
			if (client->name == user.name && client->clientIp == user.ip && client->clientPort == user.port) {
				selfUser = new ClientEntry(user);
			}
			else {
				sortedUsers.push_back(user);
			}
		}

		if (selfUser) {
			sortedUsers.insert(sortedUsers.begin(), *selfUser);
			delete selfUser;
		}

		for (size_t i = 0; i < sortedUsers.size(); ++i) {
			bool userExists = std::any_of(usersFrames.begin(), usersFrames.end(), [&](const User& frame) {
				return frame.radioButton->getText() == utf8_to_wstring(sortedUsers[i].name);
				});

			if (!userExists) {
				std::wstring client_name;
				const size_t maxNameLength = 21;
				client_name = utf8_to_wstring(sortedUsers[i].name);
				if (client_name.length() > maxNameLength) {
					client_name = client_name.substr(0, maxNameLength) + L"...";
				}

				if (i == 0) {
					client_name = L"Ты (" + client_name + L")";
				}

				float yPos = startY + i * (frameHeight + spacing);

				auto chatIt = std::find_if(chats.begin(), chats.end(), [&](const std::shared_ptr<Chat>& chat) {
					return chat->client_ip == sortedUsers[i].ip && chat->client_port == sortedUsers[i].port;
					});

				std::shared_ptr<Chat> chatPtr;
				if (chatIt == chats.end()) {
					if (isMaximum) {
						chatPtr = std::make_shared<Chat>(sortedUsers[i].name, sortedUsers[i].ip, sortedUsers[i].port, subChatMaximal, font, window);
					}
					else {
						chatPtr = std::make_shared<Chat>(sortedUsers[i].name, sortedUsers[i].ip, sortedUsers[i].port, subChatMinimal, font, window);
					}
					chats.push_back(chatPtr);
				}
				else {
					chatPtr = *chatIt;
				}

				auto* newButton = new RadioButton(
					usersFramesButtons,
					static_cast<size_t>(CategoryButton::User),
					sf::Vector2f(startX, yPos),
					"Graphics\\Buttons\\Users\\UserFrameNormal.png",
					"Graphics\\Buttons\\Users\\UserFrameHovered.png",
					"Graphics\\Buttons\\Users\\UserFramePressed.png",
					client_name,
					font,
					14,
					sf::Color::White,
					sf::Color::White,
					sf::Color::White,
					sf::Vector2f(24.f, 10.f)
				);

				usersFramesButtons.emplace_back(newButton);
				usersFrames.emplace_back(newButton, chatPtr.get());
			}
		}

		previousUsers = newUsers;
	}
}

void GuiEditor::addMessage(const std::string& client_ip, int& client_port, bool isMyMessage, const std::string& message, const std::wstring& time, bool isFile)
{
	for (auto& chat_ptr : chats) {
		Chat& chat = *chat_ptr;

		if (chat.client_ip == client_ip && chat.client_port == client_port) {
			if (isMaximum) {
				if (!isFile) {
					chat.addMessage(isMyMessage, utf8_to_wstring(message), time, font, subChatMaximal);
				}
				else {
					chat.addMessageFile(isMyMessage, message, time, font, subChatMaximal);
				}
			}
			else {
				if (!isFile) {
					chat.addMessage(isMyMessage, utf8_to_wstring(message), time, font, subChatMinimal);
				}
				else {
					chat.addMessageFile(isMyMessage, message, time, font, subChatMinimal);
				}
			}
		}
	}
}

void GuiEditor::dragAndDrop()
{
	client->sendFile(pathsDragAndDrop);

	if (!pathsDragAndDrop.empty()) {
		pathsDragAndDrop.clear();
	}
}

std::optional<std::filesystem::path> GuiEditor::openFileDialog()
{
	wchar_t filePath[MAX_PATH] = { 0 };

	OPENFILENAMEW ofn = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Все файлы\0*.*\0Текстовые файлы\0*.TXT\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	ofn.lpstrTitle = L"Выберите файл";

	if (GetOpenFileNameW(&ofn)) {
		return std::filesystem::path(filePath);
	}
	return std::nullopt;
}