#pragma once
#include <SFML/Graphics.hpp>
#include "WorldSave.h"
#include "ProgramData.h"
#include "Pos.h"
#include "Windows.h"
#include "LogicTile.h"
#include <string>
#include "MyMod.h"
#include "CraftingProcess.h"
#include "ByteColors.h"
#include "Textures.h"
#include <typeinfo>
#include <cmath>
#include "GuiHandler.h"
#include "GroundTypes.h"

sf::Color ProgramData::HSV2RGB(sf::Color input)
{
	float h = input.r * 360.f / 256.f, s = input.g / 256.f, v = input.b / 256.f;
	float c = v * s;
	float x = c * (1.f-abs(MyMod(h / 60.f, 2.f)-1.f));
	float m = v - c;
	std::array<std::array<float, 3>, 6> colorTable;
	colorTable[0] = { c,x,0 };
	colorTable[1] = { x,c,0 };
	colorTable[2] = { 0,c,x };
	colorTable[3] = { 0,x,c };
	colorTable[4] = { x,0,c };
	colorTable[5] = { c,0,x };
	std::array<float, 3> color = colorTable[int(h / 60)];
	return sf::Color(uint8_t((color[0] + m) * 256), uint8_t((color[1] + m) * 256), uint8_t((color[2] + m) * 256), input.a);
}

