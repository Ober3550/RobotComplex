#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <SFML/Graphics.hpp>

#include "Robot.h"
#include "CraftingClass.h"
#include "MyMap.h"
#include "Animation.h"
#include "LogicTile.h"

class ProgramData {
public:
	MyMap<uint16_t, std::vector<uint16_t>> itemRecipeList;	// Mapping from item type to recipe. Ie what items catalyse particular crafting recipes
	std::vector<CraftingClass> craftingRecipes;				// Vector of crafting recipes
	
	std::vector<std::string> itemTooltips;
	std::vector<std::vector<std::string>> logicTooltips;
	MyMap<char, sf::IntRect> fontMap;	// Map that relates characters to sections of the font texture
	sf::Font guiFont;
	std::vector<Animation> animations;

	//Stores sprite layers
	bool redrawStatic = true;
	bool redrawDynamic = true;				// Redraws items, logic, robots, animations
	std::vector<sf::Sprite> groundSprites;	// Ground sprites are only repopulated when the camera moves
	std::vector<sf::Sprite> itemSprites;
	std::vector<sf::Sprite> logicSprites;
	std::vector<sf::Sprite> robotSprites;
	std::vector<sf::Sprite> animationSprites;
	std::vector<sf::RectangleShape> hotbarSlots;
	std::vector<sf::RectangleShape> textBoxes;
	std::vector<sf::Text> textOverlay;

	int windowWidth;
	int windowHeight;

	Pos mousePos;
	Pos cameraPos = { 0, 300 };
	int hotbarIndex = 0;
	int hotbarSize = 10;
	std::vector<LogicTile*> hotbar;
	LogicTile* selectedLogicTile;
	Robot* selectedRobot;
	Facing placeRotation;
	
	std::string selectedSave = "";
	bool running = true;
	bool gamePaused = true;
	bool showMain = true;
	bool showSave = false;
	std::mutex rendermutex;
	std::mutex worldmutex;

	// Clock timing variables
	clock_t deltaTime = 0;
	uint32_t frames = 0;
	double frameRate = 30;

	void RecreateGroundSprites(Pos tilePos, int x, int y);
	void RecreateItemSprites(uint64_t encodedPos, int x, int y);
	void RecreateLogicSprites(uint64_t encodedPos, int x, int y);
	void RecreateRobotSprites(uint64_t encodedPos, int x, int y);
	void RecreateAnimationSprites(uint64_t encodedPos, int x, int y);
	void RecreateSprites();
	void DrawSelectedBox();
	void DrawUpdateCounter();
	void DrawTooltips();
	void DrawHotbar();
	void DrawGameState(sf::RenderWindow& window);
};
extern ProgramData program;
