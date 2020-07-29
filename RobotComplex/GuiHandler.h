#pragma once
#include <SFML/Graphics.hpp>
#include "MyMap.h"

// Create a hash function for key events to allow for key modifiers to be considered
template<>
struct std::hash<sf::Event::KeyEvent> {
	std::size_t operator()(const sf::Event::KeyEvent& k) const
	{
		//{ sf::Keyboard::Escape, /*alt*/ true, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
		return (int)k.system << 7 & (int)k.shift << 6 & (int)k.control << 5 & (int)k.alt << 4 & (int)k.code;
	}
};

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
		LoadDefaultKeyMapping();
		CreateActions();
	}
};
extern GuiHandler handler;