void ProgramData::RecreateGroundSprites(Pos pos, float x, float y)
{
	/*for(int y=0;y<32;y++)
	{
		for (int x = 0; x < 32; x++)  
		{*/
			GroundTile* tile = world.GetGroundTile(pos);
			sf::Sprite sprite;
			uint8_t textureIndex = tile->groundTile;
			if (tile->groundTile > 64)
			{
				int index = int(textureIndex * 1.3f);
				sprite.setTexture(*groundTexture);
				sprite.setTextureRect(sf::IntRect((index / 32) * 32, 0, 32, 32));
				sf::Color color = HSV2RGB(sf::Color(MyMod(64 - index, 256), 100 + MyMod(index, 32), 220, 255));
				sprite.setColor(color);
			}
			else
			{
				switch (tile->groundTile)
				{
				case water:
				{
					sprite.setTexture(*groundTextures[0]);
				}break;
				case sand:
				{
					sprite.setTexture(*groundTextures[1]);
				}break;
				default: return;// Return if the tile isn't a recognised element
				}
			}
			sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
			sprite.setPosition(float(x + GC::halfTileSize), float(y + GC::halfTileSize));
			program.groundSprites.emplace_back(sprite);
	/*	} 
	}*/
}
void ProgramData::RecreatePlatformSprites(uint64_t encodedPos, float x, float y)
{
	/*
	if (uint16_t* platform = world.platforms.GetValue(encodedPos))
	{
		if (!program.showDebugInfo)
		{
			if (auto nextPos = world.nextPlatforms.GetValue(encodedPos))
			{
				Pos pos = Pos::EncodedToCoord(encodedPos);
				Pos newPos = pos.FacingPosition(*nextPos);
				Pos difference = newPos - pos;
				float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
				if (step > 1.0f)
					step = 1.0f;
				x += int(float(difference.x) * float(GC::tileSize) * step);
				y += int(float(difference.y) * float(GC::tileSize) * step);
			}
		}

		sf::Sprite sprite;
		sprite.setTexture(*platformTexture);
		sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
		sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
		program.platformSprites.emplace_back(sprite);
	}
	*/
}
void ProgramData::RecreateItemSprites(uint64_t encodedPos, float x, float y)
{
	if (ItemTile * tile = world.GetItemTile(encodedPos))
	{
		if (tile->itemTile >= ReservedItems::totalReserved)
		{
			if (auto nextPos = world.nextItemPos.GetValue(encodedPos))
			{
				Pos pos = Pos::EncodedToCoord(encodedPos);
				Pos newPos = pos.FacingPosition(*nextPos);
				Pos difference = newPos - pos;
				float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
				if (step > 1.0f)
					step = 1.0f;
				x += int(float(difference.x) * float(GC::tileSize) * step);
				y += int(float(difference.y) * float(GC::tileSize) * step);
			}
			if (auto nextPos = world.nextPlatforms.GetValue(encodedPos))
			{
				Pos pos = Pos::EncodedToCoord(encodedPos);
				Pos newPos = pos.FacingPosition(*nextPos);
				Pos difference = newPos - pos;
				float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
				if (step > 1.0f)
					step = 1.0f;
				x += int(float(difference.x) * float(GC::tileSize) * step);
				y += int(float(difference.y) * float(GC::tileSize) * step);
			}
			tile->DrawItem(&program.itemSprites, x, y, 1.5f, 0, sf::Color(255,255,255,255));
		}
	}
}
void ProgramData::RecreateLogicSprites(uint64_t encodedPos, float x, float y)
{
	if (LogicTile * logic = world.GetLogicTile(encodedPos))
	{
		if (auto nextPos = world.nextPlatforms.GetValue(encodedPos))
		{
			Pos pos = Pos::EncodedToCoord(encodedPos);
			Pos newPos = pos.FacingPosition(*nextPos);
			Pos difference = newPos - pos;
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			x += int(float(difference.x) * float(GC::tileSize) * step);
			y += int(float(difference.y) * float(GC::tileSize) * step);
		}
		logic->DrawLogic(Pos::EncodedToCoord(encodedPos), &program.logicSprites, &world.logicTiles, x, y, 1.0f, uint8_t(program.showSignalStrength));
		//logic->DrawTile(&program.logicSprites, x, y, 1.0f, uint8_t(program.showSignalStrength), sf::Color(255,255,255,255));
	}
}
void ProgramData::RecreateRobotSprites(uint64_t encodedPos, float x, float y)
{
	if (Robot* robot = world.GetRobot(encodedPos))
	{
		if (auto nextPos = world.nextRobotPos.GetValue(encodedPos))
		{
			Pos newPos = Pos::EncodedToCoord(encodedPos).FacingPosition(*nextPos);
			Pos difference = newPos - Pos::EncodedToCoord(encodedPos);
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			x += float(difference.x) * float(GC::tileSize) * step;
			y += float(difference.y) * float(GC::tileSize) * step;
		}
		if (auto nextPos = world.nextPlatforms.GetValue(encodedPos))
		{
			Pos newPos = Pos::EncodedToCoord(encodedPos).FacingPosition(*nextPos);
			Pos difference = newPos - Pos::EncodedToCoord(encodedPos);
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			x += float(difference.x) * float(GC::tileSize) * step;
			y += float(difference.y) * float(GC::tileSize) * step;
		}
		sf::Color color;
		if (robot == program.selectedRobot)
			color = sf::Color(130, 120, 255, 255);
		else
			color = sf::Color(250, 191, 38, 255);
		robot->DrawTile(&program.robotSprites,int(x), int(y), 1.0f, 0, color);
	}
}
void ProgramData::RecreateGhostSprites(uint64_t encodedPos, float x, float y)
{
	if (program.paste)
	{
		auto kv = program.copyMap.find((Pos::EncodedToCoord(encodedPos) - program.mouseHovering).CoordToEncoded());
		if (kv != program.copyMap.end())
		{
			kv->second.DrawLogic(Pos::EncodedToCoord(kv->first), &program.logicSprites, &program.copyMap, x, y, 1.f, 128);
		}
	}
}
void ProgramData::RecreateAnimationSprites(uint64_t encodedPos, float x, float y)
{
	if (CraftingProcess * recipe = world.GetCrafting(encodedPos))
	{
		CraftingClass craftingRef = program.craftingRecipes[recipe->craftingRecipe];
		recipe->DrawAnimation(int(x), int(y));

		// Crafting loading bar
		int craftbarHeight = 10;
		float loadingBarX = x + (float(craftingRef.width) / 2.0f) * float(GC::tileSize) - float(GC::halfTileSize);
		float loadingBarY = y - craftbarHeight / 2 + (float(craftingRef.height) / 2.0f) * float(GC::tileSize);
		sf::RectangleShape craftTotal;
		craftTotal.setFillColor(sf::Color(50, 50, 50));
		craftTotal.setSize(sf::Vector2f(float(GC::tileSize), float(craftbarHeight) / 2.f));
		craftTotal.setPosition(loadingBarX, loadingBarY);
		program.scaledBoxes.emplace_back(craftTotal);

		sf::RectangleShape craftPercent;
		craftPercent.setFillColor(sf::Color(80, 140, 200));
		craftPercent.setSize(sf::Vector2f((float(recipe->totalTicks - recipe->ticks) / float(recipe->totalTicks))*GC::tileSize, float(craftbarHeight) / 2.f));
		craftPercent.setPosition(loadingBarX, loadingBarY);
		program.scaledBoxes.emplace_back(craftPercent);
	}
}
void ProgramData::UpdateElementExists()
{
	elementExists.clear();
	elementExists.reserve(60000);
	/*for (auto elem : world.items)
	{
		elementExists.insert({ elem.first });
	}*/
	for (auto elem : world.logicTiles)
	{
		elementExists.insert({ elem.first });
	}
	for (auto elem : world.robots)
	{
		elementExists.insert({ elem.first });
	}
	for (auto elem : world.craftingQueue)
	{
		elementExists.insert({ elem.first });
	}
	if (program.paste)
	{
		for (auto kv : program.copyMap)
		{
			elementExists.insert({ (Pos::EncodedToCoord(kv.first) + program.mouseHovering).CoordToEncoded() });
		}
	}
	existsUpdate = clock();
}
void ProgramData::RecreateSprites() {
	if(program.redrawGround)
		program.groundSprites.clear();
	program.platformSprites.clear();
	program.itemSprites.clear();
	program.logicSprites.clear();
	program.robotSprites.clear();
	program.animationSprites.clear();
	program.textBacks.clear();
	program.textOverlay.clear();

	int begX = int((-program.halfWindowWidth / (GC::tileSize / program.zoom)) + (cameraPos.x >> GC::tileShift)) - 4;
	int endX = int(program.halfWindowWidth / (GC::tileSize / program.zoom) + (cameraPos.x >> GC::tileShift)) + 4;
	int begY = int((-program.halfWindowHeight / (GC::tileSize / program.zoom)) + (cameraPos.y >> GC::tileShift)) - 4;
	int endY = int(program.halfWindowHeight / (GC::tileSize / program.zoom) + (cameraPos.y >> GC::tileShift)) + 4;

	/*
	int begChunkY = begY / (GC::tileSize / program.zoom);
	int endChunkY = endY / (GC::tileSize / program.zoom) + 1;
	int begChunkX = begX / (GC::tileSize / program.zoom);
	int endChunkX = endX / (GC::tileSize / program.zoom) + 1;

	program.tilesRendered = (endY - begY) * (endX - begX);
	int chunksRendered = (endChunkY - begChunkY) * (endChunkX - begChunkX);

	if (program.redrawGround)
	{
		for (int y = begChunkY; y < endChunkY; y++)
		{
			for (int x = begChunkX; x < endChunkX; x++)
			{
				Pos screenPos = Pos{ int32_t(x * 32), int32_t(y * 32) } *(int)GC::tileSize - Pos{ GC::halfTileSize,GC::halfTileSize };
				Pos chunkPos = Pos{ x,y };
				
			}
		}
	}
	*/

	UpdateElementExists();
	for (int y = begY; y < endY; y++)
	{
		for (int x = begX; x <= endX; x++)
		{
			Pos screenPos = Pos{ x, y } * (int)GC::tileSize - Pos{ GC::halfTileSize,GC::halfTileSize };
			Pos tilePos = Pos{ x,y };
			uint64_t encodedPos = tilePos.CoordToEncoded();
			if(program.redrawGround)
				RecreateGroundSprites(tilePos, float(screenPos.x), float(screenPos.y));
			RecreateItemSprites(encodedPos, float(screenPos.x), float(screenPos.y));
			if (elementExists.find(encodedPos) != elementExists.end()) {
				//RecreatePlatformSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateLogicSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateRobotSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateAnimationSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateGhostSprites(encodedPos, float(screenPos.x), float(screenPos.y));
			}
		}
	}
	if (program.redrawGround)
		program.redrawGround = false;
}
void ProgramData::DrawUpdateCounter()
{
	char buffer[50];
	sprintf_s(buffer, "FPS/UPS: %.0f/%.0f", round(program.frameRate), round(program.updateRate));
	std::string displayValue = buffer;
	CreateText(program.halfWindowWidth - 10, -program.halfWindowHeight + 10, buffer, Align::right);
}
void ProgramData::DrawTooltips()
{
	if (!program.showingTooltip)
	{
		if (ItemTile* tile = world.GetItemTile(program.mouseHovering.CoordToEncoded()))
		{
			if (tile->itemTile > 2 && !program.showingTooltip)
			{
				std::string quantity = std::to_string(tile->quantity);
				CreateText(float(program.mousePos.x), float(program.mousePos.y - 20), tile->GetTooltip() + " " + quantity, Align::centre);
				program.showingTooltip = true;
			}
		}
	}
	if (!program.showingTooltip)
	{
		if (!program.hotbarSelectedLogicTile)
		{
			if (LogicTile* logic = world.GetLogicTile(program.mouseHovering.CoordToEncoded()))
			{
				program.selectedLogicTile = logic;
				CreateText(float(program.mousePos.x), float(program.mousePos.y - 20), program.itemTooltips[program.selectedLogicTile->logicType + program.itemsEnd], Align::centre);
				program.showingTooltip = true;
			}
			else
				program.selectedLogicTile = nullptr;
		}
	}
}

