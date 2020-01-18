#include <SFML/Graphics.hpp>
#include "ProgramData.h"
#include "WorldSave.h"
#include "WidgetCreator.h"

void MouseMoved()
{
	if (!program.gamePaused)
	{
		Pos mouseHovering = (program.mousePos + program.cameraPos) >> GC::tileShift;
		if (Robot * robot = world.GetRobot(mouseHovering.CoordToEncoded()))
		{
			program.selectedRobot = robot;
			program.cameraPos = mouseHovering << GC::tileShift;
			program.redrawStatic = true;
			program.hotbarIndex = 0;
			program.hotbar[0] = nullptr;
		}
		else
		{
			bool foundLogic = false;
			for (uint8_t i = 0; i < program.hotbarSlots.size(); ++i)
			{
				sf::RectangleShape rect = program.hotbarSlots[i];
				sf::FloatRect rectBox(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
				if (rectBox.contains(sf::Vector2f(float(program.mousePos.x), float(program.mousePos.y))))
				{
					program.selectedLogicTile = program.hotbar[i];
					if(program.selectedLogicTile)
						foundLogic = true;
				}
			}
			if (!foundLogic)
			{
				if (LogicTile * logic = world.GetLogicTile(mouseHovering.CoordToEncoded()))
				{
					foundLogic = true;
					program.selectedLogicTile = logic;
				}
			}
			if (!foundLogic)
			{
				program.selectedLogicTile = nullptr;
			}
		}
	}
}

void LeftMousePressed()
{
	if (!program.gamePaused)
	{
		Pos mouseHovering = (program.mousePos + program.cameraPos) >> GC::tileShift;
		if (Robot * robot = world.robots.GetValue(mouseHovering.CoordToEncoded()))
		{
		}
		else
		{
			program.selectedRobot = nullptr;
		}
		if (GroundTile * withinMap = world.GetGroundTile(mouseHovering))
		{
			if (program.hotbarIndex < (int)program.hotbar.size())
			{
				if (program.hotbar[program.hotbarIndex])
				{
					LogicTile* hotbarElement = program.hotbar[program.hotbarIndex];
					LogicTile* logicPlace = hotbarElement->Copy();
					logicPlace->pos = mouseHovering;
					if(Robot* robot = world.GetRobot(mouseHovering))
					{
						logicPlace->DoRobotLogic(robot);
					}
					world.logictiles.insert({ logicPlace->pos.CoordToEncoded(), logicPlace });
					world.updateQueueC.insert(logicPlace->pos.CoordToEncoded());										// Queue update for placed element
					for (int i = 0; i < 4; i++)
					{
						world.updateQueueC.insert(logicPlace->pos.FacingPosition(Facing(i)).CoordToEncoded());
					}
					program.selectedLogicTile = logicPlace;
				}
				else
					program.selectedLogicTile = nullptr;
				
			}
		}
	}
}

void RightMousePressed()
{
	Pos mouseHovering = (program.mousePos + program.cameraPos) >> GC::tileShift;
	if (LogicTile * deleteLogic = world.GetLogicTile(mouseHovering.CoordToEncoded()))
	{
		world.logictiles.erase(mouseHovering.CoordToEncoded());
		if (Robot* robot = world.GetRobot(mouseHovering))
		{
			robot->stopped = false;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		world.updateQueueC.insert(mouseHovering.FacingPosition(Facing(i)).CoordToEncoded());
	}
	program.selectedLogicTile = nullptr;
}

void GameInput(sf::RenderWindow &window, sf::Event event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::W:
		{
			if (program.selectedRobot)
			{
				Pos oldPos = { program.selectedRobot->pos.x,program.selectedRobot->pos.y };
				Pos newPos = oldPos.FacingPosition(program.selectedRobot->facing);
				if (program.selectedRobot->Move())
				{
					program.cameraPos = newPos << GC::tileShift;
					world.robots.erase(oldPos.CoordToEncoded());
					program.redrawStatic = true;
					program.SwapBots();
					program.selectedRobot = world.robots.GetValue(newPos.CoordToEncoded());
				}
			}
			else
			{
				program.cameraPos.y -= GC::cameraSpeed;
				program.redrawStatic = true;
			}
		}break;
		case sf::Keyboard::A:
		{
			if (program.selectedRobot)
			{
				program.selectedRobot->Rotate(-1);
			}
			else
			{
				program.cameraPos.x -= GC::cameraSpeed;
				program.redrawStatic = true;
			}
		}break;
		case sf::Keyboard::S:
		{
			if (program.selectedRobot)
			{
				program.selectedRobot->Rotate(2);
			}
			else
			{
				program.cameraPos.y += GC::cameraSpeed;
				program.redrawStatic = true;
			}
		}break;
		case sf::Keyboard::D:
		{
			if (program.selectedRobot)
			{
				program.selectedRobot->Rotate(1);
			}
			else
			{
				program.cameraPos.x += GC::cameraSpeed;
				program.redrawStatic = true;
			}
		}break;
		case sf::Keyboard::E:
		{
			if (program.selectedRobot)
			{
				program.selectedRobot->Drop();
			}
		}break;
		case sf::Keyboard::R:
		{
			if (program.selectedLogicTile)
			{
				program.selectedLogicTile->facing = Facing((int(program.selectedLogicTile->facing) + 1) & 3);
				for (int i = 0; i < 4; i++)
				{
					world.updateQueueC.insert(program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded());
				}
				program.placeRotation = program.selectedLogicTile->facing;
			}
			else
			{
				program.placeRotation = Facing((int(program.placeRotation) + 1) & 3);
			}
		}break;
		case sf::Keyboard::Q:
		{
			if (program.selectedLogicTile)
			{
				program.hotbar[0] = program.selectedLogicTile->Copy();
				program.placeRotation = program.selectedLogicTile->facing;
			}
			program.hotbarIndex = 0;
		}break;
		case sf::Keyboard::Num1:
		{
			program.hotbarIndex = 0;
		}break;
		case sf::Keyboard::Num2:
		{
			program.hotbarIndex = 1;
		}break;
		case sf::Keyboard::Num3:
		{
			program.hotbarIndex = 2;
		}break;
		case sf::Keyboard::Num4:
		{
			program.hotbarIndex = 3;
		}break;
		case sf::Keyboard::Num5:
		{
			program.hotbarIndex = 4;
		}break;
		case sf::Keyboard::Num6:
		{
			program.hotbarIndex = 5;
		}break;
		case sf::Keyboard::Num7:
		{
			program.hotbarIndex = 6;
		}break;
		case sf::Keyboard::Num8:
		{
			program.hotbarIndex = 7;
		}break;
		case sf::Keyboard::Num9:
		{
			program.hotbarIndex = 8;
		}break;
		case sf::Keyboard::Num0:
		{
			program.hotbarIndex = 9;
		}break;
		case sf::Keyboard::Delete:
		{
			if (program.selectedLogicTile)
			{
				world.logictiles.erase(program.selectedLogicTile->pos.CoordToEncoded());
				for (int i = 0; i < 4; i++)
				{
					world.updateQueueC.insert(program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded());
				}
				program.selectedLogicTile = nullptr;
			}
		}break;
		case sf::Keyboard::Escape:
		{
			program.selectedLogicTile = nullptr;
			if (program.showMain)
			{
				program.showMain = false;
				program.gamePaused = false;
			}
			else if (program.showSave)
			{
				program.showSave = false;
				program.showMain = true;
			}
			else
			{
				program.gamePaused = true;
				program.showMain = true;
			}
		}break;
		}
	}
	if (event.type == sf::Event::MouseMoved)
	{
		sf::Vector2i tempPos = sf::Mouse::getPosition(window);
		program.mousePos = Pos{ tempPos.x - (program.windowWidth >> 1),tempPos.y - (program.windowHeight >> 1) };
		MouseMoved();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			LeftMousePressed();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			RightMousePressed();
	}
	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			LeftMousePressed();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			RightMousePressed();
	}
	if (event.type == sf::Event::MouseButtonReleased)
	{

	}
}