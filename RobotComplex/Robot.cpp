#include "Pos.h"
#include "LogicTile.h"
#include "Robot.h"
#include "CraftingClass.h"
#include "WorldSave.h"
#include "ProgramData.h"
#include "Constants.h"
#include "Facing.h"
#include "ReservedItems.h"

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
			if (auto robotNext = program.nextRobotPos.GetValue(newPos.CoordToEncoded()) == nullptr)
			{
				// Robot cannot move into void.
				if (GroundTile * groundTile = world.GetGroundTile(newPos))
				{
					ItemTile* nextTile = world.GetItemTile(newPos.CoordToEncoded());
					// If next tile has an item on it and robot is not carying an item
					// reduce the quantity of the tile and assign the item id to the robot
					if (nextTile)
					{
						if (this->itemCarying == (uint16_t)ReservedItem::nothing && nextTile->quantity != 0)
						{
							nextTile->quantity--;
							itemCarying = nextTile->itemTile;
							// If the tile has no items on it remove the item id
							if (nextTile->quantity == 0)
							{
								world.items.erase(newPos.CoordToEncoded());
								nextTile = nullptr;
								if (auto temp = world.logictiles.GetValue(newPos.CoordToEncoded()))
								{
									LogicTile* logicTile = *temp;
									logicTile->DoItemLogic();
								}
							}
							Rotate(2);
						}
					}
					// If no collisions move the robot
					if (nextTile == nullptr)
					{
						Pos tempPos = this->pos;
						this->pos = newPos;

						// If the robot landed on a logic tile, apply its logic this tick
						if (auto temp = world.logictiles.GetValue(newPos.CoordToEncoded()))
						{
							LogicTile* logicTile = *temp;
							logicTile->DoRobotLogic(this);
						}

						// If the robot left a logic tile, update its logic
						if (auto temp = world.logictiles.GetValue(tempPos.CoordToEncoded()))
						{
							LogicTile* logicTile = *temp;
							logicTile->DoRobotLogic(nullptr);
						}

						program.nextRobotPos.insert({ this->pos.CoordToEncoded(), *this });
						return true;
					}
				}
			}
		}
	}
	return false;
}
void Robot::TryCrafting(uint16_t item, Pos itemPos)
{
	if (auto recipeList = program.itemRecipeList.GetValue(item))
	{
		// Try to craft item when placed
		for (uint16_t recipe:*recipeList)
		{
			program.craftingRecipes[recipe].DoCrafting(itemPos);
		}
	}
}
void Robot::Drop()
{
	if (this->itemCarying != (uint16_t)ReservedItem::nothing)
	{
		Pos itemPos = pos.FacingPosition(facing);
		{
			if (ItemTile * item = &world.items[itemPos.CoordToEncoded()])
			{
				if (item->itemTile == (uint16_t)ReservedItem::nothing)
				{
					item->itemTile = itemCarying;
					item->quantity++;
					TryCrafting(itemCarying, itemPos);
					itemCarying = (uint16_t)ReservedItem::nothing;

					// Apply item logic to update pressureplates
					if (auto temp = world.logictiles.GetValue(itemPos.CoordToEncoded()))
					{
						LogicTile* logicTile = *temp;
						logicTile->DoItemLogic();
					}
				}
				else if (item->itemTile == itemCarying && item->quantity < Gconstants::tileItemLimit)
				{
					item->quantity++;
					TryCrafting(itemCarying, itemPos);
					itemCarying = (uint16_t)ReservedItem::nothing;
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
		sprite.setTexture(program.robotTexture);
		sprite.setTextureRect(sf::IntRect(64 * facing, 0, 32, 48));
		sprite.setPosition(float(x), float(y - Gconstants::halfTileSize));
		program.robotSprites.emplace_back(sprite);
		if (itemCarying > 2)
		{
			sf::Sprite sprite;
			sprite.setTexture(program.itemTextures[itemCarying]);
			sprite.setOrigin(Gconstants::halfItemSprite, Gconstants::halfTileSize + 10);
			float rotation = float(facing) * float(90);
			sprite.setRotation(rotation);
			sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));
			program.robotSprites.emplace_back(sprite);
		}
	}
	else
	{
		if (itemCarying > 2)
		{
			sf::Sprite sprite;
			sprite.setTexture(program.itemTextures[itemCarying]);
			sprite.setOrigin(Gconstants::halfItemSprite, Gconstants::halfTileSize + 10);
			float rotation = float(facing) * float(90);
			sprite.setRotation(rotation);
			sprite.setPosition(float(x + Gconstants::halfTileSize), float(y + Gconstants::halfTileSize));
			program.robotSprites.emplace_back(sprite);
		}
		sf::Sprite sprite;
		sprite.setTexture(program.robotTexture);
		sprite.setTextureRect(sf::IntRect(64 * facing, 0, 32, 48));
		sprite.setPosition(float(x), float(y - Gconstants::halfTileSize));
		program.robotSprites.emplace_back(sprite);
	}
}