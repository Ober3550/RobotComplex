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

void ProgramData::RecreateGroundSprites(Pos tilePos, int x, int y)
{
	if (GroundTile * tile = world.GetGroundTile(tilePos))
	{
		sf::Sprite sprite;
		uint8_t textureIndex = tile->groundTile;
		sprite.setTexture(*groundTextures[textureIndex / 4]);
		sprite.setTextureRect(sf::IntRect((textureIndex & 3)*32, 0, 32, 32));
		sprite.setPosition(float(x), float(y));
		program.groundSprites.emplace_back(sprite);
	}
}
void ProgramData::DrawItem(ItemTile item, int x, int y)
{
	sf::Sprite sprite;
	sprite.setTexture(*itemTextures[item.itemTile]);
	sprite.setOrigin(GC::halfItemSprite, GC::halfItemSprite);
	sprite.setPosition(x + GC::halfItemSprite, y + GC::halfItemSprite);
	program.itemSprites.emplace_back(sprite);
}
void ProgramData::RecreateItemSprites(uint64_t encodedPos, int x, int y)
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
				Pos screenPos = { x + int(float(difference.x) * float(GC::tileSize) * step), y + int(float(difference.y) * float(GC::tileSize) * step) };
				DrawItem(*tile, screenPos.x, screenPos.y);
			}
			else
				DrawItem(*tile, x, y);
		}
	}
}
void ProgramData::RecreateLogicSprites(uint64_t encodedPos, int x, int y)
{
	if (LogicTile * logic = world.GetLogicTile(encodedPos))
	{
		logic->DrawTile(x, y);
	}
}
void ProgramData::RecreateRobotSprites(uint64_t encodedPos, int x, int y)
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
			Pos screenPos = { x + int(float(difference.x) * float(GC::tileSize) * step), y + int(float(difference.y) * float(GC::tileSize) * step) };
			robot->DrawTile(screenPos.x, screenPos.y);
		}
		else
			robot->DrawTile(x, y);
	}
}
void ProgramData::RecreateAnimationSprites(uint64_t encodedPos, int x, int y)
{
	if (CraftingProcess * recipe = world.GetCrafting(encodedPos))
	{
		CraftingClass craftingRef = program.craftingRecipes[recipe->craftingRecipe];
		recipe->DrawAnimation(x, y);

		// Crafting loading bar
		int craftbarHeight = 10;
		float loadingBarX = float(x) + (float(craftingRef.width) / 2.0f) * float(GC::tileSize) - float(GC::halfTileSize);
		float loadingBarY = float(y) - craftbarHeight / 2 + (float(craftingRef.height) / 2.0f) * float(GC::tileSize);
		sf::RectangleShape craftTotal;
		craftTotal.setFillColor(sf::Color(50, 50, 50));
		craftTotal.setSize(sf::Vector2f(float(GC::tileSize), float(craftbarHeight) / 2.f));
		craftTotal.setPosition(loadingBarX, loadingBarY);
		program.textBoxes.emplace_back(craftTotal);

		sf::RectangleShape craftPercent;
		craftPercent.setFillColor(sf::Color(80, 140, 200));
		craftPercent.setSize(sf::Vector2f((float(recipe->totalTicks - recipe->ticks) / float(recipe->totalTicks))*GC::tileSize, float(craftbarHeight) / 2.f));
		craftPercent.setPosition(loadingBarX, loadingBarY);
		program.textBoxes.emplace_back(craftPercent);
	}
}
void ProgramData::RecreateSprites() {
	if (program.redrawStatic)
		program.groundSprites.clear();
	program.itemSprites.clear();
	program.logicSprites.clear();
	program.robotSprites.clear();
	program.animationSprites.clear();
	if (program.selectedRobot)
	{
		if (auto temp = world.nextRobotPos.GetValue(program.selectedRobot->pos))
		{
			Pos newPos = Pos::EncodedToCoord(*temp);
			Pos diff = newPos - program.selectedRobot->pos;
			float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
			if (step > 1.0f)
				step = 1.0f;
			program.cameraPos = (program.selectedRobot->pos << GC::tileShift) + Pos{ int(float(diff.x) * float(GC::tileSize) * step), int(float(diff.y) * float(GC::tileSize) * step) };
			program.redrawStatic = true;
		}
	}
	for (int y = -(program.windowHeight >> 1) - (program.cameraPos.y & GC::tileMask); y < (program.windowHeight >> 1); y += GC::tileSize)
	{
		for (int x = -(program.windowWidth >> 1) - (program.cameraPos.x & GC::tileMask); x < (program.windowWidth >> 1); x += GC::tileSize)
		{
			Pos tilePos = (Pos{ x,y } +program.cameraPos) >> GC::tileShift;
			uint64_t encodedPos = tilePos.CoordToEncoded();
			if (program.redrawStatic)
				RecreateGroundSprites(tilePos, x, y);
			RecreateItemSprites(encodedPos, x, y);
			RecreateLogicSprites(encodedPos, x, y);
			RecreateRobotSprites(encodedPos, x, y);
			RecreateAnimationSprites(encodedPos, x, y);
		}
	}
}
void ProgramData::DrawUpdateCounter()
{
	char buffer[50];
	sprintf_s(buffer, "FPS/UPS: %.0f/%.0f", round(program.frameRate), round(program.updateRate));
	std::string displayValue = buffer;
	CreateText((program.windowWidth >> 1) - 100, -(program.windowHeight >> 1) + 10, buffer);
}
void ProgramData::DrawTooltips()
{
	Pos mouseHovering = (program.mousePos + program.cameraPos) >> GC::tileShift;
	if (!program.gamePaused)
	{
		if (ItemTile * tile = world.GetItemTile(mouseHovering.CoordToEncoded()))
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
	}
#ifdef DEBUG
	char buffer[50];
	sprintf_s(buffer, "Map x/y: %d/%d", mouseHovering.x, mouseHovering.y);
	std::string displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 40, displayValue);

	sprintf_s(buffer, "Screen x/y: %d/%d", program.mousePos.x, program.mousePos.y);
	displayValue = buffer;
	CreateText(program.mousePos.x, program.mousePos.y - 60, displayValue);
