#pragma once
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
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Windows.h"
#include "MyMod.h"
#include "Textures.h"

void LogicTile::SignalEval(std::array<uint8_t, 4> neighbourSigs)
{
	if (this->logicType & UNI_DIR || this->logicType == wirebridge)
	{
		switch (this->logicType)
		{
		case toggle:
		{
			if (neighbourSigs[2] && this->signal2 == 0)
				this->signal = neighbourSigs[2];
			this->signal2 = neighbourSigs[2];
			if (neighbourSigs[1] || neighbourSigs[3])
				this->signal = 0;
		}break;
		case plusone:
		{
			if (neighbourSigs[1] == 0 && neighbourSigs[3] == 0)
				this->signal = MyMod(neighbourSigs[2] + 1, GC::maxSignalStrength);
			else
				this->signal = neighbourSigs[2];
		}break;
		case comparer:
		{
			uint8_t b = std::max(neighbourSigs[1], neighbourSigs[3]);
			if (neighbourSigs[2] == b)
				this->signal = MyMod(neighbourSigs[2] + GC::startSignalStrength, GC::maxSignalStrength);
			else
				this->signal = 0;
		}break;
		case booster:
		{
			uint8_t signal;
			uint8_t b = std::max(neighbourSigs[1], neighbourSigs[3]);
			if (neighbourSigs[2] > b)
				signal = neighbourSigs[2] + GC::startSignalStrength;
			else
				signal = 0;
			if (signal > GC::maxSignalStrength - 1)
				signal = GC::maxSignalStrength - 1;
			this->signal = signal;
		}break;
		case inverter:
		{
			uint8_t b = std::max(neighbourSigs[1], neighbourSigs[3]);
			this->signal = MyMod(GC::startSignalStrength - neighbourSigs[2] + b, GC::maxSignalStrength);
		}break;
		case wirebridge:
		{
			uint8_t maxA = std::max(neighbourSigs[0], neighbourSigs[2]);
			if (this->signal > maxA)
				this->signal = 0;
			else if (maxA > 0)
				this->signal = maxA - 1;

			uint8_t maxB = std::max(neighbourSigs[1], neighbourSigs[3]);
			if (this->signal2 > maxB)
				this->signal2 = 0;
			else if (maxB > 0)
				this->signal2 = maxB - 1;
		}break;
		}
	}
	else
	{
		uint8_t max = 0;
		// Perform max operation directly on signal value unless this element is wire bridge
		for (uint8_t i = 0; i < 4; i++)
		{
			if (neighbourSigs[i] > max)
				max = neighbourSigs[i];
		}
		if (this->signal > max)
			this->signal = 0;
		else if (max > 0)
			this->signal = max - 1;
	}
}

uint8_t LogicTile::GetSignal(LogicTile querier)
{
	if (this->logicType == wirebridge)
	{
		if (this->color == querier.color)
			return this->signal;
		if (this->color2 == querier.color)
			return this->signal2;
	}
	else
		return this->signal;
}

bool LogicTile::GetConnected(Pos currentPosition, Pos neighbourPosition, LogicTile neighbourElement)
{
	if (neighbourElement.logicType & UNI_DIR)
	{
		if (currentPosition == neighbourPosition.FacingPosition(neighbourElement.facing) || neighbourElement.logicType == pressureplate)
			return true;
		return false;
	}
	else
	{
		if (this->logicType & UNI_DIR)
		{
			if (currentPosition == neighbourPosition.FacingPosition(this->facing))
				return true;
			if (this->color == neighbourElement.color)
				return true;
			return false;
		}
		else
		{
			if (neighbourElement.logicType == wirebridge || this->logicType == wirebridge)
			{
				return true;
			}
			if (this->color == neighbourElement.color)
				return true;
			return false;
		}
	}
}

