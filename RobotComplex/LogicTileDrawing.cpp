#include "Pos.h"
#include "Facing.h"
#include "Active.h"
#include "LogicTile.h"
#include "LogicTypes.h"
#include "Robot.h"
#include <array>
#include "WorldSave.h"
#include "ProgramData.h"
#include "Constants.h"
#include <string>
#include <SFML/Graphics.hpp>
#include "RedirectorColors.h"

void LogicTile::DrawSpriteFromProperties(int x, int y, sf::Texture texture, sf::IntRect subRect, int rotation, bool inverse)
{
	// Output Sprite
	sf::Sprite sprite;
	sprite.setTexture(texture);
	uint8_t Red, Green, Blue;
	
	if (inverse)
	{
		Red = this->signal ? 0 : 255 * (this->colorClass & 1);
		Green = this->signal ? 0 : 255 * (this->colorClass >> 1 & 1);
		Blue = this->signal ? 0 : 255 * (this->colorClass >> 2 & 1);
	}
	else 
	{
		Red = this->signal ? 255 * (this->colorClass & 1) : 0;
		Green = this->signal ? 255 * (this->colorClass >> 1 & 1) : 0;
		Blue = this->signal ? 255 * (this->colorClass >> 2 & 1) : 0;
	}
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	sprite.setTextureRect(subRect);

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(rotation) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);
}
void LogicTile::DrawSprite(int x, int y, sf::Texture &texture)
{
	sf::Sprite sprite;
	sprite.setTexture(texture);

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));
	sprite.setColor(sf::Color(255, 255, 255, 255));

	program.logicSprites.emplace_back(sprite);
}
void LogicTile::DrawSprite(int x, int y, sf::Texture &texture, sf::IntRect subRect)
{
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setTextureRect(subRect);

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);
}
void LogicTile::DrawSignalStrength(int x, int y, int signal)
{
	// Signal value
	sf::Sprite signalStrength;
	std::string displayValue = std::to_string(signal);
	signalStrength.setTexture(program.font);
	signalStrength.setOrigin(1, 2);
	for (int i = 0; i < (int)displayValue.length(); i++)
	{
		signalStrength.setTextureRect(program.fontMap[displayValue[i]]);
		float adjustLeft = (float(displayValue.length() / 2)) * 3;
		signalStrength.setPosition(float(x + Gconstants::halfTileSize - adjustLeft + i * 4),float(y + Gconstants::halfTileSize));

		program.logicSprites.emplace_back(signalStrength);
	}
}

void Wire::DrawTile(int x, int y)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	uint8_t color = black;
	if (this->signal)
		color = this->colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	
	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);
	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		Pos lookingAt = this->pos.FacingPosition(Facing(i));
		if (LogicTile* neighbour = world.GetLogicTile(lookingAt.CoordToEncoded()))
		{
			if (neighbour->IsConnected(this->pos))
			{
				sprite.setTexture(texture);
				sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

				sprite_rotation = ((float)i) * (float)90.f;
				sprite.setRotation(sprite_rotation);
				sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

				program.logicSprites.emplace_back(sprite);
			}
		}
	}
	// Signal value
	if(this->signal!=0)
	DrawSignalStrength(x, y, this->signal);
}

void Redirector::DrawTile(int x, int y)
{
	// Main Sprite
	sf::Sprite sprite;
	sprite.setTexture(texture);
	
	uint8_t color = black;
	if (this->itemFilter == nothing)
		color = yellow;
	if (this->dropItem)
		color = white;
	if (this->signal)
		color = this->colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(x, y, this->signal);
}

void Inverter::DrawTile(int x, int y)
{
	// Centre Sprite
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	uint8_t color = black;
	if (!this->signal)
		color = this->colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Output Sprite
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	color = black;
	if (this->signal)
		color = this->colorClass;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));	

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(x, y, this->signal);
}

void Booster::DrawTile(int x, int y)
{
	// Input Sprite
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	uint8_t color = black;
	if (this->signal)
		color = this->colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Output Sprite
	sprite.setTexture(texture);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(x, y, this->signal);
}

void Repeater::DrawTile(int x, int y)
{
	// Input Sprite
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	uint8_t color = black;
	if (this->signal)
		color = this->colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Output Sprite
	sprite.setTexture(texture);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(x, y, this->signal);
}

void Memory::DrawTile(int x, int y)
{
	// Baseplate
	sf::Sprite sprite;
	sprite.setTexture(texture);
	uint8_t color = black;
	if (this->signal)
		color = this->colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);
	
	// Signal value
	sf::Sprite signalStrength;
	signalStrength.setTexture(program.font);
	signalStrength.setOrigin(1, 2);

	// Memory Slots
	sprite.setOrigin(3, 3);
	for (uint8_t i = 0; i < 16; i++)
	{
		sprite.setTexture(texture);
		if (memIndex == i)
			sprite.setColor(sf::Color(Red, Green, Blue, 255));
		else
			sprite.setColor(sf::Color(0, 0, 0, 255));
		sprite.setTextureRect(sf::IntRect(32, 0, 6, 6));

		sprite.setPosition(float(x + 4 + (i & 3) * 8), float(y + 4 + (i >> 2) * 8));

		program.logicSprites.emplace_back(sprite);

		std::string displayValue = std::to_string(this->memory[i]);
		for (int j = 0; j < (int)displayValue.length(); j++)
		{
			signalStrength.setTextureRect(program.fontMap[displayValue[j]]);
			float adjustLeft = (float(displayValue.length() / 2)) * 3;
			signalStrength.setPosition(float(x + 5 + (i & 3) * 8 - adjustLeft + j*3), float(y + 4 + (i >> 2) * 8));
			program.logicSprites.emplace_back(signalStrength);
		}
	}
}

void Counter::DrawTile(int x,int y)
{
	// Baseplate
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	sprite.setOrigin(Gconstants::halfTileSize, Gconstants::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));

	program.logicSprites.emplace_back(sprite);

	// Signal value
	std::string counterDisplay = std::to_string(this->signal) + " / " + std::to_string(this->maxSignal);
	sf::Text counterValue;
	counterValue.setFont(program.guiFont);
	counterValue.setString(counterDisplay);
	counterValue.setCharacterSize(10);
	counterValue.setFillColor(sf::Color::Black);
	sf::FloatRect textRect = counterValue.getLocalBounds();
	counterValue.setOrigin(textRect.width / 2, textRect.height / 2);
	counterValue.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));
	program.textOverlay.emplace_back(counterValue);
}