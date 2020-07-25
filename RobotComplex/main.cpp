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

sf::Texture* groundTexture;
std::vector<sf::Texture*> itemTextures;
std::unordered_map<uint8_t, sf::Texture*> logicTextures;
std::vector<sf::Texture*> animationTextures;
sf::Texture* robotTexture = LoadTexture("robotNew.png");
sf::Texture* font = LoadTexture("font.png");

//sf::Texture* LogicTile::texture = LoadTexture("blank.png");
//sf::Texture* Wire::texture = LoadTexture("logic/wire.png");
//sf::Texture* PressurePlate::texture = LoadTexture("logic/pressureplate.png");
//sf::Texture* Redirector::texture = LoadTexture("logic/redirector.png");
//sf::Texture* Inverter::texture = LoadTexture("logic/inverter.png");
//sf::Texture* Booster::texture = LoadTexture("logic/booster.png");
//sf::Texture* Repeater::texture = LoadTexture("logic/inverter.png");
//sf::Texture* Comparer::texture = LoadTexture("logic/comparer.png");
//sf::Texture* Gate::texture = LoadTexture("logic/holder.png");
//sf::Texture* Counter::texture = LoadTexture("logic/counter.png");
//sf::Texture* Belt::texture = LoadTexture("logic/belt.png");
//sf::Texture* PlusOne::texture = LoadTexture("logic/plusone.png");
//sf::Texture* Shover::texture = LoadTexture("logic/shover.png");
//sf::Texture* Toggle::texture = LoadTexture("logic/inverter.png");
sf::Image icon;

std::vector<sf::Event> eventHistory;

ProgramData program;
WorldSave world;
WorldSave test;
WidgetCreator* creator;
bool maximize = true;
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
	std::string font = "assets/DejaVuSans.ttf";
	defaultFont = agui::Font::load(font, 16);
	program.guiFont.loadFromFile(font);
	//Setting a global font is required and failure to do so will crash.
	agui::Widget::setGlobalFont(defaultFont);
	//Set icon image
	assert(icon.loadFromFile("assets/robotIcon.png"));
}

void UpdateWorld()
{
	while (program.running) {
		clock_t beginUpdate = clock();
		program.worldMutex.lock();
		if (!program.gamePaused)
		{
			if (creator)
				creator->PerformActions();
			if (world.tick % (GC::FRAMERATE / GC::UPDATERATE) == 0)
			{
				program.framesSinceTick = 0;
				program.UpdateMap();
			}
			world.tick++;
		}
		program.worldMutex.unlock();
		clock_t updateTime = clock();
		int padTime = int(CLOCKS_PER_SEC / float(GC::FRAMERATE)) + beginUpdate - updateTime;
		if (padTime > 0)
			Sleep(padTime);
		clock_t endUpdate = clock();
		program.updateRate = (program.updateRate + float(CLOCKS_PER_SEC) / float(endUpdate - beginUpdate)) * 0.5;
	}
}

