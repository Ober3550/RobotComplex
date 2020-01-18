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
#include <SFML/Graphics.hpp>
#include "Windows.h"
#include "MyMod.h"

LogicTile* LogicTile::Factory(uint16_t classType)
{
	LogicTypes newClass = LogicTypes(classType);
	switch (newClass)
	{
	case wire:
		return new Wire();
	case pressureplate:
		return new PressurePlate();
	case redirector:
		return new Redirector();
	case inverter:
		return new Inverter();
	case booster:
		return new Booster();
	case counter:
		return new Counter();
	case repeater:
		return new Repeater();
	case holder:
		return new Holder();
	}
}

void LogicTile::Serialize(std::ofstream* writer)
{
	writer->write((char*)&this->logictype,	sizeof(LogicTypes));
	writer->write((char*)&this->pos,		sizeof(Pos));
	writer->write((char*)&this->facing,		sizeof(Facing));
	writer->write((char*)&this->prevSignal,	sizeof(uint8_t));
	writer->write((char*)&this->signal,		sizeof(uint8_t));
	writer->write((char*)&this->colorClass,	sizeof(uint8_t));
}

void LogicTile::Deserialize(std::ifstream* reader)
{
	reader->read((char*)&this->pos,			sizeof(Pos));
	reader->read((char*)&this->facing,		sizeof(Facing));
	reader->read((char*)&this->prevSignal,	sizeof(uint8_t));
	reader->read((char*)&this->signal,		sizeof(uint8_t));
	reader->read((char*)&this->colorClass,	sizeof(uint8_t));
}

void Redirector::Serialize(std::ofstream* writer)
{
	LogicTile::Serialize(writer);
	writer->write((char*)&this->itemFilter, sizeof(uint16_t));
	writer->write((char*)&this->dropItem,	sizeof(bool));
}

void Redirector::Deserialize(std::ifstream* reader)
{
	LogicTile::Deserialize(reader);
	reader->read((char*)&this->itemFilter,	sizeof(uint16_t));
	reader->read((char*)&this->dropItem,	sizeof(bool));
}

void LogicTile::BaseCopy(LogicTile* logicTile)
{
	logicTile->logictype = this->logictype;
	logicTile->pos = this->pos;
	logicTile->facing = this->facing;
	logicTile->prevSignal = this->prevSignal;
	logicTile->colorClass = this->colorClass;
}

bool LogicTile::ReceivesSignal(Pos pos)
{
	if (LogicTile* neighbour = world.GetLogicTile(pos.CoordToEncoded()))
	{
		if (this->colorClass == neighbour->colorClass)
			return true;
	}
	return false;
}
void LogicTile::DoWireLogic() {
	std::array<uint8_t, 4> neighbourSignals = { 0,0,0,0 };
	std::array<LogicTile*, 4> neighbourTile = std::array<LogicTile*, 4>();
	this->prevSignal = this->signal;

	for (uint8_t i = 0; i < 4; i++)
	{
		Pos lookingAt = this->pos.FacingPosition(Facing(i));
		if (auto temp = world.logictiles.GetValue(lookingAt.CoordToEncoded()))
		{
			neighbourTile[i] = *temp;
			// If neighbour is 'directional signal provider'
			if (neighbourTile[i]->ReceivesSignal(this->pos))
			{
				if(neighbourTile[i]->IsSource())
					neighbourSignals[i] = neighbourTile[i]->signal + 1;
				else
					neighbourSignals[i] = neighbourTile[i]->signal;
			}
			if(!neighbourTile[i]->IsConnected(this->pos))
			{
				// If neighbour isn't connected to this element don't update it
				neighbourTile[i] = nullptr;
			}
		}
	}

	// This tiles signal will be the max value of its neighbours - 1
	int max_value = *std::max_element(neighbourSignals.begin(), neighbourSignals.end());
	if (this->signal > max_value)
	{
		this->signal = 0;
	}
	else
	{
		signal = max_value > 0 ? max_value - 1 : 0;
	}

	for (uint8_t i = 0; i < 4; i++)
	{
		if (neighbourTile[i])
		{
			// Update neighbours
			if (this->prevSignal != this->signal || (neighbourTile[i]->signal == 0 && this->signal > 0))
			{
				if (neighbourTile[i]->signal != 0 || this->signal != 0)
					world.updateQueueB.insert({ neighbourTile[i]->pos.CoordToEncoded() });
			}
		}
	}
}
bool LogicTile::IsConnected(Pos pos)
{
	if (LogicTile* neighbour = world.GetLogicTile(pos.CoordToEncoded()))
	{
		if (neighbour->colorClass == this->colorClass)
			return true;
		else
			return false;
	}
	else
		return false;
}
bool DirectionalLogicTile::IsConnected(Pos pos) {
	if (LogicTile* neighbour = world.GetLogicTile(pos.CoordToEncoded()))
	{
		Pos infront = this->pos.FacingPosition(this->facing);
		Pos behind = this->pos.BehindPosition(this->facing);
		if (pos == infront || pos == behind)
			return true;
		else
			return false;
	}
	else
		return false;
};
bool DirectionalLogicTile::ReceivesSignal(Pos pos)
{
	if (LogicTile* neighbour = world.GetLogicTile(pos.CoordToEncoded()))
	{
		Pos infront = this->pos.FacingPosition(this->facing);
		if (pos == infront)
			return true;
		else
			return false;
	}
	else
		return false;
}
std::string Wire::GetTooltip()
{
	return program.logicTooltips[0][0];
}

