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
void LogicTile::DrawSignalStrength(SpriteVector* appendTo, float x, float y, float s, int signal, uint8_t flags)
{
	if (signal > 0 && (flags & 1))
	{
		program.CreateSmallText(appendTo, std::to_string(signal), x, y, s, Align::centre);
	}
}

void LogicTile::DrawQuantity(SpriteVector* appendTo, float x, float y, float s, int quantity, uint8_t flags)
{
	program.CreateSmallText(appendTo, std::to_string(quantity), x + GC::halfTileSize, y + GC::halfTileSize, s, Align::right);
}

void LogicTile::DrawLogic(Pos currentPos, SpriteVector* appendTo, void* localMap, float x, float y, float s, uint8_t flags)
{
	// Centre Sprite
	sf::Sprite sprite;
	float sprite_rotation;

	uint8_t color = 0;
	if (this->signal)
		color = this->color;
	uint8_t Red, Green, Blue, Alpha;
	Red = 255 * (color & 1);
	Green = 255 * (color >> 1 & 1);
	Blue = 255 * (color >> 2 & 1);
	Alpha = 128 + 127 * !(flags >> 7 & 1);

	uint8_t Red2, Green2, Blue2, Alpha2;
	if (this->logicType == wirebridge)
	{
		uint8_t color2 = 0;
		if (this->signal2)
			color2 = this->color2;
		Red2 = 255 * (color2 & 1);
		Green2 = 255 * (color2 >> 1 & 1);
		Blue2 = 255 * (color2 >> 2 & 1);
		Alpha2 = 128 + 127 * !(flags >> 7 & 1);
	}

	sprite.setOrigin(GC::halfTileSize, GC::halfTileSize);
	sprite.setPosition(x + float(GC::halfTileSize), y + float(GC::halfTileSize));
	sprite.setScale(sf::Vector2f(s, s));

	// Neighbour Sprites
	for (uint8_t i = 0; i < 4; i++)
	{
		Pos lookingAt = currentPos.FacingPosition(Facing(i));
		auto neighbour = ((MyMap<uint64_t, LogicTile>*)localMap)->find(lookingAt);
		if (neighbour != ((MyMap<uint64_t, LogicTile>*)localMap)->end())
		{
			if (this->GetConnected(currentPos, lookingAt, neighbour->second)
				|| neighbour->second.GetConnected(lookingAt, currentPos, *this))
			{
				sprite.setTexture(*logicTextures[1]);
				sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));

				if (this->logicType == wirebridge)
				{
					if (int(Pos::RelativeFacing(this->facing, i)) & 1)
						sprite.setColor(sf::Color(Red2, Green2, Blue2, Alpha2));
					else
						sprite.setColor(sf::Color(Red, Green, Blue, Alpha));
				}
				else if (neighbour->second.GetSignal(*this))
					sprite.setColor(sf::Color(Red, Green, Blue, Alpha));
				else sprite.setColor(sf::Color(0, 0, 0, 128 + 127 * !(flags >> 7 & 1)));

				sprite_rotation = ((float)i) * (float)90.f;
				sprite.setRotation(sprite_rotation);

				appendTo->emplace_back(sprite);
			}
		}
	}

	bool textureFound = false;
	sprite_rotation = ((float)this->facing) * (float)90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setColor(sf::Color(Red, Green, Blue, Alpha));
	if (this->logicType == wirebridge)
		sprite.setTexture(*logicTextures[1]);
	else
	{
		auto texture = logicTextures.find(this->logicType);
		if (texture != logicTextures.end())
		{
			textureFound = true;
			sprite.setTexture(*texture->second);
		}
	}

	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
	if (this->logicType == wire || this->logicType == wirebridge)
	{
		sprite.setTextureRect(sf::IntRect(14, 14, 4, 4));
		sprite.setOrigin(2, 2);
		sprite.setPosition(x + float(GC::tileSize) / 2.f, y + float(GC::tileSize) / 2.f);
	}

	if (textureFound)
		appendTo->emplace_back(sprite);

	// Signal value
	this->DrawSignalStrength(appendTo, x - (5 * (this->logicType == wirebridge)), y, s, this->signal, flags);
	// Signal value
	if (this->logicType == wirebridge)
		this->DrawSignalStrength(appendTo, x + 5, y, s, this->signal2, flags);

	if (this->quantity > 1 && this->logicType != wirebridge || this->quantity > 2)
		DrawQuantity(appendTo, x, y, s, this->quantity, flags);
}
bool LogicTile::ShowAlign()
{
	if (this->logicType == redirector
		|| this->logicType == belt)
		return true;
	return false;
}