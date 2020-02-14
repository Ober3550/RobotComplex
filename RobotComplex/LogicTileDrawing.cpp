#include "Pos.h"
#include "Facing.h"
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
#include "Textures.h"
//static constexpr uint8_t colorClass = 1;

void LogicTile::DrawSpriteFromProperties(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture, sf::IntRect subRect, int rotation, bool inverse)
{
	// Output Sprite
	sf::Sprite sprite;
	sprite.setTexture(*texture);
	uint8_t Red, Green, Blue;
	
	if (inverse)
	{
		Red = this->signal ? 0 : 255 * (colorClass & 1);
		Green = this->signal ? 0 : 255 * (colorClass >> 1 & 1);
		Blue = this->signal ? 0 : 255 * (colorClass >> 2 & 1);
	}
	else 
	{
		Red = this->signal ? 255 * (colorClass & 1) : 0;
		Green = this->signal ? 255 * (colorClass >> 1 & 1) : 0;
		Blue = this->signal ? 255 * (colorClass >> 2 & 1) : 0;
	}
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	sprite.setTextureRect(subRect);

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(rotation) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s,s));

	appendTo->emplace_back(sprite);
}
void LogicTile::DrawSprite(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture)
{
	sf::Sprite sprite;
	sprite.setTexture(*texture);

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setColor(sf::Color(255, 255, 255, 255));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);
}
void LogicTile::DrawSprite(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture, sf::IntRect subRect)
{
	sf::Sprite sprite;
	sprite.setTexture(*texture);
	sprite.setTextureRect(subRect);

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);
}
void LogicTile::DrawSignalStrength(SpriteVector* appendTo, float x, float y, float s, int signal)
{
	// Signal value
	sf::Sprite sprite;
	std::string displayValue = std::to_string(signal);
	sprite.setTexture(*font);
	sprite.setOrigin(1, 2);
	for (int i = 0; i < (int)displayValue.length(); i++)
	{
		sprite.setTextureRect(program.fontMap[displayValue[i]]);
		float adjustLeft = (float(displayValue.length() / 2)) * 3;
		sprite.setPosition(float(x + s * float(GC::halfTileSize - adjustLeft + i * 4)),float(y + GC::halfTileSize));
		sprite.setScale(sf::Vector2f(s, s));

		appendTo->emplace_back(sprite);
	}
}

