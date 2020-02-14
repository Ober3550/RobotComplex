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
	case gate:
		return new Gate();
	}
	return nullptr;
}

void LogicTile::Serialize(std::ofstream* writer)
{
	writer->write((char*)&this->logictype,	sizeof(LogicTypes));
	writer->write((char*)&this->pos,		sizeof(Pos));
	writer->write((char*)&this->facing,		sizeof(Facing));
	writer->write((char*)&this->prevSignal,	sizeof(uint8_t));
	writer->write((char*)&this->signal,		sizeof(uint8_t));
	//writer->write((char*)&this->colorClass,	sizeof(uint8_t));
}

void LogicTile::Deserialize(std::ifstream* reader)
{
	reader->read((char*)&this->pos,			sizeof(Pos));
	reader->read((char*)&this->facing,		sizeof(Facing));
	reader->read((char*)&this->prevSignal,	sizeof(uint8_t));
	reader->read((char*)&this->signal,		sizeof(uint8_t));
	//reader->read((char*)&this->colorClass,	sizeof(uint8_t));
}

void Redirector::Serialize(std::ofstream* writer)
{
	LogicTile::Serialize(writer);
}

void Redirector::Deserialize(std::ifstream* reader)
{
	LogicTile::Deserialize(reader);
}

void LogicTile::BaseCopy(LogicTile* logicTile)
{
	logicTile->logictype = this->logictype;
	logicTile->pos = this->pos;
	logicTile->facing = this->facing;
	logicTile->prevSignal = this->prevSignal;
	logicTile->colorClass = this->colorClass;
}
bool LogicTile::GetConnected(LogicTile* querier)
{
	if (querier)
	{
		if (querier->IsSource())
			return true;
		if (this->IsSource())
			return true;
		if (querier->colorClass == this->colorClass)
			return true;
	}
	return false;
}
bool DirectionalLogicTile::GetConnected(LogicTile* querier)
{
	return true;
}
bool DirectionalLogicTile::ReceivesSignal(LogicTile* querier)
{
	if (this->pos.FacingPosition(this->facing) == querier->pos)
		return true;
	else
		return false;
}
void LogicTile::DoWireLogic() {
	//Sleep(100);
	std::array<uint8_t, 4> neighbourSignals = { 0,0,0,0 };
	std::array<LogicTile*, 4> neighbourTile = std::array<LogicTile*, 4>();
	this->prevSignal = this->signal;

	// output, b1, a, b2
	for (int i = 0; i < 4; i++)
	{
		if (neighbourTile[i] = world.GetLogicTile(this->pos.FacingPosition(Pos::RelativeFacing(this->facing, i))))
		{
			if (neighbourTile[i]->GetConnected(this))
			{
				if (neighbourTile[i]->ReceivesSignal(this))
					if (neighbourTile[i]->IsSource())
						neighbourSignals[i] = neighbourTile[i]->GetSignal(this) + 1;
					else
						neighbourSignals[i] = neighbourTile[i]->GetSignal(this);
			}
			if (this->GetConnected(neighbourTile[i]))
			{
				if (neighbourTile[i]->ReceivesSignal(this))
					if (neighbourTile[i]->IsSource())
						neighbourSignals[i] = neighbourTile[i]->GetSignal(this) + 1;
					else
						neighbourSignals[i] = neighbourTile[i]->GetSignal(this);
			}
		}
	}

	SignalEval(neighbourSignals);

	for (uint8_t i = 0; i < 4; i++)
	{
		if (neighbourTile[i])
		{
			if (neighbourTile[i]->NeighbourConnects(this))
			{
				// Update neighbours
				if (this->prevSignal != this->signal || (neighbourTile[i]->GetSignal(this) == 0 && this->GetSignal(neighbourTile[i]) > 0))
				{
					//if (neighbourTile[i]->GetSignal(this) != 0 || this->GetSignal(neighbourTile[i]) != 0)
					neighbourTile[i]->QueueUpdate();
				}
			}
		}
	}
}

void LogicTile::QueueUpdate()
{
	world.updateQueueB.insert({ this->pos.CoordToEncoded() });
}

// This tiles signal will be the max value of its neighbours - 1
void LogicTile::SignalEval(std::array<uint8_t, 4> neighbours)
{
	int max_value = std::max({ neighbours[0],neighbours[1],neighbours[2],neighbours[3] });
	if (this->signal > max_value)
	{
		this->signal = 0;
	}
	else
	{
		signal = max_value > 0 ? max_value - 1 : 0;
	}
}

void DirectionalLogicTile::DoWireLogic()
{
	//Sleep(100);
	// output, b1, a, b2
	std::array<uint8_t, 4> neighbourSignals = { 0,0,0,0 };
	for (int i = 0; i < 4; i++)
	{
		if (LogicTile* tile = world.GetLogicTile(this->pos.FacingPosition(Pos::RelativeFacing(this->facing, i))))
		{
			if (tile->GetConnected(this))
				if(tile->ReceivesSignal(this))
					neighbourSignals[i] = tile->GetSignal(this);
		}
	}
	this->SignalEval(neighbourSignals);
	// Update element infront
	if (this->prevSignal != this->signal)
	{
		Pos tileFore = this->pos.FacingPosition(this->facing);
		if (LogicTile* tileInfront = world.GetLogicTile(tileFore.CoordToEncoded()))
		{
			tileInfront->QueueUpdate();
		}
	}
}
void DirectionalLogicTile::QueueUpdate()
{
	world.updateQueueC.insert({ this->pos.CoordToEncoded(),1 });
}
std::string Wire::GetTooltip()
{
	return program.logicTooltips[0][0];
}
void Redirector::DoRobotLogic(Pos robotRef)
{
	if (!signal)
	{
		if (Robot* robot = world.GetRobot(robotRef))
		{
			robot->SetFacing(this->facing);
		}
	}
}
std::string Redirector::GetTooltip()
{
	if(this->signal)
		return program.logicTooltips[1][0];
	return program.logicTooltips[1][1];
}