void ProgramData::DrawSelectedBox(std::vector<sf::RectangleShape>* appendTo, Pos pos)
{
	sf::RectangleShape selectionBox;
	selectionBox.setSize(sf::Vector2f(32, 32));
	selectionBox.setFillColor(sf::Color(0, 0, 0, 0));
	selectionBox.setOutlineColor(sf::Color(255, 255, 0, 255));
	selectionBox.setOutlineThickness(2);
	Pos drawPos = pos * int(GC::tileSize) - Pos{ GC::halfTileSize,GC::halfTileSize };
	selectionBox.setPosition(float(drawPos.x), float(drawPos.y));
	appendTo->emplace_back(selectionBox);
}

void ProgramData::DrawSelectedBox(std::vector<sf::RectangleShape>* appendTo, Pos pos, Pos pos2)
{
	if (pos.x > pos2.x)
	{
		int temp = pos2.x;
		pos2.x = pos.x;
		pos.x = temp;
	}
	if (pos.y > pos2.y)
	{
		int temp = pos2.y;
		pos2.y = pos.y;
		pos.y = temp;
	}
	sf::RectangleShape selectionBox;
	selectionBox.setSize(sf::Vector2f(32 * (pos2.x - pos.x), 32 * (pos2.y - pos.y)));
	selectionBox.setFillColor(sf::Color(0, 0, 0, 0));
	selectionBox.setOutlineColor(sf::Color(255, 255, 0, 255));
	selectionBox.setOutlineThickness(2);
	Pos drawPos = pos * (int)(GC::tileSize) - Pos{ GC::halfTileSize,GC::halfTileSize };
	selectionBox.setPosition(float(drawPos.x), float(drawPos.y));
	appendTo->emplace_back(selectionBox);
}

