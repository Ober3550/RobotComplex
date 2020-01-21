#pragma once
#include "Robot.h"
#include "LogicTypes.h"
#include <cstdint>
#include <SFML/Graphics.hpp>
#include <array>
#include "ReservedItems.h"
#include "Constants.h"
#include <iostream>
#include <bitset>

class LogicTile {
public:
	LogicTypes logictype;					// 1 byte
	Pos pos;								// 8 bytes
	Facing facing;					// 1 byte
	uint8_t prevSignal = -1;				// 1 byte
	uint8_t signal = 0;						// 1 byte
	std::bitset<8> connected;
	//uint8_t colorClass = 1;					// xxxxxBGR 8 color byte
	static sf::Texture* texture;				// Empty texture
	virtual sf::Texture* GetTexture() { return this->texture; };
	virtual void DoWireLogic();
	virtual void DoItemLogic() {};
	virtual void DoRobotLogic(Robot* robotRef) {};
	virtual void DrawTile(int x, int y, float s) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual LogicTile* Copy() = 0;
	virtual bool GetConnected(Facing toward);
	virtual void SetConnected(Facing toward, bool);
	virtual bool IsSource() { return false; };
	virtual bool ReceivesSignal(Facing toward) { return true; };
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
	void BaseCopy(LogicTile*);		// Pass in a 'new' allocated memory address and have it populated with the originals properties
	void DrawSprite(int x, int y, float s, sf::Texture* texture);
	void DrawSprite(int x, int y, float s, sf::Texture* texture, sf::IntRect subRect);
	void DrawSpriteFromProperties(int x, int y, float s, sf::Texture* texture, sf::IntRect subRect, int rotation, bool inverse);
	void DrawSignalStrength(int x, int y, float s, int signal);
	static LogicTile* Factory(uint16_t classType);
	LogicTile()
	{
		this->pos = Pos{ 0,0 };
		this->facing = north;
		this->prevSignal = -1;
		this->signal = 0;
		for (size_t each = 0; each < this->connected.size(); each++)
			this->connected.set(each, true);
	}
};
class DirectionalLogicTile : public LogicTile {
	virtual void DoWireLogic();								// Overrides wire logic transfer
	virtual void SignalEval(std::array<uint8_t, 4> neighbours) { };
	virtual void DoItemLogic() {};
	virtual void DoRobotLogic(Robot* robotRef) {};
	virtual void DrawTile(int x, int y, float s) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual bool IsSource() { return true; };
	virtual bool ReceivesSignal(Facing toward) { if (toward == this->facing) return true; else return false; };
	virtual LogicTile* Copy() = 0;
};

class Wire : public LogicTile {
public:
	Wire() { logictype = wire; };
	static sf::Texture* texture;								// A textures for drawing
	sf::Texture* GetTexture() { return this->texture; };
	void DrawTile(int x, int y, float s);
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
	static sf::Texture* texture;					// A texture for drawing
	sf::Texture* GetTexture() { return this->texture; };
	uint16_t itemFilter;						// 2 bytes
	bool dropItem;								// 1 bit
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
	Redirector() { 
		this->logictype = redirector;
		this->itemFilter = 0; 
		this->dropItem = false; 
	}
	Redirector(uint16_t itemFilter, bool dropItem)
	{
		this->logictype = redirector;
		this->itemFilter = itemFilter;
		this->dropItem = dropItem;
	};
	Redirector(const Redirector* other) {
		this->logictype = redirector;
		this->itemFilter = other->itemFilter;
		this->dropItem = other->dropItem;
	}
	void DoRobotLogic(Robot* robotRef);						// Redirects robots when stepped onto
	void DrawTile(int x, int y, float s);
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
	PressurePlate() { logictype = pressureplate; };
	static sf::Texture* texture;								// A textures for drawing
	sf::Texture* GetTexture() { return this->texture; };
	void DoWireLogic() {}									// Overwrite base method to not transfer signals
	void DoItemLogic();										// Activates when an item of the same filter type is placed onto it
	void DoRobotLogic(Robot* robotRef);						// Activates when a robot holding the correct item filter steps onto it
	void DrawTile(int x, int y, float s){
		this->facing = north;
		LogicTile::DrawSprite(x, y, s, this->texture);
	}
	std::string GetTooltip();
	PressurePlate* Copy()
	{
		PressurePlate* logicTile = new PressurePlate();
		this->BaseCopy(logicTile);
		return logicTile;
	}
	virtual bool IsSource() { return true; };
};

class Inverter : public DirectionalLogicTile {
public:
	Inverter() { logictype = inverter; };
	static sf::Texture* texture;								// A textures for drawing
	sf::Texture* GetTexture() { return this->texture; };
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(int x, int y, float s);
	virtual bool IsConnected(Pos pos) { return true; };
	std::string GetTooltip();
	Inverter* Copy()
	{
		Inverter* logicTile = new Inverter();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Booster : public DirectionalLogicTile {
public:
	Booster() { logictype = booster; };
	static sf::Texture* texture;								// A textures for drawing
	sf::Texture* GetTexture() { return this->texture; };
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(int x, int y, float s);
	std::string GetTooltip();
	Booster* Copy()
	{
		Booster* logicTile = new Booster();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Repeater : public DirectionalLogicTile {
public:
	Repeater() { logictype = repeater; };
	static sf::Texture* texture;								// A textures for drawing
	sf::Texture* GetTexture() { return this->texture; };
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(int x, int y, float s);
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
	Holder() { logictype = holder; };
	static sf::Texture* texture;
	Robot* robotRef;
	void DrawTile(int x, int y, float s) { LogicTile::DrawSprite(x, y, s, this->texture); }
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

class Counter : public DirectionalLogicTile {
public:
	Counter() { logictype = counter; };
	static sf::Texture* texture;
	sf::Texture* GetTexture() { return this->texture; };
	uint8_t inputSignal = 0;
	uint8_t maxSignal = GC::maxSignalStrength;
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(int x, int y, float s);
	std::string GetTooltip();
	Counter* Copy()
	{
		Counter* logicTile = new Counter();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};