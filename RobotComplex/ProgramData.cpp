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
#include <typeinfo>
#include <cmath>

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

void ProgramData::RecreateGroundSprites(Pos tilePos, float x, float y)
{
	GroundTile * tile = world.GetGroundTile(tilePos);
	sf::Sprite sprite;
	uint8_t textureIndex = tile->groundTile;
	if (textureIndex > program.maxGround)
		program.maxGround = textureIndex;
	if (textureIndex < program.minGround)
		program.minGround = textureIndex;
	sprite.setTexture(*groundTextures[0]);
	sprite.setTextureRect(sf::IntRect((textureIndex / 32)*32, 0, 32, 32));
	sf::Color color = HSV2RGB(sf::Color(MyMod(textureIndex+16,256), 100 + MyMod(textureIndex,32), 220, 255 ));
	sprite.setColor(color);
	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(float(x + GC::halfTileSize), float(y + GC::halfTileSize));
	program.groundSprites.emplace_back(sprite);
}
void ProgramData::RecreatePlatformSprites(uint64_t encodedPos, float x, float y)
{
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
}
void ProgramData::DrawItem(SpriteVector* appendTo,ItemTile item, float x, float y)
{
	sf::Sprite sprite;
	sprite.setTexture(*itemTextures[item.itemTile]);
	sprite.setOrigin(GC::halfItemSprite, GC::halfItemSprite);
	sprite.setPosition(x + 16.f, y + 16.f);
	appendTo->emplace_back(sprite);
}
void ProgramData::RecreateItemSprites(uint64_t encodedPos, float x, float y)
{
	if (ItemTile * tile = world.GetItemTile(encodedPos))
	{
		if (tile->itemTile >= ReservedItems::totalReserved)
		{
			if (!program.showDebugInfo)
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
			}
			DrawItem(&program.itemSprites,*tile, x, y);
		}
	}
}
void ProgramData::RecreateLogicSprites(uint64_t encodedPos, float x, float y)
{
	if (LogicTile * logic = world.GetLogicTile(encodedPos))
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
		logic->DrawTile(&program.logicSprites, x, y, 1.0f, uint8_t(program.showSignalStrength));
	}
}
void ProgramData::RecreateRobotSprites(uint64_t encodedPos, float x, float y)
{
	if (Robot* robot = world.GetRobot(encodedPos))
	{
		if (!program.showDebugInfo)
		{
			if (auto nextPos = world.nextRobotPos.GetValue(encodedPos))
			{
				Pos newPos = robot->pos.FacingPosition(*nextPos);
				Pos difference = newPos - robot->pos;
				float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
				if (step > 1.0f)
					step = 1.0f;
				x += float(difference.x) * float(GC::tileSize) * step;
				y += float(difference.y) * float(GC::tileSize) * step;
			}
			if (auto nextPos = world.nextPlatforms.GetValue(encodedPos))
			{
				Pos newPos = robot->pos.FacingPosition(*nextPos);
				Pos difference = newPos - robot->pos;
				float step = program.framesSinceTick / float(GC::FRAMERATE / GC::UPDATERATE);
				if (step > 1.0f)
					step = 1.0f;
				x += float(difference.x) * float(GC::tileSize) * step;
				y += float(difference.y) * float(GC::tileSize) * step;
			}
		}
		robot->DrawTile(&program.robotSprites,int(x), int(y), 1.0f);
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
	for (auto elem : world.platforms)
	{
		elementExists.insert({ elem.first });
	}
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
	if (program.redrawGround)
	{
		program.minGround = 255;
		program.maxGround = 0;
		program.groundSprites.clear();
	}
	program.platformSprites.clear();
	program.itemSprites.clear();
	program.logicSprites.clear();
	program.robotSprites.clear();
	program.animationSprites.clear();
	int begY = int((-program.halfWindowHeight / (GC::tileSize / program.zoom)) + (cameraPos.y >> GC::tileShift)) - 4;
	int endY = int(program.halfWindowHeight / (GC::tileSize / program.zoom) + (cameraPos.y >> GC::tileShift)) + 4;
	int begX = int((-program.halfWindowWidth / (GC::tileSize / program.zoom)) + (cameraPos.x >> GC::tileShift)) - 4;
	int endX = int(program.halfWindowWidth / (GC::tileSize / program.zoom) + (cameraPos.x >> GC::tileShift)) + 4;
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
				//RecreatePlatformSprites(encodedPos, float(screenPos.x), float(screenPos.y));
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
	CreateText(program.halfWindowWidth - 10, -program.halfWindowHeight + 10, buffer, Align::right);
}
void ProgramData::DrawTooltips()
{
	if (ItemTile * tile = world.GetItemTile(program.mouseHovering.CoordToEncoded()))
	{
		if (tile->itemTile > 2)
		{
			std::string quantity = std::to_string(tile->quantity);
			CreateText(float(program.mousePos.x), float(program.mousePos.y - 20), tile->GetTooltip() + " " + quantity, Align::centre);
		}
	}
	else if (program.selectedHotbar)
	{
		CreateText(float(program.mousePos.x), float(program.mousePos.y - 20), program.selectedHotbar->GetTooltip(), Align::centre);
	}
	else if (program.selectedLogicTile)
	{
		CreateText(float(program.mousePos.x), float(program.mousePos.y - 20), program.selectedLogicTile->GetTooltip(), Align::centre);
	}
}
void ProgramData::DrawSelectedBox(std::vector<sf::RectangleShape>* appendTo, Pos pos)
{
	sf::RectangleShape selectionBox;
	selectionBox.setSize(sf::Vector2f(32, 32));
	selectionBox.setFillColor(sf::Color(0, 0, 0, 0));
	selectionBox.setOutlineColor(sf::Color(255, 255, 0, 255));
	selectionBox.setOutlineThickness(2);
	Pos drawPos = pos * GC::tileSize - Pos{ GC::halfTileSize,GC::halfTileSize };
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
void ProgramData::DrawHotbar()
{
	program.hotbarSprites.clear();
	program.hotbarSlots.clear();
	for (uint8_t i = 0; i < program.hotbarSize && i < program.hotbar.size(); ++i)
	{
		int temp = program.hotbarSize;
		if (temp > 10)
			temp = 10;
		int x = int(MyMod(i,10) - temp / 2.f) * (GC::hotbarSlotSize + GC::hotbarPadding);
		int y = int(program.windowHeight / 2.f) - (1+i/10) * (GC::hotbarSlotSize + GC::hotbarPadding);
		sf::RectangleShape hotbarSlot;
		if(i == program.hotbarIndex)
			hotbarSlot.setFillColor(sf::Color(200, 200, 200, 100));
		else
			hotbarSlot.setFillColor(sf::Color(50, 50, 50, 100));
		hotbarSlot.setSize(sf::Vector2f(GC::hotbarSlotSize, GC::hotbarSlotSize));
		hotbarSlot.setPosition(sf::Vector2f(x - GC::hotbarSlotSize / 2.f, y - GC::hotbarSlotSize / 2.f));
		program.hotbarSlots.emplace_back(hotbarSlot);

		if (LogicTile* logic = dynamic_cast<LogicTile*> (program.hotbar[i]))
		{
			logic->pos = Pos{ INT_MAX,INT_MAX };
			logic->signal = GC::startSignalStrength;
			logic->facing = program.placeRotation;
			logic->colorClass = program.placeColor;
			logic->DrawTile(&program.hotbarSprites, float(x - GC::halfTileSize), float(y - GC::halfTileSize), 1.0f, 2);
		}
		else if (Robot* robot = dynamic_cast<Robot*> (program.hotbar[i]))
		{
			robot->facing = program.placeRotation;
			robot->DrawTile(&program.hotbarSprites,int(x - GC::halfTileSize), int(y - GC::halfTileSize), 1.0f);
		}
		else if (ItemTile* item = dynamic_cast<ItemTile*> (program.hotbar[i]))
		{
			DrawItem(&program.hotbarSprites, *item, float(x - GC::halfTileSize), float(y - GC::halfTileSize));
			if(item->quantity > 1)
			CreateSmallText(&program.hotbarSprites, std::to_string(item->quantity), float(x), float(y), 2.f, Align::right);
		}
	}
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

	

	program.unscaledBoxes.emplace_back(backPlane);
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
		sprite.setPosition(float(x + float(GC::halfTileSize - s * adjustLeft + s * i * 4)), float(y + GC::halfTileSize));
		sprite.setScale(sf::Vector2f(s, s));
		appendTo->emplace_back(sprite);
	}
}
void ProgramData::FindMovingRobot()
{
	if (program.selectedRobot)
	{
		if (Facing* movingTo = world.nextRobotPos.GetValue(program.selectedRobot->pos))
		{
			Pos difference = program.selectedRobot->pos.FacingPosition(*movingTo) - program.selectedRobot->pos;
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
void ProgramData::DrawDebugHUD()
{
	DrawUpdateCounter();
	char buffer[50];
	std::string displayValue;
	float lineNum = 0;
	float lineSpace = 20.f;

	sprintf_s(buffer, "Chunk Count: %d", world.worldChunks.size());
	displayValue = buffer;
	CreateText(-program.halfWindowWidth, -program.halfWindowHeight + lineSpace * lineNum, displayValue, Align::left);
	lineNum++;
	
	sprintf_s(buffer, "Platform Count: %d", world.platforms.size());
	displayValue = buffer;
	CreateText(-program.halfWindowWidth, -program.halfWindowHeight + lineSpace * lineNum, displayValue, Align::left);
	lineNum++;

	sprintf_s(buffer, "Logic Count: %d", world.logictiles.size());
	displayValue = buffer;
	CreateText(-program.halfWindowWidth, -program.halfWindowHeight + lineSpace * lineNum, displayValue, Align::left);
	lineNum++;

	sprintf_s(buffer, "Item Count: %d", world.items.size());
	displayValue = buffer;
	CreateText(-program.halfWindowWidth, -program.halfWindowHeight + lineSpace * lineNum, displayValue, Align::left);
	lineNum++;

	sprintf_s(buffer, "Robot Count: %d", world.robots.size());
	displayValue = buffer;
	CreateText(-program.halfWindowWidth, -program.halfWindowHeight + lineSpace * lineNum, displayValue, Align::left);
	lineNum++;

	lineNum = 0;
	//Mouse Components
	sprintf_s(buffer, "Map x/y: %d/%d", program.mouseHovering.x, program.mouseHovering.y);
	displayValue = buffer;
	CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
	lineNum++;

	sprintf_s(buffer, "Screen x/y: %d/%d", program.mousePos.x, program.mousePos.y);
	displayValue = buffer;
	CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
	lineNum++;

	sprintf_s(buffer, "Zoom/Scale: %.2f/%.2f", program.zoom, program.scale);
	displayValue = buffer;
	CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
	lineNum++;

	sprintf_s(buffer, "Camera x/y: %d/%d", program.cameraPos.x, program.cameraPos.y);
	displayValue = buffer;
	CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
	lineNum++;

	sprintf_s(buffer, "Tiles Rendered: %d", program.tilesRendered);
	displayValue = buffer;
	CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
	lineNum++;

	if (GroundTile* ground = world.GetGroundTile(program.mouseHovering))
	{
		sprintf_s(buffer, "Ground Value: %d", ground->groundTile);
		displayValue = buffer;
		CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
		lineNum++;
	}

	if (LogicTile* logic = world.GetLogicTile(program.mouseHovering))
	{
		sprintf_s(buffer, "Logic Signal Strength: %d", logic->signal);
		displayValue = buffer;
		CreateText(float(program.mousePos.x), program.mousePos.y - lineSpace * lineNum, displayValue, Align::centre);
		lineNum++;
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
			// Recalculate the possition of the mouse and what the new selection is after moving
			program.mouseHovering = ((program.mousePos * program.zoom) + program.cameraPos) / float(GC::tileSize);
		}
		program.textOverlay.clear();
		program.unscaledBoxes.clear();
		program.scaledBoxes.clear();
		RecreateSprites();
		DrawTooltips();
		DrawHotbar();
		if(program.selectedLogicTile && !program.hoveringHotbar)
			DrawSelectedBox(&program.scaledBoxes,program.mouseHovering);
		if (program.showDebugInfo)
			DrawDebugHUD();
	//}
	window.setView(program.worldView);
	program.groundSprites.draw(window);
	program.platformSprites.draw(window);
	program.logicSprites.draw(window);
	program.itemSprites.draw(window);
	program.animationSprites.draw(window);
	program.robotSprites.draw(window);
	for (sf::RectangleShape sprite : program.scaledBoxes)
	{
		window.draw(sprite);
	}
	for (sf::RectangleShape sprite : program.scaledPersistentBoxes)
	{
		window.draw(sprite);
	}
	window.setView(program.hudView);
	for (sf::RectangleShape sprite : program.hotbarSlots)
	{
		window.draw(sprite);
	}
	program.hotbarSprites.draw(window);
	for (sf::RectangleShape sprite : program.unscaledBoxes)
	{
		window.draw(sprite);
	}
	for (sf::Text sprite : program.textOverlay)
	{
		window.draw(sprite);
	}
}

void ProgramData::MovePlatform(Pos pos, Facing toward)
{
	if(program.showDebugInfo)
		Sleep(100);
	Pos newPos = pos.FacingPosition(toward);
	program.scaledPersistentBoxes.clear();
	DrawSelectedBox(&program.scaledPersistentBoxes, pos);
	DrawSelectedBox(&program.scaledPersistentBoxes, newPos);
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
		/*
		if (LogicTile* elem = world.GetLogicTile(pos))
		{
			elem->pos = newPos;
			world.logictiles[newPos.CoordToEncoded()] = world.logictiles[pos.CoordToEncoded()];
			world.logictiles.erase(pos.CoordToEncoded());
		}
		*/
		if (ItemTile* elem = world.GetItemTile(pos.CoordToEncoded()))
		{
			if (ItemTile* elem = world.GetItemTile(newPos.CoordToEncoded()))
			{
				world.updateQueueD.insert(pos.CoordToEncoded());
				world.updateQueueD.insert(newPos.CoordToEncoded());
			}
			else
			{
				world.updateQueueD.insert(newPos.CoordToEncoded());
				world.items[newPos.CoordToEncoded()] = world.items[pos.CoordToEncoded()];
				world.items.erase(pos.CoordToEncoded());
			}
		}
		if (Robot* elem = world.GetRobot(pos.CoordToEncoded()))
		{
			elem->pos = newPos;
			/*
			if (auto moving = world.nextRobotPos.GetValue(pos.CoordToEncoded()))
			{
				world.nextRobotPos.erase(pos.CoordToEncoded());
				world.nextRobotPos.insert({ newPos.CoordToEncoded(),toward });
			}*/
			world.robots[newPos.CoordToEncoded()] = world.robots[pos.CoordToEncoded()];
			world.robots.erase(pos.CoordToEncoded());
		}
		world.logictiles[newPos.CoordToEncoded()] = world.logictiles[pos.CoordToEncoded()];
		world.logictiles[newPos.CoordToEncoded()]->pos = newPos;
		world.logictiles.erase(pos.CoordToEncoded());
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
			world.robots[newPos.CoordToEncoded()] = world.robots[pos.CoordToEncoded()];
			world.robots[newPos.CoordToEncoded()].pos = newPos;
			// Be sure to update which robot the players controlling before removing the old copy
			if (&world.robots[pos.CoordToEncoded()] == program.selectedRobot)
			{
				program.selectedRobot = &world.robots[newPos.CoordToEncoded()];
			}
			world.robots.erase(pos.CoordToEncoded());

			// If the robot landed on a logic tile, apply its logic this tick
			if (auto temp = world.logictiles.GetValue(newPos.CoordToEncoded()))
			{
				LogicTile* logicTile = *temp;
				logicTile->DoRobotLogic(newPos);
			}

			// If the robot left a logic tile, update its logic
			if (auto temp = world.logictiles.GetValue(pos.CoordToEncoded()))
			{
				LogicTile* logicTile = *temp;
				logicTile->DoRobotLogic(pos);
			}
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

void ProgramData::MoveItem(Pos pos, Facing toward)
{
	Pos newPos = pos.FacingPosition(toward);
	// If there's a item infront and they're also moving, move them first
	if (auto frontItem = world.items.GetValue(newPos))
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
		world.ChangeItem(pos, item->itemTile, -1);
		world.ChangeItem(newPos, item->itemTile, 1);
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
		if(ItemTile* item = world.items.GetValue(wasMoving))
		CraftingClass::TryCrafting(item->itemTile, Pos::EncodedToCoord(wasMoving));
	}
}

void ProgramData::UpdateMap()
{
	world.platforms.clear();
	world.updateQueueA.clear();
	// Update all the logic tiles that were queued to finish this tick
	// Decrease the number of ticks for each active recipe and complete recipes that reach 0
	for (auto iter = world.updateQueueC.begin(); iter != world.updateQueueC.end(); )
	{
		iter->second--;
		// Erase all 0 tick recipes from craftingQueue
		if (iter->second == 0)
		{
			world.updateQueueA.insert({ iter->first });
			iter = world.updateQueueC.erase(iter);
		}
		else
			++iter;
	}
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
	world.prevItemMovingTo = world.itemMovingTo;
	world.itemMovingTo.clear();
	world.robotMovingTo.clear();
	world.platformMovingTo.clear();
	SwapItems();
	MoveBots();
	CheckItemsMoved();
	SwapPlatforms();
	for (uint64_t kv : world.updateQueueD)
	{
		if (LogicTile* logic = world.GetLogicTile(kv))
		{
			logic->DoItemLogic();
		}
	}
	world.updateQueueD.clear();
	++world.tick;
}

