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
#include "RecipePrototype.h"

class ProgramData {
public:
	MyMap<uint16_t, std::vector<uint16_t>> itemRecipeList;	// Mapping from item type to recipe. Ie what items catalyse particular crafting recipes
	MyMap<uint16_t, std::vector<uint16_t>> itemResultList;
	std::vector<CraftingClass> craftingRecipes;				// Vector of crafting recipes
	MyMap<std::string, RecipeProto> recipePrototypes;
	
	MyMap<uint16_t, std::string> itemPrototypes;
	MyMap<std::string, uint16_t> itemLookups;
	MyMap<uint16_t, std::string> itemTooltips;
	int itemsEnd;
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

	// Gui Elements
	SpriteVector hotbarSprites;
	MyMap<SmallPos, sf::RectangleShape> hotbarSlots;
	SmallPos hotbarIndex = { 255,255 };
	SmallPos hoveringHotbar = { 255,255 };
	MyMap<SmallPos, ItemTile> hotbar;
	ItemTile* selectedHotbar;

	SpriteVector unscaledSprites;
	std::vector<sf::RectangleShape> unscaledBoxes;
	
	bool								craftingViewUpdate;
	std::vector<uint16_t>				foundRecipeList;
	uint16_t							craftingViewIndex;
	SpriteVector						craftingViewSprites;
	MyMap<SmallPos, sf::RectangleShape> craftingViewBacks;
	MyMap<SmallPos, ItemTile>			craftingView;
	SmallPos							craftingViewSize;

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
	LogicTile* selectedLogicTile;
	Pos selectedRobotPos;
	Robot* selectedRobot;
	int rotateBot = 0;
	bool moveBot = false;
	Facing placeRotation;
	uint8_t placeColor = 1;
	bool showSignalStrength = true;
	bool showDebugInfo = false;
	bool cut = false;
	bool copy = false;
	bool paste = false;
	bool godmode = true;
	bool startedSelection = false;
	Pos startSelection;
	Pos originSelection;
	MyMap<uint64_t, LogicTile> copyMap;
	
	
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
	void DrawItem(SpriteVector* appendTo, ItemTile item, float x, float y, uint8_t flags);
	void RecreatePlatformSprites(uint64_t encodedPos, float x, float y);
	void RecreateItemSprites(uint64_t encodedPos, float x, float y);
	void RecreateLogicSprites(uint64_t encodedPos, float x, float y);
	void RecreateRobotSprites(uint64_t encodedPos, float x, float y);
	void RecreateAnimationSprites(uint64_t encodedPos, float x, float y);
	void RecreateGhostSprites(uint64_t encodedPos, float x, float y);
	void CreateSmallText(SpriteVector* appendTo, std::string text, float x, float y, float s, Align a);
	void CreateText(float x, float y, std::string text, Align align);
	void RecreateSprites();
	void UpdateElementExists();
	
	sf::Color HSV2RGB(sf::Color);
	void DrawSelectedBox(std::vector<sf::RectangleShape>* appendTo, Pos pos);
	void DrawSelectedBox(std::vector<sf::RectangleShape>* appendTo, Pos pos, Pos pos2);
	void DrawAlignment();
	void DrawUpdateCounter();
	void DrawTooltips();
	void DrawHotbar();
	void DrawCraftingView();
	void DrawSelectedRegion();
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
	void RecalculateMousePos();
	void DrawItemGrid(int screenX, int screenY, SmallPos size, float scale, SmallPos highlight, MyMap<SmallPos, sf::RectangleShape>* slots, MyMap<SmallPos, ItemTile>* items, SpriteVector* sprites, Facing rotation, uint8_t color);
};
extern ProgramData program;