void LogicTile::DoWireLogic(Pos currentPosition)
{
	std::array<LogicTile, 4>	neighbours = {};
	std::array<uint8_t, 4>		neighbourSigs = {};
	uint8_t prevSignal = this->signal;
	uint8_t prevSignal2 = this->signal2;
	bool zeroNeighbour = false;

	for (uint8_t i = 0; i < 4; i++)
	{
		Pos neighbourPosition = currentPosition.FacingPosition(Pos::RelativeFacing(this->facing, i));
		auto neighbour = world.logicTiles.find(neighbourPosition.CoordToEncoded());
		if (neighbour != world.logicTiles.end())
		{
			neighbours[i] = neighbour->second;
			if (this->GetConnected(currentPosition, neighbourPosition, neighbours[i]))
			{
				if (neighbours[i].logicType & UNI_DIR)
					neighbourSigs[i] = neighbours[i].GetSignal(*this) + 1;
				else
				{
					neighbourSigs[i] = neighbours[i].GetSignal(*this);
					if (neighbourSigs[i] == 0)
						zeroNeighbour = true;
				}
					
			}
		}
	}
	this->SignalEval(neighbourSigs);

	if (this->signal != prevSignal || this->signal2 != prevSignal2 || (this->signal == prevSignal && this->signal > 0 && zeroNeighbour))
	{
		// If current element is a directional element only update infront
		if (this->logicType & UNI_DIR
			&& !(this->logicType == pressureplate))
		{
			if (neighbours[0].logicType & UNI_DIR)
				world.updateNext.emplace(currentPosition.FacingPosition(this->facing), 1);
			else
				world.updateProp.emplace(currentPosition.FacingPosition(this->facing));
		}
		else
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				// If current elements color matches neighbours color
				// or if neighbour is unidirectional element
				// or if current element is wirebridge
				if (this->color == neighbours[i].color
					|| (neighbours[i].logicType & UNI_DIR)
					|| this->logicType == wirebridge
					|| neighbours[i].logicType == wirebridge)
				{
					if (neighbours[i].logicType & UNI_DIR)
						world.updateNext.emplace(currentPosition.FacingPosition(Pos::RelativeFacing(this->facing, i)), 1);
					else
						world.updateProp.emplace(currentPosition.FacingPosition(Pos::RelativeFacing(this->facing, i)));
				}
			}
		}
	}
}

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
	
	sprite_rotation = ((float)this->facing) * (float)90.f;
	sprite.setRotation(sprite_rotation);
	sprite.setColor(sf::Color(Red, Green, Blue, Alpha));
	sprite.setTexture(*logicTextures[this->logicType]);
	sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
	if (this->logicType == wire)
	{
		sprite.setTextureRect(sf::IntRect(14, 14, 4, 4));
		sprite.setPosition(x + float(GC::tileSize) - 2, y + float(GC::tileSize) - 2);
	}
		
	if (this->logicType != wirebridge)
		appendTo->emplace_back(sprite);

	// Signal value
	this->DrawSignalStrength(appendTo, x - (5 * (this->logicType == wirebridge)), y, s, this->signal, flags);
	// Signal value
	if (this->logicType == wirebridge)
		this->DrawSignalStrength(appendTo, x + 5, y, s, this->signal2, flags);

	if(this->quantity > 1 && this->logicType != wirebridge || this->quantity > 2)
		DrawQuantity(appendTo, x, y, s, this->quantity, flags);
}
bool LogicTile::ShowAlign()
{
	if (this->logicType == redirector
		|| this->logicType == belt)
		return true;
	return false;
}