#endif
}
void ProgramData::DrawSelectedBox()
{
	if (program.selectedLogicTile)
	{
		Pos selectedPosition = (program.selectedLogicTile->pos << GC::tileShift) - program.cameraPos;
		sf::RectangleShape selectionBox;
		selectionBox.setSize(sf::Vector2f(32, 32));
		selectionBox.setFillColor(sf::Color(0, 0, 0, 0));
		selectionBox.setOutlineColor(sf::Color(0, 0, 0, 255));
		selectionBox.setOutlineThickness(1);
		selectionBox.setPosition(float(selectedPosition.x), float(selectedPosition.y + 7));
		program.textBoxes.emplace_back(selectionBox);
	}
}
void ProgramData::DrawHotbar()
{
	program.hotbarSlots.clear();
	for (uint8_t i = 0; i < program.hotbarSize && i < program.hotbar.size(); ++i)
	{
		int x = int(i - program.hotbarSize / 2.f) * (GC::hotbarSlotSize + GC::hotbarPadding);
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
			program.hotbar[i]->DrawTile(x - GC::halfTileSize, y - GC::halfTileSize);
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
void ProgramData::DrawGameState(sf::RenderWindow& window) {
	if (!program.gamePaused)
	{
		program.redrawDynamic = true;
		program.textOverlay.clear();
		program.textBoxes.clear();
		RecreateSprites();
		DrawUpdateCounter();
		DrawTooltips();
		DrawHotbar();
		DrawSelectedBox();
		program.redrawDynamic = false;
		program.redrawStatic = false;
	}
	for (sf::Sprite sprite : program.groundSprites)
	{
		window.draw(sprite);
	}
	for (sf::Sprite sprite : program.logicSprites)
	{
		window.draw(sprite);
	}
	for (sf::Sprite sprite : program.itemSprites)
	{
		window.draw(sprite);
	}
	for (sf::Sprite sprite : program.animationSprites)
	{
		window.draw(sprite);
	}
	for (sf::Sprite sprite : program.robotSprites)
	{
		window.draw(sprite);
	}
	for (sf::RectangleShape sprite : program.hotbarSlots)
	{
		window.draw(sprite);
	}
	for (sf::RectangleShape sprite : program.textBoxes)
	{
		window.draw(sprite);
	}
	for (sf::Text sprite : program.textOverlay)
	{
		window.draw(sprite);
	}
}
void ProgramData::SwapItems()
{
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
					iter = world.nextItemPos.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
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
	world.itemPrevMoved.clear();
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
	SwapItems();
	MoveBots();
	CheckItemsMoved();
	++world.tick;
}

