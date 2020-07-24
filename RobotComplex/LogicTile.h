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
#include "MySet.h"
#include "MyMap.h"

// Currently just 4 bytes but may be packed to 3
class LogicTile {
public:
	LogicType logicType;
	uint8_t signal : 5;
	uint8_t color : 3;
	Facing facing;
	uint8_t quantity;
	uint8_t signal2 : 5;
	uint8_t color2 : 3;
	LogicTile() {
		logicType, signal, color, facing, quantity, signal2, color2 = 0;
	}
	LogicTile(uint8_t type)
	{
		logicType = (LogicType)type;
		signal = 0;
		color = 1;
		facing = north;
		quantity = 1;
		signal2 = 0;
		color2 = 1;
	}
	void DoWireLogic(Pos currentPosition);
	uint8_t GetSignal(LogicTile querier);
	void SignalEval(std::array<uint8_t, 4> neighbourSigs);
	bool ShowAlign();
	bool GetConnected(Pos currentPosition, Pos neighbourPosition, LogicTile neigbourElement);
	void DrawLogic(Pos currentPos, SpriteVector* appendTo, void* localMap, float x, float y, float s, uint8_t flags);
	void DrawSignalStrength(SpriteVector* appendTo, float x, float y, float s, int signal, uint8_t flags);
	void DrawQuantity(SpriteVector* appendTo, float x, float y, float s, int quantity, uint8_t flags);
	void DoItemLogic() {};
	void DoRobotLogic() {};
};



