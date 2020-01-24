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
#include "SpriteVector.h"

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
	SpriteVector groundSprites;
	SpriteVector itemSprites;
	SpriteVector logicSprites;
	std::vector<sf::RectangleShape> selectionBoxes;
	SpriteVector robotSprites;
	SpriteVector animationSprites;
	SpriteVector hotbarSprites;
	std::vector<sf::RectangleShape> hotbarSlots;
	std::vector<sf::RectangleShape> textBoxes;
	std::vector<sf::Text> textOverlay;

	float windowWidth;
	float halfWindowWidth;
	float windowHeight;
	float halfWindowHeight;
	float scale = 5.0f;
	float zoom = 1.0f;
	float prevZoom = 1.0f;
	sf::View worldView;
	sf::View hudView;

	Pos mousePos;
	Pos mouseHovering;
	Pos prevCameraPos = {0, 0};
	Pos cameraPos = { 0, 0 };
	int hotbarIndex = 0;
	int hotbarSize = 9;
	std::vector<LogicTile*> hotbar;
	bool hoveringHotbar;
	LogicTile* selectedLogicTile;
	Robot* selectedRobot;
	Facing placeRotation;
	uint8_t placeColor = 1;
	
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

	void RecreateGroundSprites(Pos tilePos, float x, float y);
	void DrawItem(ItemTile item, float x, float y);
	void RecreateItemSprites(uint64_t encodedPos, float x, float y);
	void RecreateLogicSprites(uint64_t encodedPos, float x, float y);
	void RecreateRobotSprites(uint64_t encodedPos, float x, float y);
	void RecreateAnimationSprites(uint64_t encodedPos, float x, float y);
	void CreateText(int x, int y, std::string text);
	void RecreateSprites();

	void DrawSelectedBox();
	void DrawUpdateCounter();
	void DrawTooltips();
	void DrawHotbar();
	void DrawGameState(sf::RenderWindow& window);
	void DrawCrosshair(sf::RenderWindow& window);
	void FindMovingRobot();
	void SwapBots();
	void SwapItems();
	void CheckItemsMoved();
	void MoveBots();
	void UpdateMap();
};
extern ProgramData program;