void ProgramData::DrawCrosshair(sf::RenderWindow& window)
{
	sf::RectangleShape verticalLine(sf::Vector2f(2.f, program.windowHeight));
	sf::RectangleShape horizontalLine(sf::Vector2f(program.windowWidth, 2.f));
	verticalLine.setPosition(0.f, -program.halfWindowHeight);
	horizontalLine.setPosition(-program.halfWindowWidth, 0.f);
	window.draw(verticalLine);
	window.draw(horizontalLine);
}

void ProgramData::CreateText(float x, float y, std::string input, Align align)
{
	sf::Text text;
	text.setString(input);
	text.setFont(program.guiFont);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(14);
	sf::FloatRect textRect = text.getLocalBounds();

	sf::RectangleShape backPlane;
	backPlane.setSize(sf::Vector2f(textRect.width + GC::tooltipPadding, textRect.height + GC::tooltipPadding));
	backPlane.setFillColor(sf::Color(0, 0, 0, 100));
	backPlane.setPosition(x, y);

	switch (align)
	{
	case centre:
	{
		text.setOrigin(textRect.width / 2, textRect.height / 2);
		backPlane.setOrigin(textRect.width / 2, textRect.height / 2);
	}break;
	case left:
	{
		text.setOrigin(0, 0);
		backPlane.setOrigin(0, 0);
	}break;
	case right:
	{
		text.setOrigin(textRect.width, textRect.height);
		backPlane.setOrigin(textRect.width, textRect.height);
	}break;
	}
	text.setPosition(x, y);

	program.textBacks.emplace_back(backPlane);
	program.textOverlay.emplace_back(text);
}

void ProgramData::CreateSmallText(SpriteVector* appendTo, std::string text, float x, float y, float s, Align align)
{
	sf::Sprite sprite;
	sprite.setTexture(*font);
	for (int i = 0; i < (int)text.length(); i++)
	{
		float adjustLeft;
		switch (align)
		{
		case centre:
		{
			sprite.setOrigin(1.5, 2.5);
			adjustLeft = (float(text.length() - 1) / 2.f) * 4;
		}break;
		case left:
		{
			sprite.setOrigin(0, 0);
			adjustLeft = 0;
		}break;
		case right:
		{
			sprite.setOrigin(3, 5);
			adjustLeft = (float(text.length() - 1)) * 4;
		}break;
		}
		sprite.setTextureRect(program.fontMap[text[i]]);
		sprite.setPosition(float(x + float(GC::halfTileSize + s * (i * 4 - adjustLeft))), float(y + GC::halfTileSize));
		sprite.setScale(sf::Vector2f(s, s));
		appendTo->emplace_back(sprite);
	}
}
void ProgramData::FindMovingRobot()
{
	if (program.selectedRobot)
	{
		if (Facing* movingTo = world.nextRobotPos.GetValue(program.selectedRobotPos))
		{
			Pos difference = program.selectedRobotPos.FacingPosition(*movingTo) - program.selectedRobotPos;
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			program.cameraPos = (program.selectedRobotPos << (int)GC::tileShift) + Pos{ int(float(difference.x) * float(GC::tileSize) * step) ,int(float(difference.y) * float(GC::tileSize) * step) };
		}
		else
		{
			program.cameraPos = (program.selectedRobotPos << (int)GC::tileShift);
		}
	}
}