//LogicTile* LogicTile::Factory(uint16_t classType)
//{
//	/*
//	wire = 1,
//	pressureplate = 2,
//	redirector = 3,
//	inverter = 4,
//	booster = 5,
//	counter = 6,
//	repeater = 7,
//	gate = 8,
//	belt = 9,
//	wirebridge = 10,
//	comparer = 11
//	*/
//	LogicTypes newClass = LogicTypes(classType);
//	switch (newClass)
//	{
//	case wire:
//		return new Wire();
//	case pressureplate:
//		return new PressurePlate();
//	case redirector:
//		return new Redirector();
//	case inverter:
//		return new Inverter();
//	case booster:
//		return new Booster();
//	case counter:
//		return new Counter();
//	case repeater:
//		return new Repeater();
//	case gate:
//		return new Gate();
//	case belt:
//		return new Belt();
//	case wirebridge:
//		return new WireBridge();
//	case comparer:
//		return new Comparer();
//	case plusone:
//		return new PlusOne();
//	case shover:
//		return new Shover();
//	case toggle:
//		return new Toggle();
//	}
//	return nullptr;
//}
//
//void LogicTile::Serialize(std::ofstream* writer)
//{
//	writer->write((char*)&this->logictype,	sizeof(LogicTypes));
//	writer->write((char*)&this->pos,		sizeof(Pos));
//	writer->write((char*)&this->facingAndColor,	1);
//	writer->write((char*)&this->prevSignal,	1);
//	writer->write((char*)&this->signal,		1);
//	writer->write((char*)&this->quantity,	1);
//}
//
//uint16_t LogicTile::MemorySize() {
//	return 12;
//};
//
//void LogicTile::Deserialize(std::ifstream* reader, int* blockSize)
//{
//	if (int(*blockSize - sizeof(Pos)) < 0) return;
//	else *blockSize -= sizeof(Pos);
//	reader->read((char*)&this->pos,			sizeof(Pos));
//
//	
//	if (int(*blockSize - 1) < 0) return;
//	else *blockSize -= 1;
//	reader->read((char*)&this->facingAndColor, 1);
//	
//	/*
//	if (int(*blockSize - sizeof(Facing)) < 0) return;
//	else *blockSize -= sizeof(Facing);
//	uint8_t* tempByte = new uint8_t();
//	reader->read((char*)tempByte, sizeof(uint8_t));
//	this->facing = Facing(*tempByte);
//	*/
//
//	if (int(*blockSize - sizeof(uint8_t)) < 0) return;
//	else *blockSize -= sizeof(uint8_t);
//	reader->read((char*)&this->prevSignal,	sizeof(uint8_t));
//	if (int(*blockSize - sizeof(uint8_t)) < 0) return;
//	else *blockSize -= sizeof(uint8_t);
//	reader->read((char*)&this->signal,		sizeof(uint8_t));
//
//	/*
//	if (int(*blockSize - sizeof(uint8_t)) < 0) return;
//	else *blockSize -= sizeof(uint8_t);
//	tempByte = new uint8_t();
//	reader->read((char*)tempByte,	sizeof(uint8_t));
//	this->colorClass = *tempByte;
//	*/
//
//	if (int(*blockSize - sizeof(uint8_t)) < 0) return;
//	else *blockSize -= sizeof(uint8_t);
//	reader->read((char*)&this->quantity,	sizeof(uint8_t));
//}
//
//void WireBridge::Serialize(std::ofstream* writer)
//{
//	LogicTile::Serialize(writer);
//	writer->write((char*)&this->signal2, sizeof(uint8_t));
//}
//
//void WireBridge::Deserialize(std::ifstream* reader, int* blockSize)
//{
//	LogicTile::Deserialize(reader, blockSize);
//	if (int(*blockSize - sizeof(uint8_t)) < 0) return;
//	else *blockSize -= sizeof(uint8_t);
//	reader->read((char*)&this->signal2, sizeof(uint8_t));
//}
//
//void LogicTile::BaseCopy(LogicTile* logicTile)
//{
//	logicTile->logictype = this->logictype;
//	logicTile->pos = this->pos;
//	logicTile->facing = this->facing;
//	logicTile->prevSignal = this->prevSignal;
//	logicTile->signal = this->signal;
//	logicTile->colorClass = this->colorClass;
//}
//bool LogicTile::GetConnected(LogicTile* querier)
//{
//	if (querier)
//	{
//		if (querier->IsSource())
//			return true;
//		if (this->IsSource())
//			return true;
//		if (querier->GetColorClass(this) == this->GetColorClass(querier))
//			return true;
//	}
//	return false;
//}
//bool DirectionalLogicTile::GetConnected(LogicTile* querier)
//{
//	if (querier)
//	{
//		if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
//			return true;
//		if (querier->IsSource() )
//		{
//			if (querier->ReceivesSignal(this))
//			{
//				return true;
//			}
//			return false;
//		}
//		if (querier->GetColorClass(this) == this->GetColorClass(querier))
//			return true;
//	}
//	return false;
//}
//bool DirectionalLogicTile::ReceivesSignal(LogicTile* querier)
//{
//	if (this->pos.FacingPosition(this->facing) == querier->pos)
//		return true;
//	else
//		return false;
//}
//void LogicTile::DoWireLogic() {
//	program.scaledPersistentBoxes.clear();
//	if (program.showDebugInfo)
//	{
//		Sleep(100);
//		program.DrawSelectedBox(&program.scaledPersistentBoxes, pos);
//	}
//	std::array<uint8_t, 4> neighbourSignals = { 0,0,0,0 };
//	std::array<LogicTile*, 4> neighbourTile = std::array<LogicTile*, 4>();
//	this->prevSignal = this->signal;
//
//	// output, b1, a, b2
//	for (int i = 0; i < 4; i++)
//	{
//		if (neighbourTile[i] = world.GetLogicTile(this->pos.FacingPosition(Pos::RelativeFacing(this->facing, i))))
//		{
//			if (neighbourTile[i]->GetConnected(this) || this->GetConnected(neighbourTile[i]))
//			{
//				if (neighbourTile[i]->ReceivesSignal(this))
//					if (neighbourTile[i]->IsSource())
//						neighbourSignals[i] = neighbourTile[i]->GetSignal(this) + 1;
//					else
//						neighbourSignals[i] = neighbourTile[i]->GetSignal(this);
//			}
//		}
//	}
//
//	SignalEval(neighbourSignals);
//
//	for (uint8_t i = 0; i < 4; i++)
//	{
//		if (neighbourTile[i])
//		{
//			if (neighbourTile[i]->NeighbourConnects(this))
//			{
//				// Update neighbours
//				if (this->prevSignal != this->signal || (neighbourTile[i]->GetSignal(this) == 0 && this->GetSignal(neighbourTile[i]) > 0))
//				{
//					if (this->prevSignal > 0 && this->signal == 0)
//					{
//						world.updateQueueD.insert(this->pos.CoordToEncoded());
//					}
//					//if (neighbourTile[i]->GetSignal(this) != 0 || this->GetSignal(neighbourTile[i]) != 0)
//					neighbourTile[i]->QueueUpdate();
//				}
//			}
//		}
//	}
//}
//
//void LogicTile::QueueUpdate()
//{
//	world.updateQueueB.insert({ this->pos.CoordToEncoded() });
//}
//
//// This tiles signal will be the max value of its neighbours - 1
//void LogicTile::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	int max_value = std::max({ neighbours[0],neighbours[1],neighbours[2],neighbours[3] });
//	if (this->signal > max_value)
//	{
//		this->signal = 0;
//	}
//	else
//	{
//		signal = max_value > 0 ? max_value - 1 : 0;
//	}
//}
//
//void DirectionalLogicTile::DoWireLogic()
//{
//	//Sleep(100);
//	// output, b1, a, b2
//	std::array<uint8_t, 4> neighbourSignals = { 0,0,0,0 };
//	for (int i = 0; i < 4; i++)
//	{
//		if (LogicTile* tile = world.GetLogicTile(this->pos.FacingPosition(Pos::RelativeFacing(this->facing, i))))
//		{
//			if (tile->GetConnected(this) && this->GetConnected(tile))
//				if(tile->ReceivesSignal(this))
//					neighbourSignals[i] = tile->GetSignal(this);
//		}
//	}
//	this->SignalEval(neighbourSignals);
//	// Update element infront
//	if (this->prevSignal != this->signal)
//	{
//		Pos tileFore = this->pos.FacingPosition(this->facing);
//		if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
//		{
//			tileInfront->QueueUpdate();
//		}
//	}
//}
//void DirectionalLogicTile::QueueUpdate()
//{
//	world.updateQueueC.insert({ this->pos.CoordToEncoded(),1 });
//}
//std::string Wire::GetTooltip()
//{
//	return program.logicTooltips[0][0];
//}
//void Redirector::DoRobotLogic(Pos robotRef)
//{
//	if (!signal)
//	{
//		if (Robot* robot = world.GetRobot(robotRef))
//		{
//			robot->SetFacing(this->facing);
//		}
//	}
//}
//std::string Redirector::GetTooltip()
//{
//	return program.logicTooltips[1][0];
//}
//
//void PressurePlate::DoItemLogic()
//{
//	if (ItemTile * thisTile = world.GetItemTile(this->pos))
//	{
//		this->prevSignal = this->signal;
//		this->signal = MyMod(thisTile->quantity, GC::maxSignalStrength);
//	}
//	else
//	{
//		this->prevSignal = this->signal;
//		this->signal = 0;
//	}
//	if (this->prevSignal != this->signal)
//	{
//		for (uint8_t i = 0; i < 4; i++)
//		{
//			Pos facingTile = this->pos.FacingPosition(Facing(i));
//			if (LogicTile* neighbour = world.GetLogicTile(facingTile.CoordToEncoded()))
//			{
//				neighbour->DoWireLogic();
//			}
//		}
//	}
//}
//void PressurePlate::DoRobotLogic(Pos robotRef)
//{
//	if (Robot* robot = world.GetRobot(robotRef))
//	{
//		this->prevSignal = this->signal;
//		this->signal = GC::startSignalStrength;
//	}
//	else
//	{
//		this->prevSignal = this->signal;
//		this->signal = 0;
//	}
//	if (this->prevSignal != this->signal)
//	{
//		for (uint8_t i = 0; i < 4; i++)
//		{
//			Pos facingTile = this->pos.FacingPosition(Facing(i));
//			if (LogicTile* neighbour = world.GetLogicTile(facingTile.CoordToEncoded()))
//			{
//				neighbour->DoWireLogic();
//			}
//		}
//	}
//}
//
//std::string PressurePlate::GetTooltip()
//{
//	return program.logicTooltips[2][0];
//}
//
//void Inverter::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	int b = std::max(neighbours[1], neighbours[3]);
//	this->signal = MyMod(GC::startSignalStrength - neighbours[2] + b, GC::maxSignalStrength);
//}
//
//std::string Inverter::GetTooltip()
//{
//	return program.logicTooltips[3][0];
//}
//
//void Booster::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	int b = std::max(neighbours[1], neighbours[3]);
//	if (neighbours[2] > b)
//		this->signal = neighbours[2] + GC::startSignalStrength;
//	else
//		this->signal = 0;
//	if (this->signal > GC::maxSignalStrength)
//		this->signal = GC::maxSignalStrength;
//}
//
//void Comparer::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	int b = std::max(neighbours[1], neighbours[3]);
//	if (neighbours[2] == b)
//		this->signal = MyMod(neighbours[2] + GC::startSignalStrength, GC::maxSignalStrength);
//	else
//		this->signal = 0;
//}
//
//std::string Comparer::GetTooltip()
//{
//	return program.logicTooltips[9][0];
//}
//
//std::string Booster::GetTooltip()
//{
//	return program.logicTooltips[4][0];
//}
//
//void Repeater::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	//int b = std::max(neighbours[1], neighbours[3]);
//	this->signal = neighbours[2];
//}
//
//std::string Repeater::GetTooltip()
//{
//	return program.logicTooltips[5][0];
//}
//
//void Gate::DoRobotLogic(Pos robotRef)
//{
//	if (!signal)
//	{
//		if (Robot* robot = world.GetRobot(robotRef.CoordToEncoded()))
//		{
//			robot->stopped = true;
//		}
//	}
//}
//
//void Gate::DoWireLogic()
//{
//	LogicTile::DoWireLogic();
//	if (signal)
//	{
//		if (Robot* robot = world.GetRobot(this->pos.CoordToEncoded()))
//		{
//			robot->stopped = false;
//		}
//	}
//}
//
//std::string Gate::GetTooltip()
//{
//	return program.logicTooltips[7][0];
//}
//
//void Counter::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	this->prevSignal = this->inputSignal;
//	this->inputSignal = neighbours[2];
//	if (LogicTile* temp = world.GetLogicTile(this->pos.BehindFacing(this->facing)))
//	{
//		// Rising edge of a logical element
//		if (this->prevSignal != this->inputSignal && temp->prevSignal == 0 && neighbours[2] > 0)
//		{
//			++this->signal;
//			if (this->signal > 15)
//			{
//				this->signal = 0;
//			}
//		}
//	}
//}
//
//std::string Counter::GetTooltip()
//{
//	return program.logicTooltips[8][0];
//}
//
//void Belt::DoRobotLogic(Pos robotRef)
//{
//	if (!signal)
//	{
//		if (Robot* robot = world.GetRobot(robotRef))
//		{
//			robot->SetFacing(this->facing);
//		}
//	}
//}
//
//void Belt::DoItemLogic()
//{
//	if (signal == 0)
//	{
//		if (ItemTile* item = world.GetItemTile(this->pos))
//		{
//			std::vector<Pos> pushStack = { this->pos };
//			world.PushItems(&pushStack, this->facing, GC::robotStrength);
//			if (!pushStack.empty())
//			{
//				for (uint16_t i = 1; i < pushStack.size(); i++)
//				{
//					world.nextItemPos.insert({ pushStack[i - 1].CoordToEncoded(),this->facing });
//					world.updateQueueD.insert(pushStack[i - 1].FacingPosition(this->facing).CoordToEncoded());
//				}
//			}
//		}
//	}
//}
//
//void Shover::DoItemLogic()
//{
//	if (ItemTile* item = world.GetItemTile(this->pos))
//	{
//		std::vector<Pos> pushStack = { this->pos };
//		world.PushItems(&pushStack, this->facing, this->signal);
//		if (!pushStack.empty())
//		{
//			for (uint16_t i = 1; i < pushStack.size(); i++)
//			{
//				world.nextItemPos.insert({ pushStack[i - 1].CoordToEncoded(),this->facing });
//				world.updateQueueD.insert(pushStack[i - 1].FacingPosition(this->facing).CoordToEncoded());
//			}
//		}
//	}
//}
//
//uint8_t WireBridge::ShowPowered(LogicTile* querier)
//{
//	if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
//	{
//		if (this->signal)
//			return GC::colorClassA;
//	}
//	else
//	{
//		if(this->signal2)
//			return GC::colorClassB;
//	}
//	return 0;
//}
//
//uint8_t WireBridge::GetColorClass(LogicTile* querier)
//{
//	if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
//	{
//		return GC::colorClassA;
//	}
//	else
//	{
//		return GC::colorClassB;
//	}
//	return 0;
//}
//
//uint8_t WireBridge::GetSignal(LogicTile* querier)
//{
//	if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
//	{
//		return this->signal;
//	}
//	else
//	{
//		return this->signal2;
//	}
//}
//
//bool WireBridge::GetConnected(LogicTile* querier)
//{
//	if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
//	{
//		if(querier->colorClass == GC::colorClassA || querier->IsSource())
//		return true;
//	}
//	else
//	{
//		if (querier->colorClass == GC::colorClassB || querier->IsSource())
//		return true;
//	}
//	return false;
//}
//
//void WireBridge::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	uint8_t temp = signal2;
//	uint8_t maxA = std::max({ neighbours[0],neighbours[2] });
//	uint8_t maxB = std::max({ neighbours[1],neighbours[3] });
//	if (this->signal > maxA)
//		this->signal = 0;
//	else
//		signal = maxA > 0 ? maxA - 1 : 0;
//	if (this->signal2 > maxB)
//		this->signal2 = 0;
//	else
//		signal2 = maxB > 0 ? maxB - 1 : 0;
//
//	if (temp != signal2)
//		this->prevSignal = this->signal - 1;
//}
//
//void PlusOne::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	//int b = std::max(neighbours[1], neighbours[3]);
//	if (neighbours[1] == 0 && neighbours[3] == 0)
//		this->signal = MyMod(neighbours[2] + 1, GC::maxSignalStrength);
//	else
//		this->signal = neighbours[2];
//}
//
//void Toggle::SignalEval(std::array<uint8_t, 4> neighbours)
//{
//	if (LogicTile* neighbour = world.GetLogicTile(this->pos.BehindPosition(this->facing)))
//	{
//
//		if (neighbours[2] && prevBehindSignal == 0)
//			this->signal = MyMod(this->signal + GC::startSignalStrength, GC::maxSignalStrength);
//	}
//	prevBehindSignal = neighbours[2];
//}