void Redirector::DoRobotLogic(Robot* robotRef)
{
	if (robotRef && signal == 0)
	{
		if (itemFilter == anything && robotRef->itemCarying != nothing || itemFilter == robotRef->itemCarying)
		{
			if (dropItem)
				robotRef->Drop();
			robotRef->SetFacing(this->facing);
		}
	}
}

std::string Redirector::GetTooltip()
{
	if (signal)
		return program.logicTooltips[1][0];
	if (dropItem)
		return program.logicTooltips[1][1];
	if(itemFilter >= 0)
		return program.logicTooltips[1][2] + program.itemTooltips[itemFilter];
	return "error";
}

void PressurePlate::DoItemLogic()
{
	if (ItemTile * thisTile = world.GetItemTile(this->pos))
	{
		this->prevSignal = this->signal;
		this->signal = GC::maxSignalStrength;
	}
	else
	{
		this->prevSignal = this->signal;
		this->signal = 0;
	}
	if (this->prevSignal != this->signal)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			Pos facingTile = this->pos.FacingPosition(Facing(i));
			if (LogicTile* neighbour = world.GetLogicTile(facingTile.CoordToEncoded()))
			{
				neighbour->DoWireLogic();
			}
		}
	}
}
void PressurePlate::DoRobotLogic(Robot* robotRef)
{
	if (robotRef)
	{
		this->prevSignal = this->signal;
		this->signal = GC::maxSignalStrength;
	}
	else
	{
		this->prevSignal = this->signal;
		this->signal = 0;
	}
	if (this->prevSignal != this->signal)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			Pos facingTile = this->pos.FacingPosition(Facing(i));
			if (LogicTile* neighbour = world.GetLogicTile(facingTile.CoordToEncoded()))
			{
				neighbour->DoWireLogic();
			}
		}
	}
}

std::string PressurePlate::GetTooltip()
{
	return program.logicTooltips[2][0];
}

