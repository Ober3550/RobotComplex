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
#include "RedirectorColors.h"
#include "Textures.h"
#include <cmath>

sf::Color ProgramData::HSV2RGB(sf::Color input)
{
	float h = input.r * 360.f / 255.f, s = input.g / 255.f, v = input.b / 255.f;
	float c = v * s;
	float x = c * (1 - abs((int(h / 60) & 1) - 1));
	float m = v - c;
	std::array<std::array<float, 3>, 6> colorTable;
	colorTable[0] = { c,x,0 };
	colorTable[1] = { x,c,0 };
	colorTable[2] = { 0,c,x };
	colorTable[3] = { 0,x,c };
	colorTable[4] = { x,0,c };
	colorTable[5] = { c,0,x };
	std::array<float, 3> color = colorTable[int(h / 60)];
	return sf::Color((color[0] + m) * 255, (color[1] + m) * 255, (color[2] + m) * 255, input.a);
}

void ProgramData::RecreateGroundSprites(Pos tilePos, float x, float y)
{
	if (GroundTile * tile = world.GetGroundTile(tilePos))
	{
		sf::Sprite sprite;
		uint8_t textureIndex = tile->groundTile;
		sprite.setTexture(*groundTextures[0]);
		//sprite.setTextureRect(sf::IntRect((textureIndex & 3)*32, 0, 32, 32));
		sf::Color color = HSV2RGB(sf::Color(50, MyMod(textureIndex,16) * 20, 100 + (textureIndex / 20) * 10, 255));
		sprite.setColor(color);
		sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
		sprite.setPosition(float(x + GC::halfTileSize), float(y + GC::halfTileSize));
		program.groundSprites.emplace_back(sprite);
	}
}
void ProgramData::DrawItem(ItemTile item, float x, float y)
{
	sf::Sprite sprite;
	sprite.setTexture(*itemTextures[item.itemTile]);
	sprite.setOrigin(8, 8);
	sprite.setPosition(x + 16.f, y + 16.f);
	program.itemSprites.emplace_back(sprite);
}
void ProgramData::RecreateItemSprites(uint64_t encodedPos, float x, float y)
{
	if (ItemTile * tile = world.GetItemTile(encodedPos))
	{
		if (tile->itemTile >= ReservedItems::totalReserved)
		{
			Pos pos = Pos::EncodedToCoord(encodedPos);
			if (auto nextPos = world.nextItemPos.GetValue(encodedPos))
			{
				Pos newPos = Pos::EncodedToCoord(*nextPos);
				Pos difference = newPos - pos;
				float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
				if (step > 1.0f)
					step = 1.0f;
				Pos screenPos = { int(x) + int(float(difference.x) * float(GC::tileSize) * step), int(y) + int(float(difference.y) * float(GC::tileSize) * step) };
				DrawItem(*tile, float(screenPos.x), float(screenPos.y));
			}
			else
				DrawItem(*tile, x, y);
		}
	}
}
void ProgramData::RecreateLogicSprites(uint64_t encodedPos, float x, float y)
{
	if (LogicTile * logic = world.GetLogicTile(encodedPos))
	{
		logic->DrawTile(&program.logicSprites, x, y, 1.0f);
	}
}
void ProgramData::RecreateRobotSprites(uint64_t encodedPos, float x, float y)
{
	if (Robot* robot = world.GetRobot(encodedPos))
	{
		if (auto nextPos = world.nextRobotPos.GetValue(encodedPos))
		{
			Pos newPos = Pos::EncodedToCoord(*nextPos);
			Pos difference = newPos - robot->pos;
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			x += float(difference.x) * float(GC::tileSize) * step;
			y += float(difference.y) * float(GC::tileSize) * step;
		}
		robot->DrawTile(int(x), int(y), 1.0f);
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
		program.selectionBoxes.emplace_back(craftTotal);

		sf::RectangleShape craftPercent;
		craftPercent.setFillColor(sf::Color(80, 140, 200));
		craftPercent.setSize(sf::Vector2f((float(recipe->totalTicks - recipe->ticks) / float(recipe->totalTicks))*GC::tileSize, float(craftbarHeight) / 2.f));
		craftPercent.setPosition(loadingBarX, loadingBarY);
		program.selectionBoxes.emplace_back(craftPercent);
	}
}
void ProgramData::UpdateElementExists()
{
	elementExists.clear();
	for (auto elem : world.items)
	{
		elementExists.insert({ elem.first });
	}
	for (auto elem : world.logictiles)
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
}
void ProgramData::RecreateSprites() {
	if(program.redrawGround)
		program.groundSprites.clear();
	program.itemSprites.clear();
	program.logicSprites.clear();
	program.robotSprites.clear();
	program.animationSprites.clear();
	int begY = int((-program.halfWindowHeight / (GC::tileSize / program.zoom)) - 2 + (cameraPos.y >> GC::tileShift));
	int endY = int(program.halfWindowHeight / (GC::tileSize / program.zoom) + (cameraPos.y >> GC::tileShift) + 2);
	int begX = int((-program.halfWindowWidth / (GC::tileSize / program.zoom)) - 2 + (cameraPos.x >> GC::tileShift));
	int endX = int(program.halfWindowWidth / (GC::tileSize / program.zoom) + (cameraPos.x >> GC::tileShift) + 2);
	program.tilesRendered = (endY - begY) * (endX - begX);
	UpdateElementExists();
	for (int y = begY; y < endY; y++)
	{
		for (int x = begX; x <= endX; x++)
		{
			Pos screenPos = Pos{ x, y } * GC::tileSize - Pos{ GC::halfTileSize,GC::halfTileSize };
			Pos tilePos = Pos{ x,y };
			uint64_t encodedPos = tilePos.CoordToEncoded();
			if (program.redrawGround)
				RecreateGroundSprites(tilePos, float(screenPos.x), float(screenPos.y));
			if (elementExists.find(encodedPos) != elementExists.end()) {
				RecreateItemSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateLogicSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateRobotSprites(encodedPos, float(screenPos.x), float(screenPos.y));
				RecreateAnimationSprites(encodedPos, float(screenPos.x), float(screenPos.y));
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
	CreateText(int(program.halfWindowWidth) - 100, int(-program.halfWindowHeight) + 10, buffer);
}
void ProgramData::DrawTooltips()
{
	if (ItemTile * tile = world.GetItemTile(program.mouseHovering.CoordToEncoded()))
	{
		if (tile->itemTile > 2)
		{
			std::string quantity = std::to_string(tile->quantity);
			CreateText(program.mousePos.x, program.mousePos.y - 20, program.itemTooltips[tile->itemTile] + " " + quantity);
		}
	}
	else if (program.selectedLogicTile)
	{
		CreateText(program.mousePos.x, program.mousePos.y - 20, program.selectedLogicTile->GetTooltip());
	}
#ifdef DEBUG
	char buffer[50];
	sprintf_s(buffer, "Map x/y: %d/%d", program.mouseHovering.x, program.mouseHovering.y);
	std::string displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 40, displayValue);

	sprintf_s(buffer, "Screen x/y: %d/%d", program.mousePos.x, program.mousePos.y);
	displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 60, displayValue);

	sprintf_s(buffer, "Zoom/Scale: %.2f/%.2f", program.zoom, program.scale);
	displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 80, displayValue);

	sprintf_s(buffer, "Camera x/y: %d/%d", program.cameraPos.x, program.cameraPos.y);
	displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 100, displayValue);

	sprintf_s(buffer, "Tiles Rendered: %d", program.tilesRendered);
	displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 120, displayValue);