void PressurePlate::DoItemLogic()
{
	if (ItemTile * thisTile = world.GetItemTile(this->pos))
	{
		this->prevSignal = this->signal;
		this->signal = GC::startSignalStrength;
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
void PressurePlate::DoRobotLogic(Pos robotRef)
{
	if (Robot* robot = world.GetRobot(robotRef))
	{
		this->prevSignal = this->signal;
		this->signal = GC::startSignalStrength;
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

void Inverter::SignalEval(std::array<uint8_t, 4> neighbours)
{
	int b = std::max(neighbours[1], neighbours[3]);
	if (GC::startSignalStrength - neighbours[2] + b > 0)
		this->signal = MyMod(GC::startSignalStrength - neighbours[2] + b, GC::maxSignalStrength);
	else
		this->signal = 0;
}

std::string Inverter::GetTooltip()
{
	return program.logicTooltips[3][0];
}

void Booster::SignalEval(std::array<uint8_t, 4> neighbours)
{
	int b = std::max(neighbours[1], neighbours[3]);
	if (neighbours[2] > b)
		this->signal = MyMod(neighbours[2] + GC::startSignalStrength, GC::maxSignalStrength);
	else
		this->signal = 0;
}

void Comparer::SignalEval(std::array<uint8_t, 4> neighbours)
{
	int b = std::max(neighbours[1], neighbours[3]);
	if (neighbours[2] == b)
		this->signal = MyMod(neighbours[2] + GC::startSignalStrength, GC::maxSignalStrength);
	else
		this->signal = 0;
}

std::string Booster::GetTooltip()
{
	return program.logicTooltips[4][0];
}

void Repeater::SignalEval(std::array<uint8_t, 4> neighbours)
{
	//int b = std::max(neighbours[1], neighbours[3]);
	this->signal = neighbours[2];
}

std::string Repeater::GetTooltip()
{
	return program.logicTooltips[5][0];
}

void Gate::DoRobotLogic(Pos robotRef)
{
	if (!signal)
	{
		if (Robot* robot = world.GetRobot(robotRef.CoordToEncoded()))
		{
			robot->stopped = true;
		}
	}
}

void Gate::DoWireLogic()
{
	LogicTile::DoWireLogic();
	if (signal)
	{
		if (Robot* robot = world.GetRobot(this->pos.CoordToEncoded()))
		{
			robot->stopped = false;
		}
	}
}

std::string Gate::GetTooltip()
{
	return program.logicTooltips[7][0];
}

void Counter::SignalEval(std::array<uint8_t, 4> neighbours)
{
	this->prevSignal = this->inputSignal;
	this->inputSignal = neighbours[2];
	if (LogicTile* temp = world.GetLogicTile(this->pos.BehindFacing(this->facing)))
	{
		// Rising edge of a logical element
		if (this->prevSignal != this->inputSignal && temp->prevSignal == 0 && neighbours[2] > 0)
		{
			++this->signal;
			if (this->signal > 15)
			{
				this->signal = 0;
			}
		}
	}
}

std::string Counter::GetTooltip()
{
	return program.logicTooltips[8][0];
}

void Belt::DoRobotLogic(Pos robotRef)
{
	if (!signal)
	{
		if (Robot* robot = world.GetRobot(robotRef))
		{
			robot->SetFacing(this->facing);
		}
	}
}

void Belt::DoItemLogic()
{
	if (signal == 0)
	{
		if (ItemTile* item = world.GetItemTile(this->pos))
		{
			std::vector<Pos> pushStack = { this->pos };
			world.PushItems(&pushStack, this->facing, GC::robotStrength);
			if (!pushStack.empty())
			{
				for (uint16_t i = 1; i < pushStack.size(); i++)
				{
					world.nextItemPos.insert({ pushStack[i - 1].CoordToEncoded(),this->facing });
					world.updateQueueD.insert(pushStack[i - 1].FacingPosition(this->facing).CoordToEncoded());
				}
			}
		}
	}
}

uint8_t WireBridge::GetSignal(LogicTile* querier)
{
	if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
	{
		return this->signal;
	}
	else
	{
		return this->signal2;
	}
}

bool WireBridge::GetConnected(LogicTile* querier)
{
	if (this->pos.FacingPosition(this->facing) == querier->pos || this->pos.BehindPosition(this->facing) == querier->pos)
	{
		if(querier->colorClass == GC::colorClassA)
		return true;
	}
	else
	{
		if (querier->colorClass == GC::colorClassB)
		return true;
	}
	return false;
}

void WireBridge::SignalEval(std::array<uint8_t, 4> neighbours)
{
	uint8_t temp = signal2;
	uint8_t maxA = std::max({ neighbours[0],neighbours[2] });
	uint8_t maxB = std::max({ neighbours[1],neighbours[3] });
	if (this->signal > maxA)
		this->signal = 0;
	else
		signal = maxA > 0 ? maxA - 1 : 0;
	if (this->signal2 > maxB)
		this->signal2 = 0;
	else
		signal2 = maxB > 0 ? maxB - 1 : 0;

	if (temp != signal2)
		this->prevSignal = this->signal - 1;
}