/*
class LogicTile : public ParentTile {
public:
	Pos pos;								// 8 bytes
	LogicTypes logictype;					// 1 byte
	uint8_t prevSignal;						// 1 byte
	uint8_t signal;							// 1 byte
	uint8_t quantity = 1;					// quantity for crafting multiple on the same tile
	union
	{
		uint8_t facingAndColor;
		struct {
			Facing facing : 2;
			uint8_t colorClass : 3;
		};
	};
	virtual uint16_t MemorySize();
	static sf::Texture* texture;			// Empty texture
	virtual uint8_t GetColorClass(LogicTile* querier) { return this->colorClass; };
	virtual uint8_t GetSignal(LogicTile* querier) { return this->signal; };
	virtual void SignalEval(std::array<uint8_t, 4> neighbours);
	virtual void DoWireLogic();
	virtual void DoItemLogic() {};
	virtual void DoRobotLogic(Pos robotRef) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual LogicTile* Copy() = 0;
	virtual bool GetConnected(LogicTile* querier);
	virtual bool NeighbourConnects(LogicTile* querier) { return true; };
	virtual bool IsSource() { return false; };
	virtual bool ReceivesSignal(LogicTile* querier) { return true; };
	virtual bool ShowAlign() { return false; };
	virtual uint8_t ShowPowered(LogicTile* querier) { 
		if (this->signal)
		return colorClass;
		else return 0;
	};
	virtual void QueueUpdate();
	virtual void Serialize(std::ofstream*);
	virtual void Deserialize(std::ifstream*, int* blockSize);
	void BaseCopy(LogicTile*);		// Pass in a 'new' allocated memory address and have it populated with the originals properties

	virtual void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color) {};
	void DrawSprite(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture);
	void DrawSprite(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture, sf::IntRect subRect);
	void DrawSpriteFromProperties(SpriteVector* appendTo, float x, float y, float s, sf::Texture* texture, sf::IntRect subRect, int rotation, bool inverse);
	void DrawSignalStrength(SpriteVector* appendTo, float x, float y, float s, int signal, uint8_t flags);
	void DrawQuantity(SpriteVector* appendTo, float x, float y, float s, int quantity, uint8_t flags);
	static LogicTile* Factory(uint16_t classType);
	LogicTile()
	{
		this->pos = Pos{ 0,0 };
		this->facing = north;
		this->prevSignal = -1;
		this->signal = 0;
		this->colorClass = 1;
		this->quantity = 1;
	}
	void dummy(bool) {  };
};
class DirectionalLogicTile : public LogicTile {
public:
	virtual void DoWireLogic();								// Overrides wire logic transfer
	virtual bool GetConnected(LogicTile* querier);
	virtual bool ReceivesSignal(LogicTile* querier);
	virtual void DoItemLogic() {};
	virtual bool NeighbourConnects(LogicTile* querier) {	// not sure if this function is useful
		return true;
		if (querier->pos == this->pos.FacingPosition(this->facing))
			return false;
		return true;
	}
	virtual void QueueUpdate();
	virtual void DoRobotLogic(Pos robotRef) {};
	virtual void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags) {};
	virtual std::string GetTooltip() { return "base class"; };
	virtual bool IsSource() { return true; };
	virtual bool ReceivesSignal(Facing toward) { if (toward == this->facing) return true; else return false; };
	virtual LogicTile* Copy() = 0;
};

class Wire : public LogicTile {
public:
	Wire() { logictype = wire; };
	static sf::Texture* texture;								// A textures for drawing
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
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
	Redirector() { 
		this->logictype = redirector;
	}
	Redirector(const Redirector* other) {
		this->logictype = redirector;
	}
	bool ShowAlign() { return true; };
	void DoRobotLogic(Pos robotRef);						// Redirects robots when stepped onto
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
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
	void DoRobotLogic(Pos robotRef);						// Activates when a robot holding the correct item filter steps onto it
	bool GetConnected(LogicTile* querier)
	{
		if (this->colorClass == querier->colorClass)
			return true;
		return false;
	}
	bool ReceivesSignal(LogicTile* querier)
	{
		if (this->colorClass == querier->colorClass)
			return true;
		return false;
	}
	std::string GetTooltip();
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
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
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	virtual bool IsConnected(Pos pos) { return true; };
	uint8_t ShowPowered(LogicTile* querier) {
		if (this->pos.FacingPosition(this->facing) == querier->pos)
		{
			if (this->signal)
				return this->colorClass;
		}
		else
			if (!this->signal)
				return this->colorClass;
		return 0; 
	}
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
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
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
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	std::string GetTooltip();
	Repeater* Copy()
	{
		Repeater* logicTile = new Repeater();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Gate : public LogicTile {
public:
	Gate() { logictype = gate; };
	static sf::Texture* texture;
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	void DoWireLogic();
	void DoRobotLogic(Pos robotRef);
	std::string GetTooltip();
	Gate* Copy()
	{
		Gate* logicTile = new Gate();
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
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
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
	void DoRobotLogic(Pos robotRef);						// Activates when a robot holding the correct item filter steps onto it
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	std::string GetTooltip() { return "This is a belt"; };
	bool ShowAlign() { return true; };
	Belt* Copy()
	{
		Belt* logicTile = new Belt();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Shover : public LogicTile {
public:
	Shover() { logictype = shover; };
	static sf::Texture* texture;
	void DoItemLogic();										// Activates when an item of the same filter type is placed onto it
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	std::string GetTooltip() { return "Shoves only items to the side"; };
	bool ShowAlign() { return true; };
	Shover* Copy()
	{
		Shover* logicTile = new Shover();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class WireBridge : public LogicTile {
public:
	uint8_t signal2 = 0;
	uint16_t MemorySize() { return (LogicTile::MemorySize() + 1); };
	void Serialize(std::ofstream*);
	void Deserialize(std::ifstream*, int* blockSize);
	WireBridge() { logictype = wirebridge; };
	uint8_t GetColorClass(LogicTile* querier);
	bool GetConnected(LogicTile* querier);
	uint8_t GetSignal(LogicTile* querier);
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	uint8_t ShowPowered(LogicTile* querier);
	std::string GetTooltip() { return "This is a wire bridge"; };
	WireBridge* Copy()
	{
		WireBridge* logicTile = new WireBridge();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Comparer : public DirectionalLogicTile {
public:
	Comparer() { logictype = comparer; };
	static sf::Texture* texture;								// A textures for drawing
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	std::string GetTooltip();
	Comparer* Copy()
	{
		Comparer* logicTile = new Comparer();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class PlusOne : public DirectionalLogicTile {
public:
	PlusOne() { logictype = plusone; };
	static sf::Texture* texture;
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	std::string GetTooltip() { return "Adds one to input signal"; };
	PlusOne* Copy()
	{
		PlusOne* logicTile = new PlusOne();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};

class Toggle : public DirectionalLogicTile {
public:
	uint8_t prevBehindSignal;
	Toggle() { logictype = toggle; };
	static sf::Texture* texture;								// A textures for drawing
	void SignalEval(std::array<uint8_t, 4> neighbours);
	void DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color);
	std::string GetTooltip() { return "This is a toggle"; };
	Toggle* Copy()
	{
		Toggle* logicTile = new Toggle();
		this->BaseCopy(logicTile);
		return logicTile;
	}
};
*/