void ProgramData::DrawGameState(sf::RenderWindow& window) {
	if (program.worldLoaded) {
		FindMovingRobot();
		if (program.prevZoom != program.zoom)
		{
			program.redrawGround = true;
			program.worldView.zoom(program.zoom / program.prevZoom);
			program.prevZoom = program.zoom;
		}
		if (program.forceRefresh || program.prevCameraPos != program.cameraPos)
		{
			program.redrawGround = true;
			program.forceRefresh = false;
			program.worldView.move(sf::Vector2f(float(program.cameraPos.x - program.prevCameraPos.x), float(program.cameraPos.y - program.prevCameraPos.y)));
			program.prevCameraPos = program.cameraPos;
			// Recalculate the possition of the mouse and what the new selection is after moving
			program.RecalculateMousePos();
		}
		program.scaledBoxes.clear();
		RecreateSprites();
		if(program.selectedLogicTile)
			DrawSelectedBox(&program.scaledBoxes, program.mouseHovering);
		DrawAlignment();
		DrawTooltips();
		DrawSelectedRegion();
		window.setView(program.worldView);
		program.groundSprites.draw(window);
		for (sf::RectangleShape sprite : program.mapGround)
			window.draw(sprite);
		program.platformSprites.draw(window);
		program.logicSprites.draw(window);
		program.itemSprites.draw(window);
		program.animationSprites.draw(window);
		program.robotSprites.draw(window);
		for (sf::RectangleShape sprite : program.mapShapes)
			window.draw(sprite);
		for (sf::RectangleShape sprite : program.scaledBoxes)
			window.draw(sprite);
		for (sf::RectangleShape sprite : program.scaledPersistentBoxes)
			window.draw(sprite);		
	}
}

void ProgramData::MovePlatform(Pos pos, Facing toward)
{
	Pos newPos = pos.FacingPosition(toward);
	program.scaledPersistentBoxes.clear();
	if (program.showDebugInfo)
	{
		Sleep(100);
		DrawSelectedBox(&program.scaledPersistentBoxes, pos);
		DrawSelectedBox(&program.scaledPersistentBoxes, newPos);
	}
	if (LogicTile* logic = world.GetLogicTile(pos))
	{
		// If there's a platform infront and they're also moving, move them first
		if (LogicTile* platform = world.GetLogicTile(newPos))
		{	
			if (auto moving = world.nextPlatforms.GetValue(newPos))
			{
				MovePlatform(newPos, *moving);
			}
		}
		if (ItemTile* elem = world.GetItemTile(pos.CoordToEncoded()))
		{
			if (ItemTile* elem = world.GetItemTile(newPos.CoordToEncoded()))
			{
				world.updateItemsNext.insert(pos.CoordToEncoded());
				world.updateItemsNext.insert(newPos.CoordToEncoded());
			}
			else
			{
				world.updateItemsNext.insert(newPos.CoordToEncoded());
				//world.items[newPos.CoordToEncoded()] = world.items[pos.CoordToEncoded()];
				//world.items.erase(pos.CoordToEncoded());
			}
		}
		if (Robot* elem = world.GetRobot(pos.CoordToEncoded()))
		{
			/*
			if (auto moving = world.nextRobotPos.GetValue(pos.CoordToEncoded()))
			{
				world.nextRobotPos.erase(pos.CoordToEncoded());
				world.nextRobotPos.insert({ newPos.CoordToEncoded(),toward });
			}*/
			world.robots[newPos.CoordToEncoded()] = world.robots[pos.CoordToEncoded()];
			world.robots.erase(pos.CoordToEncoded());
		}
		//Get value doesn't create a new instance if the value isn't there
		if (int* update = world.updateNext.GetValue(pos.CoordToEncoded()))
		{
			world.updateNext[newPos.CoordToEncoded()] = world.updateNext[pos.CoordToEncoded()];
			world.updateNext.erase(pos.CoordToEncoded());
		}
		world.logicTiles[newPos.CoordToEncoded()] = world.logicTiles[pos.CoordToEncoded()];
		world.logicTiles.erase(pos.CoordToEncoded());
		world.nextPlatforms.erase(pos.CoordToEncoded());
	}
}

void ProgramData::SwapPlatforms()
{
	while(!world.nextPlatforms.empty())
	{
		auto value = world.nextPlatforms.begin();
		MovePlatform(Pos::EncodedToCoord(value->first),value->second);
	}
	world.moving = false;
}

void ProgramData::SwapBots()
{
	for (std::pair<uint64_t, Facing> moving : world.nextRobotPos)
	{
		Pos pos = Pos::EncodedToCoord(moving.first);
		Pos newPos = pos.FacingPosition(moving.second);
		if (Robot* test = world.GetRobot(pos))
		{
			// Be sure to update which robot the players controlling before changing the map incase of remap
			if (&world.robots[pos.CoordToEncoded()] == program.selectedRobot)
			{
				program.selectedRobot = &world.robots[newPos.CoordToEncoded()];
				program.selectedRobotPos = newPos;
			}
			world.robots[newPos.CoordToEncoded()] = world.robots[pos.CoordToEncoded()];
			world.robots.erase(pos.CoordToEncoded());

			// If the robot landed on a logic tile, apply its logic this tick
			if (LogicTile* logicTile = world.GetLogicTile(newPos))
			{
				logicTile->DoRobotLogic(newPos, newPos);
			}

			// If the robot left a logic tile, update its logic
			if (LogicTile* logicTile = world.GetLogicTile(pos))
			{
				logicTile->DoRobotLogic(pos, pos);
			}
		}
	}
	world.nextRobotPos.clear();
}