void Inverter::DoWireLogic()
{
	Pos Back = this->pos.BehindPosition(this->facing);
	Pos Left = this->pos.FacingPosition(Facing(int(this->facing) - 1));
	Pos Right = this->pos.FacingPosition(Facing(int(this->facing) + 1));
	int a = 0;
	int b1 = 0;
	int b2 = 0;
	if (LogicTile* tileBack = world.GetLogicTile(Back.CoordToEncoded()))
	{
		if (tileBack->ReceivesSignal(this->pos))
			a = tileBack->signal;
	}
	if (LogicTile* tileLeft = world.GetLogicTile(Left.CoordToEncoded()))
	{
		if (tileLeft->ReceivesSignal(this->pos))
			b1 = tileLeft->signal;
	}
	if (LogicTile* tileRight = world.GetLogicTile(Right.CoordToEncoded()))
	{
		if (tileRight->ReceivesSignal(this->pos))
			b2 = tileRight->signal;
	}
	this->prevSignal = this->signal;
	if (b1 > b2)
	{
		if (GC::maxSignalStrength - a + b1 > 0)
			this->signal = MyMod(GC::maxSignalStrength - a + b1,32);
		else
			this->signal = 0;
	}
	else
	{
		if (GC::maxSignalStrength - a + b2 > 0)
			this->signal = MyMod(GC::maxSignalStrength - a + b2, 32);
		else
			this->signal = 0;
	}
	if (this->prevSignal != this->signal)
	{
		Pos tileFore = this->pos.FacingPosition(this->facing);
		if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
		{
			world.updateQueueC.insert(tileFore.CoordToEncoded());
		}
	}
}

std::string Inverter::GetTooltip()
{
	return program.logicTooltips[3][0];
}

void Booster::DoWireLogic()
{
	Pos tileBack = this->pos.BehindPosition(this->facing);
	if (LogicTile* tileBehind = world.GetLogicTile(tileBack.CoordToEncoded()))
	{
		this->prevSignal = this->signal;
		this->signal = tileBehind->signal > 0 ? GC::maxSignalStrength : 0;
		if (this->prevSignal != this->signal)
		{
			Pos tileFore = this->pos.FacingPosition(this->facing);
			if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
			{
				world.updateQueueC.insert(tileFore.CoordToEncoded());
			}
		}
	}
}

std::string Booster::GetTooltip()
{
	return program.logicTooltips[4][0];
}

void Repeater::DoWireLogic()
{
	Pos tileBack = this->pos.BehindPosition(this->facing);
	if (auto temp = world.logictiles.GetValue(tileBack.CoordToEncoded()))
	{
		LogicTile* tileBehind = *temp;
		this->prevSignal = this->signal;
		this->signal = tileBehind->signal;
		if (this->prevSignal != this->signal)
		{
			Pos tileFore = this->pos.FacingPosition(this->facing);
			if (LogicTile * *neighbourLogic = world.logictiles.GetValue(tileFore.CoordToEncoded()))
			{
				world.updateQueueC.insert(tileFore.CoordToEncoded());
			}
		}
	}
}

std::string Repeater::GetTooltip()
{
	return program.logicTooltips[5][0];
}

void Holder::DoRobotLogic(Robot* robotRef)
{
	this->robotRef = robotRef;
	if (robotRef)
	{
		if(!signal)
		robotRef->stopped = true;
	}
}

void Holder::DoWireLogic()
{
	LogicTile::DoWireLogic();
	if (robotRef)
	{
		if (signal)
		{
			robotRef->stopped = false;
		}
	}
}

std::string Holder::GetTooltip()
{
	return program.logicTooltips[7][0];
}

void Counter::DoWireLogic()
{
	Pos tileBack = this->pos.BehindPosition(this->facing);
	if (LogicTile * tileBehind = world.GetLogicTile(tileBack.CoordToEncoded()))
	{
		this->prevSignal = this->inputSignal;
		this->inputSignal = tileBehind->signal;
		if (this->prevSignal != this->inputSignal && tileBehind->prevSignal == 0 && tileBehind->signal > 0)
		{
			++this->signal;
			if (this->signal > 15)
			{
				this->signal = 0;
			}
		}
	}
	Pos tileFront = this->pos.FacingPosition(this->facing);
	if (LogicTile * tileInfront = world.GetLogicTile(tileFront.CoordToEncoded()))
	{
		tileInfront->DoWireLogic();
	}
}

std::string Counter::GetTooltip()
{
	return program.logicTooltips[8][0];
}

