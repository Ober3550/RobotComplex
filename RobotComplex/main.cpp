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
#include "PrototypeLoader.h"
#include "Textures.h"
#include "GuiHandler.h"

#include "imgui.h"
#include "imgui-SFML.h"

std::thread worldUpdate;

sf::Texture* groundTexture;
std::vector<sf::Texture*> groundTextures;
std::vector<sf::Texture*> itemTextures;
std::unordered_map<uint8_t, sf::Texture*> logicTextures;
std::vector<sf::Texture*> animationTextures;
sf::Texture* robotTexture = LoadTexture("robotNew.png");
sf::Texture* font = LoadTexture("font.png");
sf::Texture* blank = LoadTexture("blank.png");

sf::Image icon;

ProgramData program;
WorldSave world;
GuiHandler handler;
//WidgetCreator* creator;
bool maximize = true;
constexpr uint8_t FRAMERATE = 60;
constexpr uint8_t UPDATERATE = 20;

void UpdateWorld()
{
	while (program.running) {
		
		if (program.selectedSave != "")
		{
			clock_t beginUpdate = clock();
			program.worldMutex.lock();
			if(program.acceptGameInput)
				handler.PerformActions();
			if (!program.gamePaused)
			{
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
}

int main()
{
	sf::Clock deltaClock;
	sf::Thread MapUpdate(&UpdateWorld);

	program.windowWidth = 600;
	program.windowHeight = 450;

	// Load program
	LoadPrototypes();
	LoadLogicToHotbar();
	//world = WorldSave();
	handler = GuiHandler();
	program.gamePaused = true;
	bool windowFullScreen = false;

	// Load Gui
	sf::RenderWindow window(sf::VideoMode(int(program.windowWidth), int(program.windowHeight)), "RoboFactory");
	window.setActive(true);
	sf::WindowHandle windowHandle = window.getSystemHandle();
	ShowWindow(windowHandle, SW_MAXIMIZE);
	maximize = true;

	ImGui::SFML::Init(window);
	
	std::string font = "assets/DejaVuSans.ttf";
	program.guiFont.loadFromFile(font);

	program.worldView = sf::View();
	program.hudView = sf::View();

	program.running = true;
	MapUpdate.launch();

	handler.FindRecipes("");

	srand(deltaClock.getElapsedTime().asMicroseconds());
	while (window.isOpen() && program.running)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			handler.HandleInput(event, window);
			if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size = window.getSize();
				program.windowWidth = size.x;
				program.windowHeight = size.y;
				program.halfWindowWidth = program.windowWidth / 2;
				program.halfWindowHeight = program.windowHeight / 2;
				program.worldView.setSize(sf::Vector2f(program.windowWidth, program.windowHeight));
				program.worldView.setCenter(0, 0);
				program.hudView.setSize(sf::Vector2f(program.windowWidth, program.windowHeight));
				program.hudView.setCenter(0, 0);
			}
			else if (event.type == sf::Event::Closed)
			{
				program.running = false;
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::F3)
				{
					program.showDebugInfo = !program.showDebugInfo;
					handler.showDebug = !handler.showDebug;
				}
				// Stepping back should always be considered as input
				else if (event.key.code == sf::Keyboard::Escape)
				{
					handler.currentMenu = (MenuStack)!handler.currentMenu;
					if (handler.currentMenu == noMenu)
						program.gamePaused = false;
					else
						program.gamePaused = true;
				}
			}
		}
		window.clear();
		clock_t beginUpdate = clock();
		program.startUpdate = clock();
		if (program.worldLoaded)
		{
			program.worldMutex.lock();
			program.showingTooltip = false;
			if (!program.gamePaused)
				program.framesSinceTick++;
			program.DrawGameState(window);
			program.worldMutex.unlock();
		}
		ImGui::SFML::Update(window, deltaClock.restart());
		handler.HandleGui(window);
		window.display();
		clock_t midUpdate = clock();
		int padTime = int(CLOCKS_PER_SEC / float(GC::FRAMERATE)) + beginUpdate - midUpdate;
		if (padTime > 0)
			Sleep(padTime);
		clock_t endUpdate = clock();
		if (program.frameRate == INFINITE)
			program.frameRate = GC::FRAMERATE;
		program.frameRate = (program.frameRate + float(CLOCKS_PER_SEC) / float(endUpdate - beginUpdate)) * 0.5;
	}
	//creator->SaveProgramSettings();
	ImGui::SFML::Shutdown();
	MapUpdate.wait();
	handler.SaveProgramSettings();
	return 0;
}