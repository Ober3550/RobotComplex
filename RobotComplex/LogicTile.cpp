#pragma once
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
#include "Windows.h"

void LogicTile::BaseCopy(LogicTile* logicTile)
{
	logicTile->pos = this->pos;
	logicTile->facing = this->facing;
	logicTile->prevSignal = this->prevSignal;
	logicTile->colorClass = this->colorClass;
}

void LogicTile::DoWireLogic() {
	Sleep(100);
	program.updateCounter++;
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
			if (typeid(*neighbourTile[i]) == typeid(Inverter) || typeid(*neighbourTile[i]) == typeid(Booster) || typeid(*neighbourTile[i]) == typeid(Repeater) || typeid(*neighbourTile[i]) == typeid(Counter))
			{
				if (neighbourTile[i]->pos.FacingPosition(neighbourTile[i]->facing) == this->pos)
					neighbourSignals[i] = neighbourTile[i]->signal + 1;
			}
			else if (typeid(*neighbourTile[i]) == typeid(Memory))
			{
				if (neighbourTile[i]->pos.BehindPosition(neighbourTile[i]->facing) != this->pos)
					neighbourSignals[i] = neighbourTile[i]->signal + 1;
			}
			// If neighbour is 'signal provider'
			else if (typeid(*neighbourTile[i]) == typeid(PressurePlate))
			{
				neighbourSignals[i] = neighbourTile[i]->signal + 1;
			}
			else if(neighbourTile[i]->colorClass == this->colorClass)
			{
				// If neighbour is wire or redirector or holder
				neighbourSignals[i] = neighbourTile[i]->signal;
			}
			else
			{
				// If neigbour is wire or redirector that doesn't share the same colorclass don't update it
				neighbourTile[i] = nullptr;
			}
		}
	}

	// This tiles signal will be the max value of its neighbours - 1
	int max_value = *std::max_element(neighbourSignals.begin(), neighbourSignals.end());
	signal = max_value > 0 ? max_value - 1 : 0;

	// Update neighbours
	if (this->prevSignal != this->signal)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			if (neighbourTile[i])
			{
				neighbourTile[i]->DoWireLogic();
			}
		}
	}
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
		this->signal = Gconstants::maxSignalStrength;
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
		this->signal = Gconstants::maxSignalStrength;
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
	program.updateCounter++;
	Pos tileBack = this->pos.BehindPosition(this->facing);
	if (LogicTile* tileBehind = world.GetLogicTile(tileBack.CoordToEncoded()))
	{
		this->prevSignal = this->signal;
		this->signal = Gconstants::maxSignalStrength - tileBehind->signal;
		if (this->prevSignal != this->signal)
		{
			Pos tileFore = this->pos.FacingPosition(this->facing);
			if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
			{
				world.updateQueue.insert(tileFore.CoordToEncoded());
			}
		}
	}
	else
	{
		this->prevSignal = this->signal;
		this->signal = Gconstants::maxSignalStrength;
		if (this->prevSignal != this->signal)
		{
			Pos tileFore = this->pos.FacingPosition(this->facing);
			if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
			{
				world.updateQueue.insert(tileFore.CoordToEncoded());
			}
		}
	}
}

std::string Inverter::GetTooltip()
{
	return program.logicTooltips[3][0];
}

void Booster::DoWireLogic()
{
	program.updateCounter++;
	Pos tileBack = this->pos.BehindPosition(this->facing);
	if (LogicTile* tileBehind = world.GetLogicTile(tileBack.CoordToEncoded()))
	{
		this->prevSignal = this->signal;
		this->signal = tileBehind->signal > 0 ? Gconstants::maxSignalStrength : 0;
		if (this->prevSignal != this->signal)
		{
			Pos tileFore = this->pos.FacingPosition(this->facing);
			if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
			{
				world.updateQueue.insert(tileFore.CoordToEncoded());
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
	program.updateCounter++;
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
				world.updateQueue.insert(tileFore.CoordToEncoded());
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

void Memory::DoWireLogic()
{
	Pos tileBack = this->pos.BehindPosition(this->facing);
	if (auto temp = world.logictiles.GetValue(tileBack.CoordToEncoded()))
	{
		LogicTile* tileBehind = *temp;
		this->prevSignal = this->inputSignal;
		this->inputSignal = tileBehind->signal;
		if (this->prevSignal != this->inputSignal && tileBehind->prevSignal == 0 && tileBehind->signal > 0)
		{
			memIndex++;
			if (memIndex > 15)
			{
				memIndex = 0;
			}
		}
	}
	if(memIndex >= 0)
		this->signal = memory[memIndex];
	Pos facingTile = this->pos.FacingPosition(this->facing);
	if (auto temp = world.logictiles.GetValue(facingTile.CoordToEncoded()))
	{
		LogicTile* neighbour = *temp;
		neighbour->DoWireLogic();
	}
}
void Memory::SetMemIndex(int i)
{
	this->memIndex = i;
	this->DoWireLogic();
}

std::string Memory::GetTooltip()
{
	return program.logicTooltips[6][0];
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