void ProgramData::MoveBots()
{
	for (auto& robotIter : world.robots)
	{
		if (&robotIter.second != program.selectedRobot)
		{
			robotIter.second.Move(Pos::EncodedToCoord(robotIter.first));
		}
		else
		{
			if (program.rotateBot != 0)
			{
				robotIter.second.Rotate(program.rotateBot);
				program.rotateBot = 0;
			}
			if (program.moveBot)
			{
				robotIter.second.Move(Pos::EncodedToCoord(robotIter.first));
				program.moveBot = false;
			}
		}
	}
}

void ProgramData::MoveItem(Pos pos, Facing toward)
{
	Pos newPos = pos.FacingPosition(toward);
	// If there's a item infront and they're also moving, move them first
	if (auto frontItem = world.GetItemTile(newPos))
	{
		if (auto moving = world.nextItemPos.GetValue(newPos))
		{
			MoveItem(newPos, *moving);
		}
	}
	if (ItemTile* item = world.GetItemTile(pos))
	{
		if (item->itemTile == 0)
		{
			assert(false);
		}
		world.ChangeItem(pos, BigItem(item->itemTile,-1));
		world.ChangeItem(newPos, BigItem(item->itemTile, 1));
		world.itemPrevMoved.insert(newPos.CoordToEncoded());
	}
	world.nextItemPos.erase(pos.CoordToEncoded());
}

void ProgramData::SwapItems()
{
	world.itemPrevMoved.clear();
	while (!world.nextItemPos.empty())
	{
		auto start = world.nextItemPos.begin();
		MoveItem(Pos::EncodedToCoord(start->first), start->second);
	}
}

void ProgramData::CheckItemsMoved()
{
	for (std::pair<uint64_t, uint64_t> newPos : world.nextItemPos)
	{
		world.itemPrevMoved.erase(newPos.first);
	}
	for (uint64_t wasMoving : world.itemPrevMoved)
	{
		if (ItemTile* item = world.GetItemTile(wasMoving))
			CraftingClass::TryCrafting(item->itemTile, Pos::EncodedToCoord(wasMoving));
	}
}

void ProgramData::UpdateMap()
{
	world.platforms.clear();
	world.updateCurr.clear();
	// Update all the logic tiles that were queued to finish this tick
	std::vector<uint64_t> removeList;
	for (auto iter : world.updateNext)
	{
		iter.second--;
		// Add all 0 tick queued elements to be updated this tick
		if (iter.second == 0)
		{
			world.updateCurr.insert({ iter.first });
			removeList.emplace_back(iter.first);
		}
	}
	for (uint64_t key : removeList)
	{
		world.updateNext.erase(key);
	}
	do {
		for (uint64_t kv : world.updateCurr)
		{
			auto logic = world.logicTiles.find(kv);
			if (logic != world.logicTiles.end())
			{
				logic->second.DoWireLogic(Pos::EncodedToCoord(logic->first));
			}
		}
		world.updateCurr = MySet<uint64_t>(world.updateProp);
		world.updateProp.clear();
	} while (world.updateCurr.size() != 0);
	removeList.clear();
	// Decrease the number of ticks for each active recipe and complete recipes that reach 0
	for (auto& iter : world.craftingQueue)
	{
		iter.second.ticks--;
		// Erase all 0 tick recipes from craftingQueue
		if (iter.second.ticks == 0)
			program.craftingRecipes[iter.second.craftingRecipe].SuccessfulCraft(iter.second.pos);
		if (iter.second.ticks == 0) // Check twice because SuccessfulCraft() may add ticks to queue a new craft
			removeList.emplace_back(iter.first);
	}
	for (uint64_t key : removeList)
	{
		world.craftingQueue.erase(key);
	}
	world.prevItemMovingTo = world.itemMovingTo;
	world.itemMovingTo.clear();
	world.robotMovingTo.clear();
	world.platformMovingTo.clear();
	SwapItems();
	SwapBots();
	MoveBots();
	CheckItemsMoved();
	SwapPlatforms();
	for (uint64_t kv : world.updateItemsNext)
	{
		if (LogicTile* logic = world.GetLogicTile(kv))
		{
			logic->DoItemLogic(Pos::EncodedToCoord(kv));
		}
	}
	world.updateItemsNext.clear();
	world.tick++;
}

void ProgramData::DrawAlignment()
{
	if (program.selectedLogicTile)
	{
		bool drawLine = false;
		Facing rotation = program.placeRotation;
		if (program.selectedLogicTile->ShowAlign())
			drawLine = true;
		rotation = program.selectedLogicTile->facing;
		if (drawLine)
		{
			sf::RectangleShape selectionBox;
			selectionBox.setSize(sf::Vector2f(4, 1024 * program.zoom));
			selectionBox.setOrigin(sf::Vector2f(2, 1024 * program.zoom + GC::halfTileSize));
			selectionBox.rotate(rotation * 90.f);
			selectionBox.setFillColor(sf::Color(255, 255, 0, 255));
			Pos drawPos = program.mouseHovering * (int)GC::tileSize;
			selectionBox.setPosition(float(drawPos.x), float(drawPos.y));
			program.scaledBoxes.emplace_back(selectionBox);
		}
	}
}

