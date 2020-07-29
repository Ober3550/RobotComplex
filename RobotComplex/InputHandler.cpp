#include "GuiHandler.h"
#include "WorldSave.h"
#include "ProgramData.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "MyMod.h"
#include "KeyNames.h"
#include "windows.h"
#include "MyStrings.h"
#include "Substrings.h"

void GuiHandler::LoadDefaultKeyMapping()
{
	sf::Event::KeyEvent keyPress;
	keyPress = { sf::Keyboard::W, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Move North" });

	keyPress = { sf::Keyboard::A, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Move West" });

	keyPress = { sf::Keyboard::S, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Move South" });

	keyPress = { sf::Keyboard::D, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Move East" });

	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Rotate Clockwise" });

	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ true, /*system*/ false };
	eventToAction.insert({ keyPress, "Rotate Anti-Clockwise" });

	keyPress = { sf::Keyboard::Q, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Empty Hand" });

	keyPress = { sf::Keyboard::X, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Set Robot Auto" });

	keyPress = { sf::Keyboard::Z, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Swap hotbar" });

	keyPress = { sf::Keyboard::C, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Change ColorClass" });

	keyPress = { sf::Keyboard::LAlt, /*alt*/ true, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Show Detailed Info" });

	keyPress = { sf::Keyboard::C, /*alt*/ false, /*ctrl*/ true, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Copy" });

	keyPress = { sf::Keyboard::X, /*alt*/ false, /*ctrl*/ true, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Cut" });

	keyPress = { sf::Keyboard::V, /*alt*/ false, /*ctrl*/ true, /*shift*/ false, /*system*/ false };
	eventToAction.insert({ keyPress, "Paste" });

	for (int i = 0; i < 21; i++)
	{
		keyPress = { sf::Keyboard::Key(int(sf::Keyboard::Num0) + MyMod(i + 1,10)), /*alt*/ false, /*ctrl*/ false, /*shift*/ i > 9, /*system*/ false };
		eventToAction.insert({ keyPress, "Hotbar " + std::to_string(i + 1) });
	}
}

void GuiHandler::CreateActions()
{
	//W
	userActionOrder.push_back("Move North");
	userActions.insert({ "Move North",[&] {
		if (program.selectedRobot)
		{
			program.moveBot = true;
		}
		else
		{
			//world.MovePlatform(program.mouseHovering,north);
			program.cameraPos.y -= int(GC::cameraSpeed * program.zoom);
		}
	} });

	//A
	userActionOrder.push_back("Move West");
	userActions.insert({ "Move West",[&] {
		if (program.selectedRobot)
		{
			if (handler.heldTick["Move West"] == world.tick)
				program.rotateBot = -1;
		}
		else
		{
			//world.MovePlatform(program.mouseHovering,west);
			program.cameraPos.x -= int(GC::cameraSpeed * program.zoom);
		}
	} });

	//S
	userActionOrder.push_back("Move South");
	userActions.insert({ "Move South",[&] {
		if (program.selectedRobot)
		{
			if (handler.heldTick["Move South"] == world.tick)
				program.rotateBot = 2;
		}
		else
		{
			//world.MovePlatform(program.mouseHovering,south);
			program.cameraPos.y += int(GC::cameraSpeed * program.zoom);
		}
	} });

	//D
	userActionOrder.push_back("Move East");
	userActions.insert({ "Move East",[&] {
		if (program.selectedRobot)
		{
			if (handler.heldTick["Move East"] == world.tick)
				program.rotateBot = 1;
		}
		else
		{
			//world.MovePlatform(program.mouseHovering,east);
			program.cameraPos.x += int(GC::cameraSpeed * program.zoom);
		}
	} });

	//R
	userActionOrder.push_back("Rotate Clockwise");
	userActions.insert({ "Rotate Clockwise",[&] {

		if (program.selectedLogicTile)
		{
			program.placeRotation = program.selectedLogicTile->facing;
			program.placeRotation = Pos::RelativeFacing(program.placeRotation,1);
			program.selectedLogicTile->facing = program.placeRotation;
			for (int i = 0; i < 4; i++)
			{
				world.updateNext.insert({ program.mouseHovering.FacingPosition(Facing(i)).CoordToEncoded(), 1});
			}
		}
		else
		{
			program.placeRotation = Pos::RelativeFacing(program.placeRotation,1);
		}
	} });

	//R
	userActionOrder.push_back("Rotate Anti-Clockwise");
	userActions.insert({ "Rotate Anti-Clockwise",[&] {
		if (program.selectedLogicTile)
		{
			program.placeRotation = program.selectedLogicTile->facing;
			program.placeRotation = Pos::RelativeFacing(program.placeRotation, -1);
			program.selectedLogicTile->facing = program.placeRotation;
			for (int i = 0; i < 4; i++)
			{
				world.updateNext.insert({ program.mouseHovering.FacingPosition(Facing(i)).CoordToEncoded(), 1 });
			}
		}
		else
		{
			program.placeRotation = Pos::RelativeFacing(program.placeRotation, -1);
 }
	} });

	//Q
	userActionOrder.push_back("Empty Hand");
	userActions.insert({ "Empty Hand",[&] {
		if (program.selectedRobot)
			program.selectedRobot->stopped = true;
		program.selectedRobot = nullptr;
		program.hotbarIndex = { 255,255 };
		program.paste = false;
		program.copy = false;
		program.cut = false;
	} });

	//X
	userActionOrder.push_back("Set Robot Auto");
	userActions.insert({ "Set Robot Auto",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->stopped = false;
			if (LogicTile* logic = world.GetLogicTile(program.selectedRobotPos))
			{
				logic->DoRobotLogic(program.selectedRobotPos,program.selectedRobotPos);
			}
			program.selectedRobot = nullptr;
		}
	} });

	//Z
	userActionOrder.push_back("Swap hotbar");
	userActions.insert({ "Swap hotbar",[&] {
		// TODO
	} });

	//C
	userActionOrder.push_back("Change ColorClass");
	userActions.insert({ "Change ColorClass",[&] {
		if (program.placeColor == 1)
			program.placeColor = 4;
		else
			program.placeColor = 1;
	} });

	//Alt
	userActionOrder.push_back("Show Detailed Info");
	userActions.insert({ "Show Detailed Info",[&] {
		program.showSignalStrength = !program.showSignalStrength;
	} });

	//Ctrl + C
	userActionOrder.push_back("Copy");
	userActions.insert({ "Copy",[&] {
		program.hotbarIndex = {255,255};
		program.copy = true;
	} });

	//Ctrl + X
	userActionOrder.push_back("Cut");
	userActions.insert({ "Cut",[&] {
		program.hotbarIndex = {255,255};
		program.cut = true;
	} });

	//Ctrl + V
	userActionOrder.push_back("Paste");
	userActions.insert({ "Paste",[&] {
		program.paste = true;
	} });

	for (int i = 1; i < 21; i++)
	{
		userActionOrder.push_back("Hotbar " + std::to_string(i));
		userActions.insert({ "Hotbar " + std::to_string(i),[i] {program.hotbarIndex = {(uint8_t)MyMod(i - 1,10),(uint8_t)(i < 11)}; } });
	}

	// Start Selection
	userActions.insert({ "Start Selection", [&] {
		if (program.cut || program.copy)
		{
			program.startedSelection = true;
			program.startSelection = program.mouseHovering;
		}
		if (program.paste)
		{
			world.PasteSelection();
		}
	} });

	// Place Element
	userActions.insert({ "Place Element", [&] {
		if (program.hoveringHotbar != SmallPos{255,255})
		{
			program.hotbarIndex = program.hoveringHotbar;
		}
		else
		{
			if (GroundTile* withinMap = world.GetGroundTile(program.mouseHovering))
			{
				if (program.hotbarIndex == SmallPos{ 255,255 })
				{
					if (program.selectedRobot)
						program.selectedRobot->stopped = true;
					program.selectedRobot = nullptr;
				}
				else
				{
					auto kv = program.hotbar.find(program.hotbarIndex);
					if (kv != program.hotbar.end())
					{
						if (world.PlaceElement(program.mouseHovering, kv->second.itemTile))
						{
							// Item may be removed because of hub
							if(ItemTile* item = world.GetItemTile(program.mouseHovering))
								CraftingClass::TryCrafting(item->itemTile, program.mouseHovering);
							kv->second.quantity--;
							if (kv->second.quantity == 0)
							{
								program.hotbar.erase(kv->first);
							}
						}
					}
				}
			}
		}
	} });

	// Remove Element
	userActions.insert({ "Remove Element", [&] {
		bool elementFound = false;
		uint16_t element = 0;
		if (!elementFound)
			if (element = world.ChangeRobot(program.mouseHovering, -1))
				elementFound = true;
		if (!elementFound)
			if (element = world.ChangeItem(program.mouseHovering, -1, 0))
				elementFound = true;
		if (!elementFound)
			if (element = world.ChangeLogic(program.mouseHovering, -1, 0))
				elementFound = true;
		if (element)
			world.ChangeInventory(element, 1);
	} });

	// Every tick
	actionFrequency["Move North"] = 1;
	actionFrequency["Move East"] = 1;
	actionFrequency["Move South"] = 1;
	actionFrequency["Move West"] = 1;
	actionFrequency["Remove Element"] = 8;
	actionFrequency["Place Element"] = 10;
}

void GuiHandler::PerformActions()
{
	// There can only be one input for robot control...
	std::vector<std::string> mask = { "Move North","Move East","Move South","Move West" };
	int oldest[4] = { 0 };
	bool outerMask = false;
	for (std::pair<std::string, int> action : handler.heldTick)
	{
		if (action.second > 0)
		{
			bool masked = false;
			if (program.selectedRobot)
			{
				for (int i = 0; i < mask.size(); i++)
				{
					if (action.first == mask[i])
					{
						oldest[i] = 1 + world.tick - action.second;
						masked = true;
						outerMask = true;
					}
				}
			}
			if (!masked)
			{
				// If an action has a frequency it'll be repeated at that frequency while the action is held
				if (int* frequency = actionFrequency.GetValue(action.first))
				{
					if ((world.tick - action.second) % actionFrequency[action.first] == 0)
					{
						if (std::function<void()>* func = handler.userActions.GetValue(action.first))
						{
							(*func)();
						}
					}
				}
				else
				{
					// If an action doesn't have a frequency assume that it'll only be activated on the first tick
					if (action.second == world.tick)
					{
						if (std::function<void()>* func = handler.userActions.GetValue(action.first))
						{
							(*func)();
						}
					}
				}
			}
		}
	}
	if (outerMask)
	{
		int max = 0;
		int maxInd = -1;
		for (int i = 0; i < 4; i++)
		{
			if (oldest[i] > max)
			{
				max = oldest[i];
				maxInd = i;
			}
		}
		if ((max - 1) % actionFrequency[mask[maxInd]] == 0)
		{
			if (std::function<void()>* func = handler.userActions.GetValue(mask[maxInd]))
			{
				if (mask[maxInd] == "Move North" && program.selectedRobot)
				{
					if (max - 1 == 0 || max > 20)
						(*func)();
				}
				else
				{
					(*func)();
				}
			}
		}
	}
}

void GuiHandler::MapNewButton(sf::Event::KeyEvent newButton)
{
	std::string oldAction = eventToAction[newButton];
	sf::Event::KeyEvent* oldButtonRemove = new sf::Event::KeyEvent;
	for (std::pair<sf::Event::KeyEvent, std::string> oldButton : eventToAction)
	{
		if (oldButton.second == oldAction)
			oldButtonRemove = &oldButton.first;
	}
	if (oldButtonRemove)
		eventToAction.erase(*oldButtonRemove);

	eventToAction[newButton] = actionBindWaiting;
	actionToEvent[actionBindWaiting] = newButton;

	// Clean up
	actionBindWaiting = "";
}

void GuiHandler::MouseMoved()
{
	if (!program.gamePaused)
	{

		if (Robot* robot = world.GetRobot(program.mouseHovering.CoordToEncoded()))
		{
			if (robot != program.selectedRobot)
			{
				if (program.selectedRobot)
					program.selectedRobot->stopped = true;
				program.selectedRobot = robot;
				program.selectedRobotPos = program.mouseHovering;
				program.selectedRobot->stopped = false;
				if (LogicTile* logic = world.GetLogicTile(program.selectedRobotPos))
				{
					logic->DoRobotLogic(program.selectedRobotPos, program.selectedRobotPos);
				}
			}
		}
	}
}

void GuiHandler::FinishedSelection(Pos start, Pos end)
{
	// Empty the old list when creating a new one
	program.copyMap.clear();
	// Make sure start is the top left position of the rect
	if (start.x > end.x)
	{
		int temp = end.x;
		end.x = start.x;
		start.x = temp;
	}
	if (start.y > end.y)
	{
		int temp = end.y;
		end.y = start.y;
		start.y = temp;
	}
	program.originSelection = start;
	// Search the region and add elements to a list
	for (int i = start.x; i < end.x; i++)
	{
		for (int j = start.y; j < end.y; j++)
		{
			if (LogicTile* logic = world.GetLogicTile(Pos{ i,j }))
			{
				LogicTile copyLogic = *logic;
				copyLogic.quantity = 1;
				program.copyMap.insert({ (Pos{ i,j } -start).CoordToEncoded(), copyLogic });
				if (program.cut)
				{
					uint16_t element = 0;
					if (program.cut)
						element = world.ChangeLogic(Pos{ i,j }, -1, 0);
					if (element)
					{
						world.ChangeInventory(element, 1);
					}
				}
			}
		}
	}
	program.cut = false;
	program.copy = false;
	program.startedSelection = false;
	program.paste = true;
}


void GuiHandler::HandleInput(sf::Event input, sf::RenderWindow& window)
{
	const int maskedKeys[6] = { sf::Keyboard::LControl ,sf::Keyboard::RControl, sf::Keyboard::LShift , sf::Keyboard::RShift , sf::Keyboard::LAlt ,sf::Keyboard::RAlt };
	if (input.type == sf::Event::KeyPressed)
	{
		if (actionBindWaiting != "")
		{
			for (int mask : maskedKeys)
			{
				if ((int)input.key.code == mask)
					return;
			}
			MapNewButton(input.key);
		}
		else
		{
			auto action = eventToAction.find(input.key);
			if (action != eventToAction.end())
			{
				auto activated = heldTick.find(action->second);
				if (activated != heldTick.end())
				{
					if (activated->second == 0)
					{
						heldTick[action->second] = world.tick;
					}
				}
				else
				{
					heldTick[action->second] = world.tick;
				}
			}
		}
	}
	else if (input.type == sf::Event::KeyReleased)
	{
		if (actionBindWaiting != "")
		{
			MapNewButton(input.key);
		}
		else if (std::string* action = eventToAction.GetValue(input.key))
		{
			heldTick[*action] = 0;
		}
	}
	else if (input.type == sf::Event::MouseMoved)
	{
		program.prevMouseHovering = program.mouseHovering;
		program.mouseHovering = ((program.mousePos * program.zoom) + program.cameraPos) / float(GC::tileSize);
		MouseMoved();
		if (program.mouseHovering != program.prevMouseHovering)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				heldTick["Place Element"] = world.tick;
		}
	}
	else if (input.type == sf::Event::MouseButtonPressed)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			heldTick["Start Selection"] = world.tick;
			heldTick["Place Element"] = world.tick;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			heldTick["Remove Element"] = world.tick;
		}
	}
	else if (input.type == sf::Event::MouseButtonReleased)
	{
		if (input.mouseButton.button == sf::Mouse::Button::Right)
		{
			heldTick["Remove Element"] = 0;
		}
		else if (input.mouseButton.button == sf::Mouse::Button::Left)
		{
			heldTick["Place Element"] = 0;
			heldTick["Start Selection"] = 0;
			if (program.startedSelection)
			{
				FinishedSelection(program.startSelection, program.mouseHovering);
			}
		}
	}
	else if (input.type == sf::Event::MouseWheelScrolled)
	{
		if (!program.gamePaused)
		{
			if (input.mouseWheelScroll.delta != 0)
			{
				program.zoom -= input.mouseWheelScroll.delta * GC::ZOOM_VELOCITY;
				if (program.zoom < GC::MIN_ZOOM)
					program.zoom = GC::MIN_ZOOM;
				if (program.zoom > GC::MAX_ZOOM)
					program.zoom = GC::MAX_ZOOM;
				program.RecalculateMousePos();
			}
		}
	}
}

void GuiHandler::SaveProgramSettings()
{
	world.Serialize(world.name);
	std::ofstream myfile;
	myfile.open("saves/config.txt", std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		myfile << world.name + "\r\n";
		for (std::pair<sf::Event::KeyEvent, std::string> action : eventToAction)
		{
			myfile << action.second + ":" + KeyNames::toString(action.first) + "\r\n";
		}
	}
	myfile.close();
}

// Returns true when a key mapping config has been found
bool GuiHandler::LoadProgramSettings()
{
	std::ifstream myfile("saves/config.txt");
	if (!myfile.is_open())
		return false;
	std::string str;
	int i = 0;
	while (std::getline(myfile, str))
	{
		if (i == 0)
		{
			world.name = str;
		}
		else if (i > 0)
		{
			std::vector<std::string> splitLine;
			split(&splitLine, str, ':');
			if (splitLine.size() != 0)
			{
				if (sf::Event::KeyEvent* valid = KeyNames::toEvent(splitLine[1]))
				{
					eventToAction.insert({ *valid, splitLine[0] });
				}
			}
		}
		i++;
	}
	myfile.close();
	//world.Deserialize(world.name);
	return true;
}

void GuiHandler::FindRecipes(std::string name)
{
	program.foundRecipeList.clear();
	name = lowercase(swapChar(name, ' ', '_'));
	if (name != "")
	{
		std::vector<uint16_t> foundNames = findSubstrings(&program.itemPrototypes, name);
		for (uint16_t element : foundNames)
		{
			if (auto recipeList = program.itemResultList.GetValue(element))
			{
				for (uint16_t recipe : *recipeList)
				{
					program.foundRecipeList.emplace_back(recipe);
				}
			}
		}
	}
	else
	{
		for (auto recipes : program.itemResultList)
		{
			for (uint16_t recipe : recipes.second)
			{
				program.foundRecipeList.emplace_back(recipe);
			}
		}
	}
	program.craftingViewIndex = 0;
	if (program.foundRecipeList.size() > 0)
	{
		resultsTitle = "Results: " + std::to_string(program.craftingViewIndex + 1) + "/" + std::to_string(program.foundRecipeList.size());
	}
	program.craftingViewUpdate = true;
}