#endif
}
void ProgramData::DrawSelectedBox()
{
	if (program.selectedLogicTile && !program.hoveringHotbar)
	{
		sf::RectangleShape selectionBox;
		selectionBox.setSize(sf::Vector2f(32, 32));
		selectionBox.setFillColor(sf::Color(0, 0, 0, 0));
		selectionBox.setOutlineColor(sf::Color(255, 255, 0, 255));
		selectionBox.setOutlineThickness(2);
		Pos drawPos = program.mouseHovering * GC::tileSize - Pos{ GC::halfTileSize,GC::halfTileSize };
		selectionBox.setPosition(float(drawPos.x), float(drawPos.y));
		program.selectionBoxes.emplace_back(selectionBox);
	}
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
void ProgramData::DrawHotbar()
{
	program.hotbarSprites.clear();
	program.hotbarSlots.clear();
	for (uint8_t i = 0; i < program.hotbarSize && i < program.hotbar.size(); ++i)
	{
		int x = int(i - (program.hotbarSize - 1) / 2.f) * (GC::hotbarSlotSize + GC::hotbarPadding);
		int y = int(program.windowHeight / 2.f) - GC::hotbarSlotSize;
		sf::RectangleShape hotbarSlot;
		if(i == program.hotbarIndex)
			hotbarSlot.setFillColor(sf::Color(200, 200, 200, 100));
		else
			hotbarSlot.setFillColor(sf::Color(50, 50, 50, 100));
		hotbarSlot.setSize(sf::Vector2f(GC::hotbarSlotSize, GC::hotbarSlotSize));
		hotbarSlot.setPosition(sf::Vector2f(x - GC::hotbarSlotSize / 2.f, y - GC::hotbarSlotSize / 2.f));
		program.hotbarSlots.emplace_back(hotbarSlot);

		if (program.hotbar[i])
		{
			program.hotbar[i]->facing = program.placeRotation;
			program.hotbar[i]->colorClass = program.placeColor;
			program.hotbar[i]->DrawTile(&program.hotbarSprites, float(x - GC::halfTileSize), float(y - GC::halfTileSize), 1.0f);
		}
	}
}
void ProgramData::CreateText(int x, int y, std::string input)
{
	sf::Text text;
	text.setString(input);
	text.setFont(program.guiFont);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(14);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.width / 2, textRect.height / 2);
	text.setPosition(float(x), float(y));

	sf::RectangleShape backPlane;
	backPlane.setSize(sf::Vector2f(textRect.width + GC::tooltipPadding, textRect.height + GC::tooltipPadding));
	backPlane.setFillColor(sf::Color(0, 0, 0, 100));
	backPlane.setOrigin(textRect.width / 2, textRect.height / 2);
	backPlane.setPosition(float(x), float(y));

	program.textBoxes.emplace_back(backPlane);
	program.textOverlay.emplace_back(text);
}
void ProgramData::FindMovingRobot()
{
	if (program.selectedRobot)
	{
		if (uint64_t* movingTo = world.nextRobotPos.GetValue(program.selectedRobot->pos))
		{
			Pos difference = Pos::EncodedToCoord(*movingTo) - program.selectedRobot->pos;
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			program.cameraPos = (program.selectedRobot->pos << GC::tileShift) + Pos{ int(float(difference.x) * float(GC::tileSize) * step) ,int(float(difference.y) * float(GC::tileSize) * step) };
		}
		else
		{
			program.cameraPos = (program.selectedRobot->pos << GC::tileShift);
		}
	}
}
void ProgramData::DrawGameState(sf::RenderWindow& window) {
	//if (!program.gamePaused)
	//{
		FindMovingRobot();
		if (program.prevZoom != program.zoom)
		{
			program.redrawGround = true;
			program.worldView.zoom(program.zoom / program.prevZoom);
			program.prevZoom = program.zoom;
		}
		if (program.prevCameraPos != program.cameraPos)
		{
			program.redrawGround = true;
			program.worldView.move(sf::Vector2f(float(program.cameraPos.x - program.prevCameraPos.x), float(program.cameraPos.y - program.prevCameraPos.y)));
			program.prevCameraPos = program.cameraPos;
		}
		program.textOverlay.clear();
		program.textBoxes.clear();
		program.selectionBoxes.clear();
		RecreateSprites();
		DrawUpdateCounter();
		DrawTooltips();
		DrawHotbar();
		DrawSelectedBox();
	//}
	window.setView(program.worldView);
	program.groundSprites.draw(window);
	program.logicSprites.draw(window);
	program.itemSprites.draw(window);
	program.animationSprites.draw(window);
	program.robotSprites.draw(window);
	for (sf::RectangleShape sprite : program.selectionBoxes)
	{
		window.draw(sprite);
	}
	window.setView(program.hudView);
	for (sf::RectangleShape sprite : program.hotbarSlots)
	{
		window.draw(sprite);
	}
	program.hotbarSprites.draw(window);
	for (sf::RectangleShape sprite : program.textBoxes)
	{
		window.draw(sprite);
	}
	for (sf::Text sprite : program.textOverlay)
	{
		window.draw(sprite);
	}
#ifdef DEBUG
	//DrawCrosshair(window);
#endif
}