void ProgramData::DrawSelectedRegion()
{
	if (program.copy || program.cut)
	{
		if (program.startedSelection)
		{
			DrawSelectedBox(&program.scaledBoxes, program.startSelection, program.mouseHovering);
		}
	}
}

void ProgramData::RecalculateMousePos()
{
	program.mouseHovering = ((program.mousePos * program.zoom) + program.cameraPos) / float(GC::tileSize);
}

void ProgramData::DrawItemGrid(int screenX, int screenY, SmallPos size, float s, SmallPos highlight, MyMap<SmallPos,std::string>* tooltips, MyMap<SmallPos, BigItem>* items, SpriteVector* sprites, Facing rotation, uint8_t color, bool drawMid, MyMap<SmallPos, sf::Sprite>* backs)
{
	float extraScale = 1.5f;
	for (uint8_t i = 0; i < size.x; i++)
	{
		for (uint8_t j = 0; j < size.y; j++)
		{
			if (drawMid || (i != (size.x / 2)))
			{
				int x = screenX + (i * GC::hotbarTotalSize * s) + (GC::hotbarTotalSize * 0.5 * s);
				int y = screenY + (j * GC::hotbarTotalSize * s) + (GC::hotbarTotalSize * 0.5 * s);
				sf::Sprite gridBack;
				gridBack.setTexture(*blank);
				
				if(i == highlight.x && j == highlight.y)
					gridBack.setColor(sf::Color(200, 200, 200, 100));
				else
					gridBack.setColor(sf::Color(50, 50, 50, 100));
				gridBack.setOrigin(GC::halfTileSize, GC::halfTileSize);
				gridBack.setScale(sf::Vector2f(s * extraScale, s * extraScale));
				gridBack.setPosition(sf::Vector2f(x, y));
				backs->insert({ SmallPos{i,j},gridBack });
			}
		}
	}
	for (auto kv : *items)
	{
		//sf::Vector2f ImguiOffset = sf::Vector2f(program.halfWindowWidth - GC::halfTileSize, program.halfWindowHeight - GC::halfTileSize);
		sf::Vector2f ImguiOffset = sf::Vector2f(0, 0);
		BigItem item = kv.second;
		SmallPos pos = kv.first;
		int x = screenX + (pos.x * GC::hotbarTotalSize * s) + (GC::hotbarTotalSize * 0.5 * s);
		int y = screenY + (pos.y * GC::hotbarTotalSize * s) + (GC::hotbarTotalSize * 0.5 * s);
		if (item.itemTile <= program.itemsEnd)
		{
			item.DrawItem(sprites, x - GC::halfTileSize + ImguiOffset.x, y - GC::halfTileSize + ImguiOffset.y, s * extraScale, 0, sf::Color(255, 255, 255, 255));
			tooltips->insert({ kv.first, item.GetTooltip() });
		}
		else if (item.itemTile < program.itemsEnd + 255)
		{
			LogicTile logic = LogicTile(item.itemTile - program.itemsEnd);
			logic.color = color;
			logic.signal = 1;
			logic.facing = rotation;
			logic.DrawLogic(Pos{ MAXINT32,MAXINT32 }, sprites, &world.logicTiles, x - GC::halfTileSize + ImguiOffset.x, y - GC::halfTileSize + ImguiOffset.y, s, 0);
			tooltips->insert({ kv.first, item.GetTooltip() });
		}
		else if (item.itemTile == program.itemsEnd + 255)
		{
			Robot robot = Robot();
			robot.facing = rotation;
			robot.DrawTile(sprites, x - GC::halfTileSize + ImguiOffset.x, y - GC::halfTileSize + ImguiOffset.y, s, 0, sf::Color(250, 191, 38, 255));
			tooltips->insert({ kv.first, item.GetTooltip() });
		}
		if (item.quantity > 1)
			CreateSmallText(sprites, std::to_string(item.quantity), x - GC::halfTileSize + GC::halfTileSize * s * extraScale, y - GC::halfTileSize + GC::halfTileSize * s * extraScale, s * extraScale, Align::right);
	}
}

