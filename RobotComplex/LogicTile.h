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
#include "SpriteVector.h"

class LogicTile {
public:
	LogicTypes logictype;					// 1 byte
	Pos pos;								// 8 bytes
	Facing facing;							// 1 byte
	uint8_t prevSignal;						// 1 byte
	uint8_t signal;							// 1 byte
	uint8_t colorClass;						// xxxxxBGR 8 color byte
	static sf::Texture* texture;			// Empty texture
	virtual uint8_t GetSignal(LogicTile* querier) { return this->signal; };
	virtual void SignalEval(std::array<uint8_t, 4> neighbours);
	virtual void DoWireLogic();
	virtual void DoItemLogic() {};
	virtual void DoRobotLogic(Robot* robotRef) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual LogicTile* Copy() = 0;
	virtual bool GetConnected(LogicTile* querier);
	virtual bool NeighbourConnects() { return true; };
	virtual bool IsSource() { return false; };
	virtual bool ReceivesSignal(LogicTile* querier) { return true; };
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
	void BaseCopy(LogicTile*);		// Pass in a 'new' allocated memory address and have it populated with the originals properties

	virtual void DrawTile(SpriteVector* appendTo, float x, float y, float s) {};
	void DrawSprite(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture);
	void DrawSprite(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture, sf::IntRect subRect);
	void DrawSpriteFromProperties(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture, sf::IntRect subRect, int rotation, bool inverse);
	void DrawSignalStrength(SpriteVector* appendTo, float x, float y, float s, int signal);
	static LogicTile* Factory(uint16_t classType);
	LogicTile()
	{
		this->pos = Pos{ 0,0 };
		this->facing = north;
		this->prevSignal = -1;
		this->signal = 0;
		this->colorClass = 1;
	}
};
class DirectionalLogicTile : public LogicTile {
	virtual void DoWireLogic();								// Overrides wire logic transfer
	virtual bool GetConnected(LogicTile* querier);
	virtual bool ReceivesSignal(LogicTile* querier);
	virtual void DoItemLogic() {};
	virtual void DoRobotLogic(Robot* robotRef) {};
	virtual void DrawTile(SpriteVector* appendTo, float x, float y, float s) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual bool IsSource() { return true; };
	virtual bool ReceivesSignal(Facing toward) { if (toward == this->facing) return true; else return false; };
	virtual LogicTile* Copy() = 0;
};

class Wire : public LogicTile {
public:
	Wire() { logictype = wire; };
	static sf::Texture* texture;								// A textures for drawing
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
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
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*);
	Redirector() { 
		this->logictype = redirector;
	}
	Redirector(const Redirector* other) {
		this->logictype = redirector;
	}
	void DoRobotLogic(Robot* robotRef);						// Redirects robots when stepped onto
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
	std::string GetTooltip();
	Redirector* Copy()
	{
		Redirector* logicTile = new Redirector();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class PressurePlate : public LogicTile {
public:
	PressurePlate() { logictype = pressureplate; };
	static sf::Texture* texture;								// A textures for drawing
	void DoWireLogic() {}									// Overwrite base method to not transfer signals
	void DoItemLogic();										// Activates when an item of the same filter type is placed onto it
	void DoRobotLogic(Robot* robotRef);						// Activates when a robot holding the correct item filter steps onto it
	void DrawTile(SpriteVector* appendTo, float x, float y, float s){
		this->facing = north;
		LogicTile::DrawSprite(appendTo, x, y, s, this->texture);
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
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
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
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
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
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
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
	void DrawTile(SpriteVector* appendTo, float x, float y, float s) { LogicTile::DrawSprite(appendTo, x, y, s, this->texture); }
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
	uint8_t inputSignal = 0;
	uint8_t maxSignal = GC::startSignalStrength;
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
	std::string GetTooltip();
	Counter* Copy()
	{
		Counter* logicTile = new Counter();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Belt : public LogicTile {
public:
	Belt() { logictype = belt; };
	static sf::Texture* texture;
	void DoItemLogic();										// Activates when an item of the same filter type is placed onto it
	void DoRobotLogic(Robot* robotRef);						// Activates when a robot holding the correct item filter steps onto it
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
	std::string GetTooltip() { return "This is a belt"; };
	Belt* Copy()
	{
		Belt* logicTile = new Belt();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class WireBridge : public LogicTile {
public:
	uint8_t signal2 = 0;
	WireBridge() { logictype = wirebridge; };
	bool NeighbourConnects() { return false; };
	bool GetConnected(LogicTile* querier);
	uint8_t GetSignal(LogicTile* querier);
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s);
	std::string GetTooltip() { return "This is a wire bridge"; };
	WireBridge* Copy()
	{
		WireBridge* logicTile = new WireBridge();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};