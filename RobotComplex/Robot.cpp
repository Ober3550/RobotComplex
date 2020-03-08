#include "Pos.h"
#include "LogicTile.h"
#include "Robot.h"
#include "CraftingClass.h"
#include "WorldSave.h"
#include "ProgramData.h"
#include "Constants.h"
#include "Facing.h"
#include "ReservedItems.h"
#include "Textures.h"

void Robot::SetFacing(Facing direction)
{
	this->facing = direction;
}
void Robot::Rotate(int r)
{
	// Converts enum to int to apply rotation. 
	// Then applies mask to have within range of 0-3
	// Then converts back to enum
	facing = Facing((int(facing) + r) & 3);
}
bool Robot::Move()
{
	// For logic tile that stops robot untill it receives a signal
	if (!stopped)
	{
		Pos newPos = pos.FacingPosition(facing);
		// If there is no robot in the stationary map ie. No collision between moving -> stationary
		if (auto robot = world.robots.GetValue(newPos.CoordToEncoded()) == nullptr)
		{
			// If there is no robot in the moved map ie. No collision between moving -> moved
			if (!world.robotMovingTo.contains(newPos.CoordToEncoded()))
			{
				// Robot cannot move into void.
				if (GroundTile * groundTile = world.GetGroundTile(newPos))
				{
					// If next tile has an item on it and robot is not carying an item
					// reduce the quantity of the tile and assign the item id to the robot
					bool step = true;
					if (ItemTile* nextTile = world.GetItemTile(newPos))
					{
						std::vector<Pos> pushStack = { newPos };
						step = world.PushItems(&pushStack, this->facing, GC::robotStrength);
						if (!pushStack.empty())
						{
							for (uint16_t i = 1; i < pushStack.size(); i++)
							{
								world.itemMovingTo.insert(pushStack[i].CoordToEncoded());
								world.nextItemPos.insert({ pushStack[i - 1].CoordToEncoded(),this->facing });
							}
						}
					}
					if (step)
					{
						world.robotMovingTo.insert(newPos.CoordToEncoded());
						world.nextRobotPos.insert({ this->pos.CoordToEncoded(), this->facing });
						return true;
					}
					else
					{
						bool pushFailed = true;
					}
				}
			}
		}
	}
	return false;
}

void Robot::DrawTile(SpriteVector* appendTo, int x, int y, float s, uint8_t flags, sf::Color color)
{
	sf::Sprite sprite;
	sprite.setTexture(*robotTexture);
	sprite.setOrigin(16, 32);
	sprite.setPosition(float(x + 16), float(y + 16));
	sprite.setScale(sf::Vector2f(s, s));

	sprite.setColor(color);
	sprite.setTextureRect(sf::IntRect(64 * facing, 48, 32, 48));
	appendTo->emplace_back(sprite);
	
	sprite.setColor(sf::Color(255, 255, 255, 255));
	sprite.setTextureRect(sf::IntRect(64 * facing, 0, 32, 48));
	appendTo->emplace_back(sprite);
}

void Robot::Serialize(std::ofstream* writer)
{
	writer->write((char*)&this->pos, sizeof(Pos));
	writer->write((char*)&this->facing, sizeof(Facing));
	writer->write((char*)&this->stopped, sizeof(bool));
}
void Robot::Deserialize(std::ifstream* reader)
{
	reader->read((char*)&this->pos, sizeof(Pos));
	reader->read((char*)&this->facing, sizeof(Facing));
	reader->read((char*)&this->stopped, sizeof(bool));
}