void Wire::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;
	sprite.setTexture(*texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	uint8_t color = black;
	if (this->signal)
		color = colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	
	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);
	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		Pos lookingAt = this->pos.FacingPosition(Facing(i));
		if (LogicTile* neighbour = world.GetLogicTile(lookingAt.CoordToEncoded()))
		{
			if (neighbour->GetConnected(this))
			{
				sprite.setTexture(*texture);
				sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

				sprite_rotation = ((float)i) * (float)90.f;
				sprite.setRotation(sprite_rotation);
				sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
				sprite.setScale(sf::Vector2f(s, s));

				appendTo->emplace_back(sprite);
			}
		}
	}
	// Signal value
	if(this->signal!=0)
	DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void Redirector::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Main Sprite
	sf::Sprite sprite;
	sprite.setTexture(*texture);
	
	uint8_t color = black;
	if (this->signal)
		color = colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void Inverter::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;
	sprite.setTexture(*Wire::texture);

	uint8_t Red, Green, Blue;
	uint8_t color;

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		Facing lookingAt = Pos::RelativeFacing(this->facing, i);
		if (LogicTile* neighbour = world.GetLogicTile(this->pos.FacingPosition(lookingAt).CoordToEncoded()))
		{
			color = black;
			if (neighbour->signal)
				color = neighbour->colorClass;
			if (neighbour->GetConnected(this) && this->GetConnected(neighbour))
			{
				Red = 255 * (color & 1);
				Green = 255 * (color >> 1 & 1);
				Blue = 255 * (color >> 2 & 1);
				sprite.setColor(sf::Color(Red, Green, Blue, 255));
				
				sprite_rotation = ((float)lookingAt) * (float)90.f;
				sprite.setRotation(sprite_rotation);

				appendTo->emplace_back(sprite);
			}
		}
	}
	// Centre Sprite
	sprite.setTexture(*texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	color = black;
	if (this->signal)
		color = colorClass;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void Booster::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;
	sprite.setTexture(*Wire::texture);

	uint8_t Red, Green, Blue;
	uint8_t color;

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		Facing lookingAt = Pos::RelativeFacing(this->facing, i);
		if (LogicTile* neighbour = world.GetLogicTile(this->pos.FacingPosition(lookingAt).CoordToEncoded()))
		{
			color = black;
			if (neighbour->signal)
				color = neighbour->colorClass;
			if (neighbour->GetConnected(this) && this->GetConnected(neighbour))
			{
				Red = 255 * (color & 1);
				Green = 255 * (color >> 1 & 1);
				Blue = 255 * (color >> 2 & 1);
				sprite.setColor(sf::Color(Red, Green, Blue, 255));

				sprite_rotation = ((float)lookingAt) * (float)90.f;
				sprite.setRotation(sprite_rotation);

				appendTo->emplace_back(sprite);
			}
		}
	}
	// Centre Sprite
	sprite.setTexture(*texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	color = black;
	if (this->signal)
		color = colorClass;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
		DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void Comparer::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;
	sprite.setTexture(*Wire::texture);

	uint8_t Red, Green, Blue;
	uint8_t color;

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		Facing lookingAt = Pos::RelativeFacing(this->facing, i);
		if (LogicTile* neighbour = world.GetLogicTile(this->pos.FacingPosition(lookingAt).CoordToEncoded()))
		{
			color = black;
			if (neighbour->signal)
				color = neighbour->colorClass;
			if (neighbour->GetConnected(this) && this->GetConnected(neighbour))
			{
				Red = 255 * (color & 1);
				Green = 255 * (color >> 1 & 1);
				Blue = 255 * (color >> 2 & 1);
				sprite.setColor(sf::Color(Red, Green, Blue, 255));

				sprite_rotation = ((float)lookingAt) * (float)90.f;
				sprite.setRotation(sprite_rotation);

				appendTo->emplace_back(sprite);
			}
		}
	}
	// Centre Sprite
	sprite.setTexture(*texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	color = black;
	if (this->signal)
		color = colorClass;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
		DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void Repeater::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Input Sprite
	sf::Sprite sprite;
	sprite.setTexture(*texture);
	sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

	uint8_t color = black;
	if (this->signal)
		color = colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Output Sprite
	sprite.setTexture(*texture);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
	DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void Counter::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Baseplate
	sf::Sprite sprite;
	sprite.setTexture(*texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	std::string counterDisplay = std::to_string(signal) + " / " + std::to_string(maxSignal);
	sf::Text counterValue;
	counterValue.setFont(program.guiFont);
	counterValue.setString(counterDisplay);
	counterValue.setCharacterSize(10);
	counterValue.setFillColor(sf::Color::Black);
	sf::FloatRect textRect = counterValue.getLocalBounds();
	counterValue.setOrigin(textRect.width / 2, textRect.height / 2);
	counterValue.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	counterValue.setScale(sf::Vector2f(s, s));

	program.textOverlay.emplace_back(counterValue);
}

void Belt::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Main Sprite
	sf::Sprite sprite;
	sprite.setTexture(*texture);

	uint8_t color = black;
	if (this->signal)
		color = colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
		DrawSignalStrength(appendTo, x, y, s, this->signal);
}

void WireBridge::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;
	sprite.setTexture(*Wire::texture);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

	uint8_t color = black;
	if (this->signal)
		color = GC::colorClassA;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);

	uint8_t color2 = black;
	if (this->signal2)
		color2 = GC::colorClassB;
	uint8_t Red2, Green2, Blue2;
	Red2 = 255 * (color2 & 1);
	Green2 = 255 * (color2 >> 1 & 1);
	Blue2 = 255 * (color2 >> 2 & 1);
	
	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);
	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		
		Pos lookingAt = this->pos.FacingPosition(Facing(i));
		if (LogicTile* neighbour = world.GetLogicTile(lookingAt.CoordToEncoded()))
		{
			if (this->GetConnected(neighbour))
			{
				sprite.setTexture(*Wire::texture);
				sprite.setTextureRect(sf::IntRect(32, 0, 32, 32));

				if (int(Pos::RelativeFacing(this->facing, i)) & 1)
					sprite.setColor(sf::Color(Red2, Green2, Blue2, 255));
				else
					sprite.setColor(sf::Color(Red, Green, Blue, 255));

				sprite_rotation = ((float)i) * (float)90.f;
				sprite.setRotation(sprite_rotation);
				sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
				sprite.setScale(sf::Vector2f(s, s));

				appendTo->emplace_back(sprite);
			}
		}
	}
	// Signal value
	if (this->signal != 0)
		DrawSignalStrength(appendTo, x - 5, y, s, this->signal);

	// Signal value
	if (this->signal2 != 0)
		DrawSignalStrength(appendTo, x + 5, y, s, this->signal2);
}

void Gate::DrawTile(SpriteVector* appendTo, float x, float y, float s)
{
	// Main Sprite
	sf::Sprite sprite;
	sprite.setTexture(*texture);

	uint8_t color = black;
	if (this->signal)
		color = colorClass;
	uint8_t Red, Green, Blue;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	sprite.setColor(sf::Color(Red, Green, Blue, 255));

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	float sprite_rotation = float(this->facing) * 90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	appendTo->emplace_back(sprite);

	// Signal value
	if (this->signal != 0)
		DrawSignalStrength(appendTo, x, y, s, this->signal);
}