void ProgramData::SwapBots()
{
	for (std::pair<uint64_t, uint64_t> newPos : world.nextRobotPos)
	{
		world.robots[newPos.second] = world.robots[newPos.first];
		world.robots[newPos.second].pos = Pos::EncodedToCoord(newPos.second);
		// Be sure to update which robot the players controlling before removing the old copy
		if (&world.robots[newPos.first] == program.selectedRobot)
		{
			program.selectedRobot = &world.robots[newPos.second];
		}
		world.robots.erase(newPos.first);

		// If the robot landed on a logic tile, apply its logic this tick
		if (auto temp = world.logictiles.GetValue(newPos.second))
		{
			LogicTile* logicTile = *temp;
			logicTile->DoRobotLogic(&world.robots[newPos.second]);
		}

		// If the robot left a logic tile, update its logic
		if (auto temp = world.logictiles.GetValue(newPos.first))
		{
			LogicTile* logicTile = *temp;
			logicTile->DoRobotLogic(nullptr);
		}
	}
	world.nextRobotPos.clear();
}

void ProgramData::MoveBots()
{
	SwapBots();
	for (MyMap<uint64_t,Robot>::iterator robotIter = world.robots.begin(); robotIter != world.robots.end(); robotIter++)
	{
		if (&robotIter->second != program.selectedRobot)
			robotIter->second.Move();
	}
}

