#pragma once
#include "Robot.h"
#include "Active.h"
#include "LogicTypes.h"
#include <cstdint>
#include <SFML/Graphics.hpp>
#include "ReservedItems.h"
#include "Constants.h"
#include <array>

class LogicTile {
public:
	Pos pos;								// 8 bytes
	Facing facing = north;					// 1 byte
	uint8_t prevSignal = -1;				// 1 byte
	uint8_t signal = 0;						// 1 byte
	uint8_t colorClass = 1;					// xxxxxBGR 8 color byte
	static sf::Texture texture;				// Empty texture
	virtual sf::Texture GetTexture() { return this->texture; };
	virtual void DoWireLogic();
	virtual void DoItemLogic() {};
	virtual void DoRobotLogic(Robot* robotRef) {};
	virtual void DrawTile(int x, int y) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual LogicTile* Copy() = 0;
	void BaseCopy(LogicTile*);		// Pass in a 'new' allocated memory address and have it populated with the originals properties
	void DrawSprite(int x, int y, sf::Texture& texture);
	void DrawSprite(int x, int y, sf::Texture& texture, sf::IntRect subRect);
	void DrawSpriteFromProperties(int x, int y, sf::Texture texture, sf::IntRect subRect, int rotation, bool inverse);
	void DrawSignalStrength(int x, int y, int signal);
};

class Wire : public LogicTile {
public:
	static sf::Texture texture;								// A textures for drawing
	sf::Texture GetTexture() { return this->texture; };
	void DrawTile(int x, int y);
	std::string GetTooltip();
	Wire* Copy()
	{
		Wire* logicTile = new Wire();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Redirector : public LogicTile {
public:
	static sf::Texture texture;					// A texture for drawing
	sf::Texture GetTexture() { return this->texture; };
	uint16_t itemFilter;						// 2 bytes
	bool dropItem;								// 1 bit
	Redirector() { this->itemFilter = 0; this->dropItem = false; }
	Redirector(uint16_t itemFilter, bool dropItem)
	{
		this->itemFilter = itemFilter;
		this->dropItem = dropItem;
	};
	Redirector(const Redirector* other) {
		this->itemFilter = other->itemFilter;
		this->dropItem = other->dropItem;
	}
	void DoRobotLogic(Robot* robotRef);						// Redirects robots when stepped onto
	void DrawTile(int x, int y);
	std::string GetTooltip();
	Redirector* Copy()
	{
		Redirector* logicTile = new Redirector();
		this->BaseCopy(logicTile);
		logicTile->itemFilter = this->itemFilter;
		logicTile->dropItem = this->dropItem;
		return logicTile;
	}
};

class PressurePlate : public LogicTile {
public:
	static sf::Texture texture;								// A textures for drawing
	sf::Texture GetTexture() { return this->texture; };
	void DoWireLogic() {}									// Overwrite base method to not transfer signals
	void DoItemLogic();										// Activates when an item of the same filter type is placed onto it
	void DoRobotLogic(Robot* robotRef);						// Activates when a robot holding the correct item filter steps onto it
	void DrawTile(int x, int y){
		this->facing = north;
		LogicTile::DrawSprite(x, y, this->texture);
	}
	std::string GetTooltip();
	PressurePlate* Copy()
	{
		PressurePlate* logicTile = new PressurePlate();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Inverter : public LogicTile {
public:
	static sf::Texture texture;								// A textures for drawing
	sf::Texture GetTexture() { return this->texture; };
	void DoWireLogic();								// Overrides wire logic transfer
	void DrawTile(int x, int y);
	std::string GetTooltip();
	Inverter* Copy()
	{
		Inverter* logicTile = new Inverter();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Booster : public LogicTile {
public:
	static sf::Texture texture;								// A textures for drawing
	sf::Texture GetTexture() { return this->texture; };
	void DoWireLogic();										// Overrides wire logic transfer
	void DrawTile(int x, int y);
	std::string GetTooltip();
	Booster* Copy()
	{
		Booster* logicTile = new Booster();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Repeater : public LogicTile {
public:
	static sf::Texture texture;								// A textures for drawing
	sf::Texture GetTexture() { return this->texture; };
	void DoWireLogic();										// Overrides wire logic transfer
	void DrawTile(int x, int y);
	std::string GetTooltip();
	Repeater* Copy()
	{
		Repeater* logicTile = new Repeater();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Holder : public LogicTile {
public:
	static sf::Texture texture;
	Robot* robotRef;
	void DrawTile(int x, int y) { LogicTile::DrawSprite(x, y, this->texture); }
	void DoWireLogic();
	void DoRobotLogic(Robot* robotRef);
	std::string GetTooltip();
	Holder* Copy()
	{
		Holder* logicTile = new Holder();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Memory : public LogicTile {
public:
	static sf::Texture texture;									// A texture for drawing
	sf::Texture GetTexture() { return this->texture; };
	uint8_t inputSignal = 0;
	uint8_t memIndex = -1;										// The current index accessed for the memory
	uint8_t standByIndex = 0;									// The 'row' index for when transfering control between memory tiles for expandability
	std::array<uint8_t, Gconstants::memoryLimit> memory;
	Memory() {};
	Memory(bool reversed)
	{
		memory = std::array<uint8_t, Gconstants::memoryLimit>();
		for (uint8_t i = 0; i < Gconstants::memoryLimit; i++)
		{
			if (reversed)
				memory[i] = Gconstants::memoryLimit - i - 1;
			else
				memory[i] = i;
		}
	}
	void DoWireLogic();											// Overrides wire logic transfer
	void DrawTile(int x, int y);
	void SetMemIndex(int i);
	std::string GetTooltip();
	Memory* Copy()
	{
		Memory* logicTile = new Memory();
		this->BaseCopy(logicTile);
		logicTile->memory = this->memory;
		return logicTile;
	}
};

class Counter : public LogicTile {
public:
	static sf::Texture texture;
	sf::Texture GetTexture() { return this->texture; };
	uint8_t inputSignal = 0;
	uint8_t maxSignal = Gconstants::maxSignalStrength;
	void DoWireLogic();
	void DrawTile(int x, int y);
	std::string GetTooltip();
	Counter* Copy()
	{
		Counter* logicTile = new Counter();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};