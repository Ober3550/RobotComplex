#include <SFML/Graphics.hpp>
#include <iostream>
#include <functional>
#include <string>
#include "Windows.h"
#include <thread>

#include "WorldSave.h"
#include "ProgramData.h"
#include "Pos.h"
#include "LogicTile.h"
#include "MyMod.h"
#include "CraftingProcess.h"
#include "RedirectorColors.h"
#include "SpriteGenerator.h"
#include "TestWorld.h"
#include "PrototypeLoader.h"
#include "GameInput.h"
#include "TitleScreen.h"
#include "WidgetCreator.h"

agui::Gui *gui = NULL;
agui::SFML2Input* inputHandler = NULL;
agui::SFML2Graphics* graphicsHandler = NULL;
agui::Font *defaultFont = NULL;
std::thread worldUpdate;
bool running;

ProgramData program;
WorldSave world;
WorldSave test;
WidgetCreator* creator;
constexpr uint8_t FRAMERATE = 60;

class SimpleButtonListener : public agui::ButtonListener
{
private:
	std::function<void()> buttonAction;
public:
	SimpleButtonListener(std::function<void()> buttonAction) {
		this->buttonAction = buttonAction;
	};
	void buttonStateChanged(agui::Button *, agui::Button::ButtonStateEnum state) {
		if (state == agui::Button::ButtonStateEnum::CLICKED)
		{
			buttonAction();
		}
	}
	void toggleStateChanged(agui::Button *, bool) {}
	void death(agui::Button *) {}
	void isToggleButtonChanged(agui::Button *, bool) {}
	void textAlignmentChanged(agui::Button *, agui::AreaAlignmentEnum) {}
	~SimpleButtonListener(void) {};
};

WidgetCreator::WidgetCreator(agui::Gui *guiInstance)
{
	mGui = guiInstance;

	gui->add(&frame);
	// Options frame
	int frameWidth = 220;
	int frameHeight = 200;
	frame.setSize(frameWidth, frameHeight);
	frame.setLocation((program.windowWidth / 2) - (frameWidth / 2), (program.windowHeight / 2) - (frameHeight / 2));
	frame.setText("Main Menu");
	// Load Button
	loadButton.setSize(frameWidth - 10, 50);
	loadButton.setText("Load Game");
	loadButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldmutex.lock();
		world.Deserialize("TestWorld");
		program.worldmutex.unlock();
	}));
	frame.add(&loadButton);
	loadButton.setLocation(0, 0);
	// Save Button
	saveButton.setSize(frameWidth - 10, 50);
	saveButton.setText("Save Game");
	saveButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldmutex.lock();
		world.Serialize("TestWorld");
		program.worldmutex.unlock();
	}));
	frame.add(&saveButton);
	saveButton.setLocation(0, 50);
	// Exit Button
	exitButton.setSize(frameWidth - 10, 50);
	exitButton.setText("Exit Game");
	exitButton.addButtonListener(new SimpleButtonListener([&] {
		running = false;
	}));
	frame.add(&exitButton);
	exitButton.setLocation(0, 100);
}

void initializeAgui(sf::RenderTarget& target)
{
	//Set the image loader
	agui::Image::setImageLoader(new agui::SFML2ImageLoader());
	//Set the font loader
	agui::Font::setFontLoader(new agui::SFML2FontLoader());
	//Instance the input handler
	inputHandler = new agui::SFML2Input();
	//Instance the graphics handler
	graphicsHandler = new agui::SFML2Graphics(target);
	agui::Color::setPremultiplyAlpha(false);
	//Instance the gui
	gui = new agui::Gui();
	//Set the input handler
	gui->setInput(inputHandler);
	//Set the graphics handler
	gui->setGraphics(graphicsHandler);
	//Set the font
	defaultFont = agui::Font::load("Fonts/DejaVuSans.ttf", 16);
	//Setting a global font is required and failure to do so will crash.
	agui::Widget::setGlobalFont(defaultFont);
}

void addWidgets()
{
	creator = new WidgetCreator(gui);
}

void cleanUp()
{
	gui->getTop()->clear();
	delete creator;
	creator = NULL;
	delete gui;
	gui = NULL;
	delete inputHandler;
	delete graphicsHandler;
	inputHandler = NULL;
	graphicsHandler = NULL;
	delete defaultFont;
	defaultFont = NULL;
}
void UpdateWorld()
{
	while (running) {
		if (!program.gamePaused)
		{
			creator->frame.setVisibility(false);
			clock_t beginUpdate = clock();
			program.worldmutex.lock();
			UpdateMap();
			program.worldmutex.unlock();
			clock_t updateTime = clock();
			int padTime = int(CLOCKS_PER_SEC / float(FRAMERATE)) + beginUpdate - updateTime;
			if (padTime > 0)
				Sleep(padTime);
		}
		else
		{
			creator->frame.setVisibility(true);
		}
	}
}

int main()
{
	sf::Thread MapUpdate(&UpdateWorld);

	// Generate the window
	program.windowHeight = 1010;
	program.windowWidth = 1920;
	sf::RenderWindow window(sf::VideoMode(program.windowWidth, program.windowHeight), "Agui - SFML2 Example");
	sf::View view = window.getView();
	view.setCenter(0, 0);
	window.setView(view);
	window.setFramerateLimit(FRAMERATE);

	// Load program
	LoadPrototypes();
	LoadLogicToHotbar();
	world = WorldSave();
	CreateTestWorld();
	program.gamePaused = true;

	// Load Gui
	initializeAgui(window);
	addWidgets();
	MapUpdate.launch();
	running = true;
	while (window.isOpen() && running)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				gui->resizeToDisplay();
			}
			inputHandler->processEvent(event);
			GameInput(window, event);
		}
		
		window.clear();
		clock_t beginUpdate = clock();
		program.DrawGameState(window);
		
		clock_t endUpdate = clock();
		program.deltaTime += endUpdate - beginUpdate;
		program.frames++;
		if (program.deltaTime > 1000.0)
		{
			program.frameRate = double(program.frames) * 0.5 + program.frameRate * 0.5;
			program.frames = 0;
			program.deltaTime -= CLOCKS_PER_SEC;
		}
		gui->logic();
		gui->render();
		window.display();
	}
	running = false;
	MapUpdate.wait();
	cleanUp();
	return 0;
}