void ProgramData::SwapItems()
{
	world.itemPrevMoved.clear();
	while (!world.nextItemPos.empty())
	{
		for (auto iter = world.nextItemPos.begin(); iter != world.nextItemPos.end(); )
		{
			// If item is successfully moved then remove it from the map. Else keep trying
			ItemTile* item = world.items.GetValue(iter->first);
			bool add = world.ChangeItem(Pos::EncodedToCoord(iter->second), item->itemTile, 1);
			if (add)
			{
				world.itemPrevMoved.insert({ iter->second });
				bool remove = world.ChangeItem(Pos::EncodedToCoord(iter->first), item->itemTile, -1);
				if (remove)
				{
					iter = world.nextItemPos.erase(iter);
				}
			}
			else
			{
				iter++;
			}
		}
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
		if(ItemTile* item = world.items.GetValue(wasMoving))
		CraftingClass::TryCrafting(item->itemTile, Pos::EncodedToCoord(wasMoving));
	}
}

void ProgramData::UpdateMap()
{
	// Update all the logic tiles that were queued last tick
	world.updateQueueA = MySet<uint64_t>(world.updateQueueC);
	world.updateQueueC.clear();
	do {
		for (uint64_t kv : world.updateQueueA)
		{
			if (LogicTile* logic = world.GetLogicTile(kv))
			{
				logic->DoWireLogic();
			}
		}
		world.updateQueueA = MySet<uint64_t>(world.updateQueueB);
		world.updateQueueB.clear();
	} while (world.updateQueueA.size() != 0);
	// Decrease the number of ticks for each active recipe and complete recipes that reach 0
	for (auto iter = world.craftingQueue.begin(); iter != world.craftingQueue.end(); )
	{
		iter->second.ticks--;
		// Erase all 0 tick recipes from craftingQueue
		if (iter->second.ticks == 0)
			program.craftingRecipes[iter->second.craftingRecipe].SuccessfulCraft(iter->second.pos);
		if (iter->second.ticks == 0) // Check twice because SuccessfulCraft() may add ticks to queue a new craft
			iter = world.craftingQueue.erase(iter);
		else
			++iter;
	}
	for (uint64_t kv : world.updateQueueD)
	{
		if (LogicTile* logic = world.GetLogicTile(kv))
		{
			logic->DoItemLogic();
		}
	}
	world.prevItemMovingTo = world.itemMovingTo;
	world.itemMovingTo.clear();
	world.robotMovingTo.clear();
	world.platformMovingTo.clear();
	SwapItems();
	MoveBots();
	CheckItemsMoved();
	++world.tick;
}

