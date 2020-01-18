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
void Robot::PushItems(std::vector<Pos>* itemsMoving, Facing toward, int pushesLeft)
{
	Pos prevPos = itemsMoving->back();
	ItemTile* prevItem = world.GetItemTile(prevPos);
	if (pushesLeft == 0)
	{
		// There is an item at the front of the stack that isn't the same type
		if (ItemTile* nextItem = world.GetItemTile(prevPos.FacingPosition(toward)))
		{
			if (prevItem->itemTile != nextItem->itemTile && nextItem->quantity > 1)
			{
				itemsMoving->pop_back();
			}
		}
		return;
	}
	if (ItemTile* nextItem = world.GetItemTile(prevPos.FacingPosition(toward)))
	{
		if (prevItem->itemTile != nextItem->itemTile && nextItem->quantity > 1)
		{
			return;
		}
		else
		{
			itemsMoving->emplace_back(prevPos.FacingPosition(toward));
			PushItems(itemsMoving, toward, pushesLeft - 1);
		}
	}
	else
	{
		itemsMoving->emplace_back(prevPos.FacingPosition(toward));
		return;
	}
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
			if (auto robotNext = world.nextRobotPos.GetValue(newPos.CoordToEncoded()) == nullptr)
			{
				// Robot cannot move into void.
				if (GroundTile * groundTile = world.GetGroundTile(newPos))
				{
					// If next tile has an item on it and robot is not carying an item
					// reduce the quantity of the tile and assign the item id to the robot
					if (ItemTile* nextTile = world.GetItemTile(newPos))
					{
						std::vector<Pos> pushStack = { newPos};
						PushItems(&pushStack, this->facing, GC::robotStrength);
						if (!pushStack.empty())
						{
							for (uint16_t i=1; i<pushStack.size();i++)
							{
								world.nextItemPos.insert({ pushStack[i - 1].CoordToEncoded(),pushStack[i].CoordToEncoded() });
							}
						}
						/*
						if (this->itemCarying == (uint16_t)ReservedItems::nothing && nextTile->quantity != 0)
						{
							itemCarying = nextTile->itemTile;
							world.ChangeItem(newPos, nextTile->itemTile, -1);
							Rotate(2);
						}*/
					}
					world.nextRobotPos.insert({ this->pos.CoordToEncoded(), newPos.CoordToEncoded() });
					return true;
				}
			}
		}
	}
	return false;
}
void Robot::Drop()
{
	if (this->itemCarying != (uint16_t)ReservedItems::nothing)
	{
		Pos itemPos = pos.FacingPosition(facing);
		{
			if (ItemTile * item = &world.items[itemPos.CoordToEncoded()])
			{
				if (item->itemTile == (uint16_t)ReservedItems::nothing)
				{
					item->itemTile = itemCarying;
					item->quantity++;
					//TryCrafting(itemCarying, itemPos);
					itemCarying = (uint16_t)ReservedItems::nothing;

					// Apply item logic to update pressureplates
					if (auto temp = world.logictiles.GetValue(itemPos.CoordToEncoded()))
					{
						LogicTile* logicTile = *temp;
						logicTile->DoItemLogic();
					}
				}
				else if (item->itemTile == itemCarying && item->quantity < GC::tileItemLimit)
				{
					item->quantity++;
					//TryCrafting(itemCarying, itemPos);
					itemCarying = (uint16_t)ReservedItems::nothing;
				}
			}
		}
	}
}

void Robot::DrawTile(int x, int y)
{
	if (facing)
	{
		sf::Sprite sprite;
		sprite.setTexture(*robotTexture);
		sprite.setTextureRect(sf::IntRect(64 * facing, 0, 32, 48));
		sprite.setPosition(float(x), float(y - GC::halfTileSize));
		program.robotSprites.emplace_back(sprite);
		if (itemCarying > 2)
		{
			sf::Sprite sprite;
			sprite.setTexture(*itemTextures[itemCarying]);
			sprite.setOrigin(GC::halfItemSprite, GC::halfTileSize + 10);
			float rotation = float(facing) * float(90);
			sprite.setRotation(rotation);
			sprite.setPosition(float(x + GC::halfTileSize), float(y + GC::halfTileSize));
			program.robotSprites.emplace_back(sprite);
		}
	}
	else
	{
		if (itemCarying > 2)
		{
			sf::Sprite sprite;
			sprite.setTexture(*itemTextures[itemCarying]);
			sprite.setOrigin(GC::halfItemSprite, GC::halfTileSize + 10);
			float rotation = float(facing) * float(90);
			sprite.setRotation(rotation);
			sprite.setPosition(float(x + GC::halfTileSize), float(y + GC::halfTileSize));
			program.robotSprites.emplace_back(sprite);
		}
		sf::Sprite sprite;
		sprite.setTexture(*robotTexture);
		sprite.setTextureRect(sf::IntRect(64 * facing, 0, 32, 48));
		sprite.setPosition(float(x), float(y - GC::halfTileSize));
		program.robotSprites.emplace_back(sprite);
	}
}