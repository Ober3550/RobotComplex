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

void LogicTile::DoItemLogic(Pos currentPos)
{
	switch (this->logicType)
	{
	case pressureplate:
	{
		uint8_t prevSignal;
		if (ItemTile* thisTile = world.GetItemTile(currentPos))
		{
			prevSignal = this->signal;
			this->signal = MyMod(thisTile->quantity, GC::maxSignalStrength);
		}
		else
		{
			prevSignal = this->signal;
			this->signal = 0;
		}
		if (prevSignal != this->signal)
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				Pos facingTile = currentPos.FacingPosition(Facing(i));
				if (LogicTile* neighbour = world.GetLogicTile(facingTile.CoordToEncoded()))
				{
					neighbour->DoWireLogic(facingTile);
				}
			}
		}
	}break;
	case belt:
	{
		if (signal == 0)
		{
			if (ItemTile* item = world.GetItemTile(currentPos))
			{
				std::vector<Pos> pushStack = { currentPos };
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
	}break;
	case shover:
	{
		if (ItemTile* item = world.GetItemTile(currentPos))
		{
			std::vector<Pos> pushStack = { currentPos };
			world.PushItems(&pushStack, this->facing, this->signal);
			if (!pushStack.empty())
			{
				for (uint16_t i = 1; i < pushStack.size(); i++)
				{
					world.nextItemPos.insert({ pushStack[i - 1].CoordToEncoded(),this->facing });
					world.updateQueueD.insert(pushStack[i - 1].FacingPosition(this->facing).CoordToEncoded());
				}
			}
		}
	}break;
	case hub:
	{
		if (ItemTile* item = world.GetItemTile(currentPos))
		{
			world.resourcesDelivered[item->itemTile] += item->quantity;
			program.technologyViewUpdate = true;
			world.ChangeItem(currentPos, -255, item->itemTile);
		}
	}break;
	default:
		break;
	}
}

void LogicTile::DoRobotLogic(Pos currentPos, Pos robotRef)
{
	switch (this->logicType)
	{
	case redirector:
	{
		if (!signal)
		{
			if (Robot* robot = world.GetRobot(robotRef))
			{
				robot->SetFacing(this->facing);
			}
		}
	}break;
	case pressureplate:
	{
		uint8_t prevSignal;
		if (Robot* robot = world.GetRobot(robotRef))
		{
			prevSignal = this->signal;
			this->signal = GC::startSignalStrength;
		}
		else
		{
			prevSignal = this->signal;
			this->signal = 0;
		}
		if (prevSignal != this->signal)
		{
			for (uint8_t i = 0; i < 4; i++)
			{
				Pos facingTile = currentPos.FacingPosition(Facing(i));
				if (LogicTile* neighbour = world.GetLogicTile(facingTile.CoordToEncoded()))
				{
					neighbour->DoWireLogic(facingTile);
				}
			}
		}
	}break;
	case gate:
	{
		if (!signal)
		{
			if (Robot* robot = world.GetRobot(robotRef.CoordToEncoded()))
			{
				robot->stopped = true;
			}
		}
	}break;
	case belt:
	{
		if (!signal)
		{
			if (Robot* robot = world.GetRobot(robotRef))
			{
				robot->SetFacing(this->facing);
			}
		}
	}break;
	default:
		break;
	}
}