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
#include "MySet.h"
#include "Align.h"
#include "MyVector.h"

class ProgramData {
public:
	MyMap<uint16_t, std::vector<uint16_t>> itemRecipeList;	// Mapping from item type to recipe. Ie what items catalyse particular crafting recipes
	std::vector<CraftingClass> craftingRecipes;				// Vector of crafting recipes
	
	std::vector<std::string> itemPrototypes;
	std::vector<std::string> itemTooltips;
	int regItemsEnd;
	std::vector<std::vector<std::string>> logicTooltips;
	MyMap<char, sf::IntRect> fontMap;	// Map that relates characters to sections of the font texture
	sf::Font guiFont;
	std::vector<Animation> animationTemplates;

	int minGround = 255;
	int maxGround = 0;
	//Stores sprite layers
	SpriteVector groundSprites;
	SpriteVector platformSprites;
	SpriteVector itemSprites;
	SpriteVector logicSprites;
	std::vector<sf::RectangleShape> scaledBoxes;
	std::vector<sf::RectangleShape> scaledPersistentBoxes;
	SpriteVector robotSprites;
	SpriteVector animationSprites;
	SpriteVector hotbarSprites;
	std::vector<sf::RectangleShape> hotbarSlots;
	std::vector<sf::RectangleShape> unscaledBoxes;
	std::vector<sf::Text> textOverlay;
	MySet<uint64_t> elementExists;

	float windowedWidth;
	float windowedHeight;
	float windowWidth;
	float halfWindowWidth;
	float windowHeight;
	float halfWindowHeight;
	float scale = 5.0f;
	float zoom = 1.0f;
	float prevZoom = 1.0f;
	bool redrawGround = true;
	int tilesRendered = 0;
	sf::View worldView;
	sf::View hudView;

	Pos mousePos;
	Pos prevMouseHovering;
	Pos mouseHovering;
	Pos prevCameraPos = {0, 0};
	Pos cameraPos = { 0, 0 };
	int hotbarIndex = 0;
	int hotbarSize = 20;
	MyVector<ParentTile*> hotbar;
	int hoveringHotbar;
	ParentTile* selectedHotbar;
	LogicTile* selectedLogicTile;
	Robot* selectedRobot;
	Facing placeRotation;
	uint8_t placeColor = 1;
	bool showSignalStrength = true;
	bool showDebugInfo = false;
	
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
	void DrawItem(SpriteVector* appendTo, ItemTile item, float x, float y);
	void RecreatePlatformSprites(uint64_t encodedPos, float x, float y);
	void RecreateItemSprites(uint64_t encodedPos, float x, float y);
	void RecreateLogicSprites(uint64_t encodedPos, float x, float y);
	void RecreateRobotSprites(uint64_t encodedPos, float x, float y);
	void RecreateAnimationSprites(uint64_t encodedPos, float x, float y);
	void CreateSmallText(SpriteVector* appendTo, std::string text, float x, float y, float s, Align a);
	void CreateText(float x, float y, std::string text, Align align);
	void RecreateSprites();
	void UpdateElementExists();
	
	sf::Color HSV2RGB(sf::Color);
	void DrawSelectedBox(std::vector<sf::RectangleShape>* appendTo, Pos pos);
	void DrawUpdateCounter();
	void DrawTooltips();
	void DrawHotbar();
	void DrawDebugHUD();
	void DrawGameState(sf::RenderWindow& window);
	void DrawCrosshair(sf::RenderWindow& window);
	void FindMovingRobot();
	void MovePlatform(Pos pos, Facing toward);
	void SwapPlatforms();
	void SwapBots();
	void MoveItem(Pos pos, Facing toward);
	void SwapItems();
	void CheckItemsMoved();
	void MoveBots();
	void UpdateMap();
};
extern ProgramData program;
