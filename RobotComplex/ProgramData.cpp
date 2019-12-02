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
#include "SpriteGenerator.h"

void ProgramData::RecreateGroundSprites(Pos tilePos, int x, int y)
{
	if (GroundTile * tile = world.GetGroundTile(tilePos))
	{
		sf::Sprite sprite;
		sprite.setTexture(program.groundTextures[tile->groundTile]);
		sprite.setPosition(float(x), float(y));
		program.groundSprites.emplace_back(sprite);
	}
}
void ProgramData::RecreateItemSprites(uint64_t encodedPos, int x, int y)
{
	if (ItemTile * tile = world.GetItemTile(encodedPos))
	{
		if (tile->itemTile > 2)
		{
			sf::Sprite sprite;
			sprite.setTexture(program.itemTextures[tile->itemTile]);
			sprite.setOrigin(Gconstants::halfItemSprite, Gconstants::halfItemSprite);
			sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));
			program.itemSprites.emplace_back(sprite);
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
	if (Robot * robot = world.GetRobot(encodedPos))
	{
		robot->DrawTile(x, y);
	}
}
void ProgramData::RecreateAnimationSprites(uint64_t encodedPos, int x, int y)
{
	if (CraftingProcess * recipe = world.GetCrafting(encodedPos))
	{
		CraftingClass craftingRef = program.craftingRecipes[recipe->craftingRecipe];
		if (craftingRef.animationTextureRef > 0)	// Default animation is none
		{
			sf::Sprite sprite;
			sprite.setTexture(program.animationTextures[craftingRef.animationTextureRef - 1]);
			sf::FloatRect spriteRect = sprite.getGlobalBounds();
			int8_t animationPosition = MyMod(recipe->ticks / craftingRef.animationSpeed, (craftingRef.animationFrames << 1) - 1);
			if (animationPosition >= craftingRef.animationFrames)
			{
				animationPosition = -animationPosition + craftingRef.animationFrames;
			}
			sf::IntRect subRect(int(animationPosition * craftingRef.animationJump), 0, int(craftingRef.animationJump), int(spriteRect.height));
			sprite.setTextureRect(subRect);
			sprite.setPosition(float(x + craftingRef.animationOffset.x), float(y + craftingRef.animationOffset.y));
			program.animationSprites.emplace_back(sprite);
		}
	}
}
void ProgramData::RecreateSprites() {
	if (program.redrawStatic)
		program.groundSprites.clear();
	program.itemSprites.clear();
	program.logicSprites.clear();
	program.robotSprites.clear();
	program.animationSprites.clear();
	program.animations.clear();
	for (int y = -(program.windowHeight >> 1) - (program.cameraPos.y & Gconstants::tileMask); y < (program.windowHeight >> 1); y += Gconstants::tileSize)
	{
		for (int x = -(program.windowWidth >> 1) - (program.cameraPos.x & Gconstants::tileMask); x < (program.windowWidth >> 1); x += Gconstants::tileSize)
		{
			Pos tilePos = (Pos{ x,y } +program.cameraPos) >> Gconstants::tileShift;
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
	int x = (program.windowWidth >> 1) - 100;
	int y = -(program.windowHeight >> 1) + 10;

	char buffer[50];
	sprintf_s(buffer, "%.2f", program.frameRate);
	std::string displayValue = buffer;

	sf::Text text;
	text.setFont(program.guiFont);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(10);
	text.setPosition(float(x), float(y));
	text.setString(displayValue);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.width / 2, textRect.height / 2);

	sf::RectangleShape backPlane;
	backPlane.setPosition(float(x), float(y));
	backPlane.setSize(sf::Vector2f(textRect.width + Gconstants::tooltipPadding, textRect.height + Gconstants::tooltipPadding));
	backPlane.setFillColor(sf::Color(0, 0, 0, 100));
	backPlane.setOrigin(textRect.width / 2, textRect.height / 2);

	program.textBoxes.emplace_back(backPlane);
	program.textOverlay.emplace_back(text);
}
void ProgramData::DrawTooltips()
{
	Pos mouseHovering = (program.mousePos + program.cameraPos) >> Gconstants::tileShift;
	if (ItemTile * tile = world.GetItemTile(mouseHovering.CoordToEncoded()))
	{
		if (tile->itemTile > 2)
		{
			int x = program.mousePos.x;
			int y = program.mousePos.y - 20;
			sf::Text text;
			std::string quantity = std::to_string(tile->quantity);
			text.setString(program.itemTooltips[tile->itemTile] + " " + quantity);
			text.setFont(program.guiFont);
			text.setFillColor(sf::Color::White);
			text.setCharacterSize(14);
			sf::FloatRect textRect = text.getLocalBounds();
			text.setOrigin(textRect.width / 2, textRect.height / 2);
			text.setPosition(float(x), float(y));

			sf::RectangleShape backPlane;
			backPlane.setSize(sf::Vector2f(textRect.width + Gconstants::tooltipPadding, textRect.height + Gconstants::tooltipPadding));
			backPlane.setFillColor(sf::Color(0, 0, 0, 100));
			backPlane.setOrigin(textRect.width / 2, textRect.height / 2);
			backPlane.setPosition(float(x), float(y));

			program.textBoxes.emplace_back(backPlane);
			program.textOverlay.emplace_back(text);
		}
	}
	if (program.selectedLogicTile)
	{
		sf::Text text;
		int x = program.mousePos.x;
		int y = program.mousePos.y - 20;
		text.setString(program.selectedLogicTile->GetTooltip());
		text.setFont(program.guiFont);
		text.setFillColor(sf::Color::White);
		text.setCharacterSize(14);
		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.width / 2, textRect.height / 2);
		text.setPosition(float(x), float(y));

		sf::RectangleShape backPlane;
		backPlane.setSize(sf::Vector2f(textRect.width + Gconstants::tooltipPadding, textRect.height + Gconstants::tooltipPadding));
		backPlane.setFillColor(sf::Color(0, 0, 0, 100));
		backPlane.setOrigin(textRect.width / 2, textRect.height / 2);
		backPlane.setPosition(float(x), float(y));

		program.textBoxes.emplace_back(backPlane);
		program.textOverlay.emplace_back(text);
	}
}
void ProgramData::DrawSelectedBox()
{
	if (program.selectedLogicTile)
	{
		Pos selectedPosition = (program.selectedLogicTile->pos << Gconstants::tileShift) - program.cameraPos;
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
		int x = int(i - program.hotbarSize / 2.f) * (Gconstants::hotbarSlotSize + Gconstants::hotbarPadding);
		int y = int(program.windowHeight / 2.f) - Gconstants::hotbarSlotSize;
		sf::RectangleShape hotbarSlot;
		if(i == program.hotbarIndex)
			hotbarSlot.setFillColor(sf::Color(200, 200, 200, 100));
		else
			hotbarSlot.setFillColor(sf::Color(50, 50, 50, 100));
		hotbarSlot.setSize(sf::Vector2f(Gconstants::hotbarSlotSize, Gconstants::hotbarSlotSize));
		hotbarSlot.setPosition(sf::Vector2f(x - Gconstants::hotbarSlotSize / 2.f, y - Gconstants::hotbarSlotSize / 2.f));
		program.hotbarSlots.emplace_back(hotbarSlot);

		if (program.hotbar[i])
		{
			program.hotbar[i]->facing = program.placeRotation;
			program.hotbar[i]->DrawTile(x - Gconstants::halfTileSize, y - Gconstants::halfTileSize);
		}
	}
}
void ProgramData::DrawGameState(sf::RenderWindow& window) {
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
	for (sf::Sprite sprite : program.robotSprites)
	{
		window.draw(sprite);
	}
	for (sf::Sprite sprite : program.animationSprites)
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

