#pragma once
#include <SFML/Graphics.hpp>
#include "MyMap.h"

enum MenuStack {
	noMenu,
	mainMenu,
	saveMenu,
	controlMenu
};

class GuiHandler {
public:
	MenuStack currentMenu = mainMenu;
	bool craftingViewShow = true;
	std::string resultsTitle = "";
	bool populateResults = true;
	bool showDebug = false;
	bool useNormalMovement = false;

	// Window context
	//sf::RenderWindow* window;

	// Input handling
	MyMap<sf::Event::KeyEvent, std::string>   eventToAction;
	MyMap<std::string, sf::Event::KeyEvent>   actionToEvent;
	MyMap<std::string, std::function<void()>> userActions;
	MyMap<std::string, int> heldTick;
	MyMap<std::string, int> actionFrequency;
	std::vector<std::string> userActionOrder;
	std::string actionBindWaiting = "";
	
	void FindRecipes(std::string name);
	void LoadDefaultKeyMapping();
	void CreateActions();
	void SaveProgramSettings();
	bool LoadProgramSettings();
	void PerformActions();
	void MouseMoved();
	void FinishedSelection(Pos start, Pos end);
	void MapNewButton(sf::Event::KeyEvent newButton);
	void HandleInput(sf::Event event, sf::RenderWindow& window);
	void HandleGui(sf::RenderWindow& window);
	GuiHandler() {
		LoadProgramSettings();
		if(eventToAction.size() == 0)
			LoadDefaultKeyMapping();
		for (auto kv : eventToAction)
		{
			actionToEvent.insert({ kv.second,kv.first });
		}
		CreateActions();
	}
	void DrawCraftingViewer();
	void DrawTechnologyViewer();
	void DrawHotbar();
};
extern GuiHandler handler;

