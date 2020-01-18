#include <SFML/Graphics.hpp>
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
#include "TestWorld.h"
#include "PrototypeLoader.h"
#include "TitleScreen.h"
#include "WidgetCreator.h"
#include "Textures.h"

agui::Gui *gui = NULL;
agui::SFML2Input* inputHandler = NULL;
agui::SFML2Graphics* graphicsHandler = NULL;
agui::Font *defaultFont = NULL;
std::thread worldUpdate;

std::vector<sf::Texture*> groundTextures;
std::vector<sf::Texture*> itemTextures;
std::vector<sf::Texture*> animationTextures;
sf::Texture* robotTexture = LoadTexture("robotNew.png");
sf::Texture* font = LoadTexture("font.png");
sf::Texture* buttonTexture = LoadTexture("button.png");

sf::Texture* LogicTile::texture = LoadTexture("blank.png");
sf::Texture* Wire::texture = LoadTexture("logic/wire.png");
sf::Texture* PressurePlate::texture = LoadTexture("logic/pressureplate.png");
sf::Texture* Redirector::texture = LoadTexture("logic/redirector.png");
sf::Texture* Inverter::texture = LoadTexture("logic/inverter.png");
sf::Texture* Booster::texture = LoadTexture("logic/inverter.png");
sf::Texture* Repeater::texture = LoadTexture("logic/inverter.png");
sf::Texture* Holder::texture = LoadTexture("logic/holder.png");
sf::Texture* Counter::texture = LoadTexture("logic/counter.png");

ProgramData program;
WorldSave world;
WorldSave test;
WidgetCreator* creator;
constexpr uint8_t FRAMERATE = 60;
constexpr uint8_t UPDATERATE = 20;

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
	std::string font = "Fonts/DejaVuSans.ttf";
	defaultFont = agui::Font::load(font, 16);
	program.guiFont.loadFromFile(font);
	//Setting a global font is required and failure to do so will crash.
	agui::Widget::setGlobalFont(defaultFont);
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
	while (program.running) {
		clock_t beginUpdate = clock();
		program.worldMutex.lock();
		program.framesSinceTick = 0;
		if (!program.gamePaused)
			program.UpdateMap();
		program.worldMutex.unlock();
		clock_t updateTime = clock();
		int padTime = int(CLOCKS_PER_SEC / float(GC::UPDATERATE)) + beginUpdate - updateTime;
		if (padTime > 0)
			Sleep(padTime);
		clock_t endUpdate = clock();
		program.updateRate = (program.updateRate + float(CLOCKS_PER_SEC) / float(endUpdate - beginUpdate)) * 0.5;
	}
}

int main()
{
	sf::Thread MapUpdate(&UpdateWorld);

	// Generate the window
	program.windowHeight = 1010;
	program.windowWidth = 1920;
	sf::RenderWindow window(sf::VideoMode(program.windowWidth, program.windowHeight), "Terraforma");
	sf::View view = window.getView();
	view.setCenter(0, 0);
	window.setView(view);
	window.setFramerateLimit(GC::FRAMERATE);

	// Load program
	LoadPrototypes();
	LoadLogicToHotbar();
	world = WorldSave();
	CreateTestWorld2();
	program.gamePaused = true;

	// Load Gui
	initializeAgui(window);
	creator = new WidgetCreator(gui, &window);
	MapUpdate.launch();
	program.running = true;
	while (window.isOpen() && program.running)
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
			creator->UserInput(event);
		}
		creator->SetGuiVisibility();
		window.clear();
		clock_t beginUpdate = clock();
		program.worldMutex.lock();
		program.framesSinceTick++;
		program.DrawGameState(window);
		program.worldMutex.unlock();
		gui->logic();
		gui->render();
		window.display();
		clock_t endUpdate = clock();
		program.frameRate = (program.frameRate + float(CLOCKS_PER_SEC) / float(endUpdate - beginUpdate)) * 0.5;
	}
	program.running = false;
	MapUpdate.wait();
	cleanUp();
	return 0;
}