void ResizeWindow(sf::RenderWindow& window, bool windowFullScreen, bool recreateWindow)
{
	if (maximize || recreateWindow && !windowFullScreen)
	{
		sf::WindowHandle windowHandle = window.getSystemHandle();
		ShowWindow(windowHandle, SW_MAXIMIZE);
	}

	agui::Dimension displaySize;
	if (auto display = gui->getGraphics())
	{
		displaySize = gui->getGraphics()->getDisplaySize();
	}

	if (program.windowWidth != displaySize.getWidth() || program.windowHeight != displaySize.getHeight())
	{
		// Find the max window size for when going from fullscreen to windowed
		if (displaySize.getWidth() > program.windowedWidth)
			program.windowedWidth = float(displaySize.getWidth());
		if (displaySize.getHeight() > program.windowedHeight)
			program.windowedHeight = float(displaySize.getHeight());

		program.windowWidth = float(displaySize.getWidth());
		program.windowHeight = float(displaySize.getHeight());
		program.halfWindowWidth = program.windowWidth / 2;
		program.halfWindowHeight = program.windowHeight / 2;
		window.setActive(false);
		window.create(sf::VideoMode(int(program.windowWidth), int(program.windowHeight)), "RoboFactory", sf::Style::Default);
		window.setActive(true);
		if (program.windowedWidth == program.windowWidth && program.windowedHeight == program.windowHeight)
			maximize = true;
	}
	if (recreateWindow && !windowFullScreen)
	{
		program.windowWidth = program.windowedWidth;
		program.windowHeight = program.windowedHeight;
		program.halfWindowWidth = program.windowWidth / 2;
		program.halfWindowHeight = program.windowHeight / 2;
		window.setActive(false);
		window.create(sf::VideoMode(int(program.windowWidth), int(program.windowHeight)), "RoboFactory", sf::Style::Default);
		window.setActive(true);
		maximize = true;
	}
	if (windowFullScreen)
	{
		window.setActive(false);
		window.create(sf::VideoMode(int(sf::VideoMode::getDesktopMode().width), int(sf::VideoMode::getDesktopMode().height)), "RoboFactory", sf::Style::Fullscreen);
		window.setActive(true);
		program.windowWidth = float(sf::VideoMode::getDesktopMode().width);
		program.windowHeight = float(sf::VideoMode::getDesktopMode().height);
		program.halfWindowWidth = program.windowWidth / 2;
		program.halfWindowHeight = program.windowHeight / 2;
	}		

	program.worldView = sf::View();
	program.worldView.setSize(sf::Vector2f(program.windowWidth, program.windowHeight));
	program.worldView.setCenter(0, 0);

	program.hudView = sf::View();
	program.hudView.setSize(sf::Vector2f(program.windowWidth, program.windowHeight));
	program.hudView.setCenter(0, 0);

	// Re add the icon to the window
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	program.redrawGround = true;

	// Copy the gui stack (state) before deleting and reinitializing it

	if (creator)
		creator->~WidgetCreator();

	creator = new WidgetCreator(gui, &window);
	if (!program.gamePaused) {
		creator->guiStack.pop();
	}
	
	window.setView(program.worldView);
	window.setFramerateLimit(GC::FRAMERATE);

	if (recreateWindow || maximize)
	{
		sf::WindowHandle windowHandle = window.getSystemHandle();
		ShowWindow(windowHandle, SW_MAXIMIZE);
		maximize = false;
	}
}

int main()
{
	sf::Thread MapUpdate(&UpdateWorld);

	// Generate the window
	program.windowWidth = 1366;
	program.windowHeight = 748;	

	// Load program
	LoadPrototypes();
	LoadLogicToHotbar();
	world = WorldSave();
	CreateTestWorld2();
	program.gamePaused = true;
	bool windowFullScreen = false;

	// Load Gui
	sf::RenderWindow window(sf::VideoMode(int(program.windowWidth), int(program.windowHeight)), "RoboFactory");
	window.setActive(true);
	sf::WindowHandle windowHandle = window.getSystemHandle();
	ShowWindow(windowHandle, SW_MAXIMIZE);
	maximize = true;
	initializeAgui(window);
	
	program.running = true;
	MapUpdate.launch();

	while (window.isOpen() && program.running)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (!program.running)
				goto shutdown;
			if (event.type == sf::Event::Closed)
			{
				program.running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				ResizeWindow(window, windowFullScreen,false);
				// adjust the viewport when the window is resized
				gui->resizeToDisplay();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::F11)
				{
					windowFullScreen = !windowFullScreen;
					ResizeWindow(window, windowFullScreen,true);
				}
				// Stepping back should always be considered as input
				else if (event.key.code == sf::Keyboard::Escape)
				{
					if (!creator->guiStack.empty()) {
						creator->guiStack.pop();
					}
					else {
						creator->guiStack.push(&creator->mainFrame);
					}
				}
			}
			creator->UserInput(event);
			inputHandler->processEvent(event);
			if (creator->guiStack.empty())
				program.gamePaused = false;
			else
				program.gamePaused = true;
			eventHistory.emplace_back(event);
		}
		creator->SetGuiVisibility();
		window.clear();
		clock_t beginUpdate = clock();
		program.worldMutex.lock();
		if(!program.gamePaused)
			program.framesSinceTick++;
		program.DrawGameState(window);
		program.worldMutex.unlock();
		gui->logic();
		gui->render();

		// These elements will be ontop of the gui
		for (auto element : program.craftingViewBacks)
		{
			window.draw(element.second);
		}
		program.craftingViewSprites.draw(window);
		for (sf::Text sprite : program.textOverlay)
		{
			window.draw(sprite);
		}

		window.display();
		clock_t endUpdate = clock();
		if (program.frameRate == INFINITE)
			program.frameRate = GC::FRAMERATE;
		program.frameRate = (program.frameRate + float(CLOCKS_PER_SEC) / float(endUpdate - beginUpdate)) * 0.5;
	}
shutdown:;
	creator->SaveProgramSettings();
	MapUpdate.wait();
	return 0;
}