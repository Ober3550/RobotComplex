#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <SFML/Graphics.hpp>

#include "Robot.h"
#include "CraftingClass.h"
#include "MyMap.h"
#include "LogicTile.h"
#include "Animation.h"
#include "ItemTile.h"

class ProgramData {
public:
	MyMap<uint16_t, std::vector<uint16_t>> itemRecipeList;	// Mapping from item type to recipe. Ie what items catalyse particular crafting recipes
	std::vector<CraftingClass> craftingRecipes;				// Vector of crafting recipes
	
	std::vector<std::string> itemPrototypes;
	std::vector<std::string> itemTooltips;
	std::vector<std::vector<std::string>> logicTooltips;
	MyMap<char, sf::IntRect> fontMap;	// Map that relates characters to sections of the font texture
	sf::Font guiFont;
	std::vector<Animation> animationTemplates;

	//Stores sprite layers
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
	float zoom = 5;
	float scale;

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
	std::mutex worldMutex;

	// Clock timing variables
	double frameRate = 30;
	double updateRate = 30;
	int framesSinceTick = 0;

	void RecreateGroundSprites(Pos tilePos, int x, int y);
	void DrawItem(ItemTile item, int x, int y);
	void RecreateItemSprites(uint64_t encodedPos, int x, int y);
	void RecreateLogicSprites(uint64_t encodedPos, int x, int y);
	void RecreateRobotSprites(uint64_t encodedPos, int x, int y);
	void RecreateAnimationSprites(uint64_t encodedPos, int x, int y);
	void RecreateSprites();
	void CreateText(int x, int y, std::string text);
	void DrawSelectedBox();
	void DrawUpdateCounter();
	void DrawTooltips();
	void DrawHotbar();
	void DrawGameState(sf::RenderWindow& window);
	void SwapBots();
	void SwapItems();
	void CheckItemsMoved();
	void MoveBots();
	void UpdateMap();
};
extern ProgramData program;