SmallPos ProgramData::DrawGridTooltips(MyMap<SmallPos, sf::RectangleShape>* slots, MyMap<SmallPos, BigItem>* items)
{
	if (!program.showingTooltip)
	{
		for (auto element : *slots)
		{
			sf::RectangleShape rect = element.second;
			sf::FloatRect rectBox(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
			if (rectBox.contains(sf::Vector2f(float(program.mousePos.x), float(program.mousePos.y))))
			{
				auto kv = items->find(element.first);
				if (kv != items->end())
				{
					CreateText(float(program.mousePos.x), float(program.mousePos.y - 20), kv->second.GetTooltip(), Align::centre);
					program.showingTooltip = true;
					return element.first;
				}
			}
		}
	}
	return SmallPos{ 255,255 };
}

void ProgramData::DrawHotbar()
{
	if (program.hotbarUpdate)
	{
		program.hotbarBacks.clear();
		program.hotbarSprites.clear();
		program.hotbarTooltips.clear();

		if (program.hotbarSelectedLogicTile)
			if (program.selectedLogicTile)
			{
				delete program.selectedLogicTile;
				program.selectedLogicTile = nullptr;
				program.hotbarSelectedLogicTile = false;
			}

		if (program.hotbarShow)
		{
			float windowEndHeight = 15;
			float border = 10;
			float viewScaleA = (program.hotbarDimensions.x - 2 * border) / (GC::hotbarTotalSize * float(10));
			float viewScaleB = (program.hotbarDimensions.y - 2 * border - windowEndHeight) / (GC::hotbarTotalSize * float(2));
			float viewScale = std::min(viewScaleA, viewScaleB);

			DrawItemGrid(border, windowEndHeight + border, SmallPos{ 10,2 }, viewScale, program.hotbarIndex, &program.hotbarTooltips, &program.hotbar, &program.hotbarSprites, program.placeRotation, program.placeColor, true, &program.hotbarBacks);

			if (BigItem* item = program.hotbar.GetValue(program.hotbarIndex))
			{
				if (item->itemTile > program.itemsEnd)
				{
					program.selectedLogicTile = new LogicTile(item->itemTile - program.itemsEnd);
					program.selectedLogicTile->facing = program.placeRotation;
					program.hotbarSelectedLogicTile = true;
				}
			}
		}
	}
}

void ProgramData::DrawCraftingView()
{
	if (program.craftingViewUpdate)
	{
		program.craftingViewBacks.clear();
		program.craftingViewSprites.clear();
		program.craftingViewTooltips.clear();

		if (program.craftingViewShow)
		{
			if (foundRecipeList.size() > 0)
			{
				program.craftingViewUnlocked = program.craftingRecipes[program.foundRecipeList[program.craftingViewIndex]].unlocked;
				program.craftingRecipes[(int)program.foundRecipeList[program.craftingViewIndex]].ShowRecipeAsGrid();

				ImVec2 lastElement = ImGui::GetCursorPos();
				float windowEndHeight = 15;
				float border = 10;
				float viewScaleA = (program.craftingViewDimensions.x - 2 * border) / (GC::hotbarTotalSize * float(program.craftingViewSize.x));
				float viewScaleB = (program.craftingViewDimensions.y - 2 * border - windowEndHeight - lastElement.y) / (GC::hotbarTotalSize * float(program.craftingViewSize.y));
				float viewScale = std::min(viewScaleA, viewScaleB);

				DrawItemGrid(border, lastElement.y + border, program.craftingViewSize, viewScale, SmallPos{ 255,255 }, &program.craftingViewTooltips, &program.craftingView, &program.craftingViewSprites, south, red, false, &program.craftingViewBacks);
			}
		}
		program.craftingViewUpdate = false;
	}
}

void ProgramData::DrawTechnologyView()
{
	if (program.technologyViewUpdate)
	{
		program.technologyViewBacks.clear();
		program.technologyViewSprites.clear();
		program.technologyViewTooltips.clear();

		if (program.technologyViewShow)
		{
			for (auto& tech : program.technologyPrototypes)
			{
				if (tech.name == world.currentTechnology.name)
				{
					tech.ShowRequirementsAsGrid();
					if (world.techCompleted)
					{
						world.techCompleted = false;
						tech.Unlock();
						program.technologyView.clear();
						world.unlockedTechnologies.emplace_back(tech.name);
						world.FindNextTechnology();
					}
					else
						break;
				}
			}
			ImVec2 lastElement = ImGui::GetCursorPos();
			float windowEndHeight = 15;
			float border = 10;
			float viewScaleA = (program.technologyViewDimensions.x - 2 * border) / (GC::hotbarTotalSize * float(technologyViewSize.x));
			float viewScaleB = (program.technologyViewDimensions.y - 2 * border - windowEndHeight - lastElement.y) / (GC::hotbarTotalSize * float(technologyViewSize.y));
			float viewScale = std::min(viewScaleA, viewScaleB);
			
			DrawItemGrid(border, lastElement.y + border, program.technologyViewSize, viewScale, SmallPos{ 255,255 }, &program.technologyViewTooltips, &program.technologyView, &program.technologyViewSprites, south, red, false, &program.technologyViewBacks);
		}
		program.technologyViewUpdate = false;
	}
}