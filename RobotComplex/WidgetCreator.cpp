#include "WidgetCreator.h"
#include "ProgramData.h"
#include "WorldSave.h"
#include <Windows.h>
#include <vector>
#include <string>
#include "FindInVector.h"
#include "KeyNames.h"
#include "GetFileNamesInFolder.h"
#include "MyMod.h"
#include "ItemTileWPOS.h"
#include "SplitString.h"

class SimpleButtonListener : public agui::ButtonListener
{
private:
	std::function<void()> buttonAction;
public:
	SimpleButtonListener(std::function<void()> buttonAction) {
		this->buttonAction = buttonAction;
	};
	void buttonStateChanged(agui::Button *, agui::Button::ButtonStateEnum state) {
		if (state == agui::Button::ButtonStateEnum::CLICKED)
		{
			if (creator->buttonsEnabled)
				buttonAction();
		}
	}
	void toggleStateChanged(agui::Button *, bool) {}
	void death(agui::Button *) {}
	void isToggleButtonChanged(agui::Button *, bool) {}
	void textAlignmentChanged(agui::Button *, agui::AreaAlignmentEnum) {}
	~SimpleButtonListener(void) {};
};

class SaveSelect : public agui::SelectionListener
{
	virtual void selectionChanged(agui::Widget *source, const std::string &item, int index, bool selected)
	{
		program.selectedSave = item;
	}
};

WidgetCreator::WidgetCreator(agui::Gui *guiInstance, sf::RenderWindow* windowRef)
{
	mGui = guiInstance;
	mGui->setDelayMouseDownEvents(false);
	window = windowRef;
	AddMainFrame();
	AddSaveFrame();
	AddNewWorldFrame();
	if (!LoadProgramSettings())
	{
		LoadDefaultKeyMapping();
	}
	CreateActionList();
	AddKeyMapFrame();
}
void WidgetCreator::AddMainFrame()
{
	mGui->add(&mainFrame);
	// Options mainFrame
	mainFrame.setText("Main Menu");
	int row = 0;
	
	// New World Button
	/*
	newWorldFromStart.setText("New World");
	newWorldFromStart.addButtonListener(new SimpleButtonListener([&] {
		// Don't push duplicates to the stack
		if (!creator->guiStack.empty())
			if (creator->guiStack.top() != &creator->newWorldFrame)
				creator->guiStack.push(&newWorldFrame);
		}));
	mainFrame.add(&newWorldFromStart);
	newWorldFromStart.setSize(gridSize * frameWidth, gridSize);
	newWorldFromStart.setLocation(0, row * gridSize);
	row++;*/

	// Play Button changes to load and save frame
	playButton.setSize(gridSize * frameWidth, gridSize);
	playButton.setText("Play");
	playButton.addButtonListener(new SimpleButtonListener([&] {
		// Don't push duplicates to the stack
		if (!creator->guiStack.empty())
			if (creator->guiStack.top() != &creator->saveFrame)
				creator->guiStack.push(&saveFrame);
		creator->worldSaves.clearItems();
		if (!CreateDirectory("saves", NULL) && ERROR_ALREADY_EXISTS != GetLastError())
			return;
		std::vector<std::string> names = getFolderNamesInFolder("saves");
		for (std::string name : names)
		{
			worldSaves.addItem(name);
		}
	}));
	mainFrame.add(&playButton);
	playButton.setLocation(0, row*gridSize);
	row++;
	// 'Controls' Button changes key mapping
	controlsButton.setText("Controls");
	controlsButton.addButtonListener(new SimpleButtonListener([&] {
		// Don't push duplicates to the stack
		if (!creator->guiStack.empty())
			if (creator->guiStack.top() != &creator->keyFrame)
				creator->guiStack.push(&keyFrame);
	}));
	mainFrame.add(&controlsButton);
	controlsButton.setSize(gridSize * frameWidth, gridSize);
	controlsButton.setLocation(0, row * gridSize);
	row++;

	// Back button exists on every frame 
	// such that those that aren't good with keyboard and mouse have easy navigation
	mainBackButton.setText("Back");
	mainBackButton.addButtonListener(new SimpleButtonListener([&] {
		if (!creator->guiStack.empty()) {
			creator->guiStack.pop();
		}
		}));
	mainFrame.add(&mainBackButton);
	mainBackButton.setSize(gridSize * frameWidth, gridSize);
	mainBackButton.setLocation(0, row * gridSize);
	row++;

	// Exit Button
	exitButton.setText("Exit Game");
	exitButton.addButtonListener(new SimpleButtonListener([&] {
		program.running = false;
		}));
	mainFrame.add(&exitButton);
	exitButton.setSize(gridSize * frameWidth, gridSize);
	exitButton.setLocation(0, row * gridSize);
	row++;

	mainFrame.setVisibility(true);
	// Ensure that top of gui stack is the main frame
	guiStack.push(&mainFrame);

	// Resize parent element according to its children
	mainFrame.setSize(agui::Dimension(gridSize * frameWidth + padding, row * gridSize + frameHeadPadding));
	mainFrame.setLocation((int)(program.windowWidth / 2) - (mainFrame.getSize().getWidth() / 2), (int)(program.windowHeight / 2) - (mainFrame.getSize().getHeight() / 2));
}
void WidgetCreator::AddSaveFrame()
{
	// Current vertical grid position
	int row = 0;

	// New World Button
	newWorldButton.setText("New World");
	newWorldButton.addButtonListener(new SimpleButtonListener([&] {
		// Don't push duplicates to the stack
		if (!creator->guiStack.empty())
			if (creator->guiStack.top() != &creator->newWorldFrame)
				creator->guiStack.push(&newWorldFrame);
		}));
	saveFrame.add(&newWorldButton);
	newWorldButton.setSize(gridSize * frameWidth, gridSize);
	newWorldButton.setLocation(0, row * gridSize);
	row++;

	//Save Listbox
	saveFrame.add(&worldSaves);
	worldSaves.addSelectionListener(new SaveSelect());
	worldSaves.setLocation(0, row * gridSize);
	worldSaves.setSize(frameWidth * gridSize - padding, 4 * gridSize);
	row += 4;

	// Save Button
	saveButton.setText("Save Game");
	saveButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldMutex.lock();
		world.Serialize(program.selectedSave);
		program.worldMutex.unlock();
	}));
	saveFrame.add(&saveButton);
	saveButton.setSize(gridSize * frameWidth, gridSize);
	saveButton.setLocation(0, row * gridSize);
	row++;

	// Load Button
	loadButton.setText("Load Game");
	loadButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldMutex.lock();
		world.Deserialize(program.selectedSave);
		program.worldMutex.unlock();
		while (!creator->guiStack.empty())
		{
			creator->guiStack.pop();
		}
	}));
	saveFrame.add(&loadButton);
	loadButton.setSize(gridSize * frameWidth, gridSize);
	loadButton.setLocation(0, row * gridSize);
	row++;

	// Back button exists on every frame 
	// such that those that aren't good with keyboard and mouse have easy navigation
	saveBackButton.setText("Back");
	saveBackButton.addButtonListener(new SimpleButtonListener([&] {
		if (!creator->guiStack.empty()) {
			creator->guiStack.pop();
		}
		else {
			creator->guiStack.push(&creator->mainFrame);
		}
	}));
	saveFrame.add(&saveBackButton);
	saveBackButton.setSize(gridSize * frameWidth, gridSize);
	saveBackButton.setLocation(0, row * gridSize);
	row++;

	// Save Frame
	mGui->add(&saveFrame);
	saveFrame.setText("Save Menu");
	saveFrame.setSize(frameWidth * gridSize, row * gridSize + frameHeadPadding);
	saveFrame.setLocation((int)(program.windowWidth / 2) - (saveFrame.getSize().getWidth() / 2), (int)(program.windowHeight / 2) - (saveFrame.getSize().getHeight() / 2));
	saveFrame.setVisibility(true);
}

void WidgetCreator::AddNewWorldFrame()
{
	int row = 0;
	worldNameTitle.setText("World Name:");
	newWorldFrame.add(&worldNameTitle);
	worldNameTitle.setSize(gridSize * frameWidth / 2, gridSize);
	worldNameTitle.setLocation(0.2 * gridSize * frameWidth, (float(row) + 0.25) * gridSize);
	newWorldFrame.add(&worldName);
	worldName.setFocusable(true);
	worldName.setSize(gridSize * frameWidth / 2, gridSize);
	worldName.setLocation(gridSize * frameWidth / 2, row * gridSize);
	row++;

	// Back button exists on every frame 
	// such that those that aren't good with keyboard and mouse have easy navigation
	std::hash<std::string> hasher;
	newWorldGenerate.setText("Generate");
	newWorldGenerate.addButtonListener(new SimpleButtonListener([&] {
		std::string worldName = creator->worldName.getText();
		if (worldName != "")
		{
			program.worldMutex.lock();
			world.clear();

			world = WorldSave(hasher(worldName));
			world.Serialize(worldName);
			program.redrawGround = true;
			program.worldMutex.unlock();
			while (!creator->guiStack.empty())
			{
				creator->guiStack.pop();
			}
		}
		}));
	newWorldFrame.add(&newWorldGenerate);
	newWorldGenerate.setSize(gridSize * frameWidth, gridSize);
	newWorldGenerate.setLocation(0, row * gridSize);
	row++;
	
	// Back button exists on every frame 
	// such that those that aren't good with keyboard and mouse have easy navigation
	newWorldBackButton.setText("Back");
	newWorldBackButton.addButtonListener(new SimpleButtonListener([&] {
		if (!creator->guiStack.empty()) {
			creator->guiStack.pop();
		}
		else {
			creator->guiStack.push(&creator->mainFrame);
		}
		}));
	newWorldFrame.add(&newWorldBackButton);
	newWorldBackButton.setSize(gridSize * frameWidth, gridSize);
	newWorldBackButton.setLocation(0, row * gridSize);
	row++;

	mGui->add(&newWorldFrame);
	newWorldFrame.setText("New World Menu");
	newWorldFrame.setSize(frameWidth * gridSize, row * gridSize + frameHeadPadding);
	newWorldFrame.setLocation((int)(program.windowWidth / 2) - (saveFrame.getSize().getWidth() / 2), (int)(program.windowHeight / 2) - (saveFrame.getSize().getHeight() / 2));
	newWorldFrame.setVisibility(true);
}

void WidgetCreator::SetGuiVisibility()
{
	buttonsEnabled = false;
	mainFrame.setVisibility(false);
	saveFrame.setVisibility(false);
	keyFrame.setVisibility(false);
	newWorldFrame.setVisibility(false);
	if (!guiStack.empty())
	{
		agui::Frame* visibleFrame = guiStack.top();
		visibleFrame->setVisibility(true);
	}
	buttonsEnabled = true;
}
void WidgetCreator::LoadDefaultKeyMapping()
{
	sf::Event::KeyEvent keyPress;
	keyPress = { sf::Keyboard::W, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Move North" });

	keyPress = { sf::Keyboard::A, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Move West" });

	keyPress = { sf::Keyboard::S, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Move South" });

	keyPress = { sf::Keyboard::D, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Move East" });

	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Rotate Clockwise" });

	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ true, /*system*/ false };
	actionMap.insert({ keyPress, "Rotate Anti-Clockwise" });

	keyPress = { sf::Keyboard::Q, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Empty Hand" });

	keyPress = { sf::Keyboard::Delete, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Remove Logic" });

	keyPress = { sf::Keyboard::X, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Set Robot Auto" });

	keyPress = { sf::Keyboard::Z, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Swap hotbar" });

	keyPress = { sf::Keyboard::C, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Change ColorClass" });

	keyPress = { sf::Keyboard::LAlt, /*alt*/ true, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Show Detailed Info" });

	keyPress = { sf::Keyboard::C, /*alt*/ false, /*ctrl*/ true, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Copy" });

	keyPress = { sf::Keyboard::X, /*alt*/ false, /*ctrl*/ true, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Cut" });

	keyPress = { sf::Keyboard::V, /*alt*/ false, /*ctrl*/ true, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Paste" });

	keyPress = { sf::Keyboard::F3, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Show Debug Info" });

	for (int i = 0; i < 20; i++)
	{
		keyPress = { sf::Keyboard::Key(int(sf::Keyboard::Num0) + MyMod(i + 1,10)), /*alt*/ false, /*ctrl*/ false, /*shift*/ i > 9, /*system*/ false };
		actionMap.insert({ keyPress, "Hotbar " + std::to_string(i + 1) });
	}
}
void WidgetCreator::CreateActionList()
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
		if(!world.moving)
			world.MovePlatform(program.mouseHovering,north);
			program.cameraPos.y -= int(GC::cameraSpeed * program.zoom);
		}
	} });

	//A
	userActionOrder.push_back("Move West");
	userActions.insert({ "Move West",[&] {
		if (program.selectedRobot)
		{
			if (creator->heldTick["Move West"] == world.tick)
				program.rotateBot = -1;
		}
		else
		{
			if (!world.moving)
			world.MovePlatform(program.mouseHovering,west);
			program.cameraPos.x -= int(GC::cameraSpeed * program.zoom);
		}
	} });

	//S
	userActionOrder.push_back("Move South");
	userActions.insert({ "Move South",[&] {
		if (program.selectedRobot)
		{
			if (creator->heldTick["Move South"] == world.tick)
				program.rotateBot = 2;
		}
		else
		{
			if (!world.moving)
			world.MovePlatform(program.mouseHovering,south);
			program.cameraPos.y += int(GC::cameraSpeed * program.zoom);
		}
	} });

	//D
	userActionOrder.push_back("Move East");
	userActions.insert({ "Move East",[&] {
		if (program.selectedRobot)
		{
			if (creator->heldTick["Move East"] == world.tick)
				program.rotateBot = 1;
		}
		else
		{
			if (!world.moving)
			world.MovePlatform(program.mouseHovering,east);
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
				world.updateQueueC.insert({program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded(), 1});
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
				world.updateQueueC.insert({ program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded(), 1 });
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
		program.hotbarIndex = -1;
		program.paste = false;
		program.copy = false;
		program.cut = false;
	} });

	//Delete
	userActionOrder.push_back("Remove Logic");
	userActions.insert({ "Remove Logic",[&] {
		if (program.selectedLogicTile)
		{
			world.logictiles.erase(program.selectedLogicTile->pos.CoordToEncoded());
			for (int i = 0; i < 4; i++)
			{
				world.updateQueueC.insert({ program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded(),1 });
			}
			program.selectedLogicTile = nullptr;
		}
	} });

	//X
	userActionOrder.push_back("Set Robot Auto");
	userActions.insert({ "Set Robot Auto",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->stopped = false;
			if (LogicTile* logic = world.GetLogicTile(program.selectedRobot->pos))
			{
				logic->DoRobotLogic(program.selectedRobot->pos);
			}
			program.selectedRobot = nullptr;
		}
	} });

	//Z
	userActionOrder.push_back("Swap hotbar");
	userActions.insert({ "Swap hotbar",[&] {
		for (int i = 0; i < 10; i++)
		{
			ParentTile* temp = program.hotbar[i];
			program.hotbar[i] = program.hotbar[i + 10];
			program.hotbar[i + 10] = temp;
		}
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
		program.hotbarIndex = -1;
		program.copy = true;
	} });
	
	//Ctrl + X
	userActionOrder.push_back("Cut");
	userActions.insert({ "Cut",[&] {
		program.hotbarIndex = -1;
		program.cut = true;
	} });
	
	//Ctrl + V
	userActionOrder.push_back("Paste");
	userActions.insert({ "Paste",[&] {
		program.paste = true;
	} });

	//F3
	userActionOrder.push_back("Show Debug Info");
	userActions.insert({ "Show Debug Info",[&] {
		program.showDebugInfo = !program.showDebugInfo;
	} });
	
	for (int i = 0; i < 20; i++)
	{
		userActionOrder.push_back("Hotbar " + std::to_string(i + 1));
		userActions.insert({ "Hotbar "+std::to_string(i+1),[i] {program.hotbarIndex = i; } });
		
	}

	// Start Selection
	userActions.insert({ "Start Selection", [&]{
		if (program.cut || program.copy)
		{
			program.startedSelection = true;
			program.startSelection = program.mouseHovering;
		}
		if (program.paste)
		{
			PasteSelection();
		}
	} });

	// Place Element
	userActions.insert({ "Place Element", [&] {
		if (program.hoveringHotbar != -1)
		{
			program.hotbarIndex = program.hoveringHotbar;
		}
		else
		{
			if (GroundTile* withinMap = world.GetGroundTile(program.mouseHovering))
			{
				if (program.hotbarIndex == -1)
				{
					if (program.selectedRobot)
						program.selectedRobot->stopped = true;
					program.selectedRobot = nullptr;
				}
				else if (program.hotbar[program.hotbarIndex])
				{
					if (LogicTile* hotbarLogic = dynamic_cast<LogicTile*> (program.hotbar[program.hotbarIndex]))
					{
						if (LogicTile* existingLogic = world.GetLogicTile(program.mouseHovering))
						{
							if (existingLogic->logictype == hotbarLogic->logictype)
							{
								existingLogic->quantity++;
								hotbarLogic->quantity--;
							}
							if (hotbarLogic->quantity == 0)
							{
								delete hotbarLogic;
								program.hotbar[program.hotbarIndex] = nullptr;
							}
						}
						else
						{
							LogicTile* logicPlace = hotbarLogic->Copy();
							logicPlace->signal = 0;
							logicPlace->pos = program.mouseHovering;
							if (Robot* robot = world.GetRobot(program.mouseHovering))
							{
								logicPlace->DoRobotLogic(robot->pos);
							}
							world.logictiles.insert({ logicPlace->pos.CoordToEncoded(), logicPlace });
							world.updateQueueD.insert(logicPlace->pos.CoordToEncoded());
							world.updateQueueC.insert({ logicPlace->pos.CoordToEncoded(),1 });										// Queue update for placed element
							for (int i = 0; i < 4; i++)
							{
								world.updateQueueC.insert({ logicPlace->pos.FacingPosition(Facing(i)).CoordToEncoded(),1 });
							}
							program.selectedLogicTile = logicPlace;
							hotbarLogic->quantity--;
							if (hotbarLogic->quantity == 0)
							{
								delete hotbarLogic;
								program.hotbar[program.hotbarIndex] = nullptr;
							}
						}
					}
					else if (Robot* hotbarRobot = dynamic_cast<Robot*> (program.hotbar[program.hotbarIndex]))
					{
						if (Robot* robot = world.GetRobot(program.mouseHovering))
						{
							// Don't do anything if a robot is already there
						}
						else
						{
							Robot* newRobot = hotbarRobot->Copy();
							newRobot->pos = program.mouseHovering;
							newRobot->stopped = true;
							world.robots.insert({ newRobot->pos.CoordToEncoded(),*newRobot });
							delete hotbarRobot;
							program.hotbar[program.hotbarIndex] = nullptr;
						}
					}
					else if (ItemTile* hotbarItem = dynamic_cast<ItemTile*> (program.hotbar[program.hotbarIndex]))
					{
						if (world.ChangeItem(program.mouseHovering, hotbarItem->itemTile, 1))
						{
							CraftingClass::TryCrafting(hotbarItem->itemTile, program.mouseHovering);
							hotbarItem->quantity--;
							if (hotbarItem->quantity == 0)
							{
								delete hotbarItem;
								program.hotbar[program.hotbarIndex] = nullptr;
							}
						}
					}
				}
			}
		}
	} });

	// Remove Element
	userActions.insert({ "Remove Element", [&] {
		if (ItemTile* item = world.GetItemTile(program.mouseHovering))
		{
			// If you can take an item off then do so
			if (world.ChangeItem(program.mouseHovering, item->itemTile, -1))
			{
				// Look through the hotbar if there's already a stack there
				for (int i = 0; i < program.hotbar.size(); i++)
				{
					if (ItemTile* hotbarItem = dynamic_cast<ItemTile*> (program.hotbar[i]))
					{
						if (item->itemTile == hotbarItem->itemTile)
						{
							if (hotbarItem->quantity != UINT8_MAX)
							{
								hotbarItem->quantity++;
								return;
							}
						}
					}
				}
				for (int i = 0; i < program.hotbar.size(); i++)
				{
					if (program.hotbar[i] == nullptr)
					{
						ItemTile* newItem = new ItemTile();
						newItem->itemTile = item->itemTile;
						newItem->quantity = 1;
						program.hotbar[i] = newItem;
						return;
					}
				}
				// If not go put it in an empty slot
				if (program.hotbar.size() < program.hotbarSize)
				{
					ItemTile* newItem = new ItemTile();
					newItem->itemTile = item->itemTile;
					newItem->quantity = 1;
					program.hotbar.emplace_back(newItem);
				}
				else
				{
					world.ChangeItem(program.mouseHovering, item->itemTile, 1);
				}
			}
			return;
		}
		if (LogicTile* logic = world.GetLogicTile(program.mouseHovering))
		{
			// Look through the hotbar if there's already a stack there
			for (int i = 0; i < program.hotbar.size(); i++)
			{
				if (LogicTile* hotbarLogic = dynamic_cast<LogicTile*> (program.hotbar[i]))
				{
					if (hotbarLogic->logictype == logic->logictype)
					{
						if (hotbarLogic->quantity != UINT8_MAX)
						{
							hotbarLogic->quantity++;
							world.ChangeLogic(program.mouseHovering, -1);
							return;
						}
					}
				}
			}
			// If not go put it in an empty slot
			for (int i = 0; i < program.hotbar.size(); i++)
			{
				if (program.hotbar[i] == nullptr)
				{
					LogicTile* hotbarLogic = logic->Copy();
					program.hotbar[i] = hotbarLogic;
					world.ChangeLogic(program.mouseHovering, -1);
					return;
				}
			}
			// If not go put it in an empty slot
			if (program.hotbar.size() < program.hotbarSize)
			{
				LogicTile* hotbarLogic = logic->Copy();
				program.hotbar.emplace_back(hotbarLogic);
				world.ChangeLogic(program.mouseHovering, -1);
			}
			return;
		}
		if (Robot* robot = world.GetRobot(program.mouseHovering))
		{
			for (int i = 0; i < program.hotbar.size(); i++)
			{
				if (program.hotbar[i] == nullptr)
				{
					Robot* hotbarRobot = robot->Copy();
					program.hotbar[i] = hotbarRobot;
					world.ChangeRobot(program.mouseHovering, -1);
					return;
				}
			}
			if (program.hotbar.size() < program.hotbarSize)
			{
				Robot* hotbarRobot = robot->Copy();
				program.hotbar.emplace_back(hotbarRobot);
				world.ChangeRobot(program.mouseHovering, -1);
			}
			return;
		}
	} });

	// Every tick
	for (std::string action : userActionOrder)
	{
		actionFrequency.insert({ action, 3600 });
	}
	actionFrequency["Move North"] = 1;
	actionFrequency["Move East"] = 1;
	actionFrequency["Move South"] = 1;
	actionFrequency["Move West"] = 1;
	actionFrequency["Remove Element"] = 8;
	actionFrequency["Place Element"] = 10;
	actionFrequency["Start Selection"] = 1;
}

void WidgetCreator::AddKeyMapFrame()
{
	const int scal = 2;
	// Key Frame
	mGui->add(&keyFrame);
	keyFrame.setText("Controls");
	keyFrame.setSize(gridSize * 4 * scal, gridSize * 6 + frameHeadPadding);
	keyFrame.setLocation((int)(program.windowWidth / 2) - int(keyFrame.getSize().getWidth()/2), (int)(program.windowHeight / 2) - int(keyFrame.getSize().getHeight() / 2));
	keyFrame.setVisibility(false);

	keyFrame.add(&controlPane);
	controlPane.setWheelScrollRate(10);
	controlPane.setVKeyScrollRate(20);
	controlPane.setSize(gridSize * 4 * scal - padding, gridSize * 5);
	controlPane.setLocation(0, 0);

	// Back button exists on every frame 
	// such that those that aren't good with keyboard and mouse have easy navigation
	controlBackButton.setSize(gridSize * 4 * scal, gridSize);
	controlBackButton.setText("Back");
	controlBackButton.addButtonListener(new SimpleButtonListener([&] {
		if (!creator->guiStack.empty()) {
			creator->guiStack.pop();
		}
		else {
			creator->guiStack.push(&creator->mainFrame);
		}
		}));
	keyFrame.add(&controlBackButton);
	controlBackButton.setLocation(0, gridSize * 5);

	int buttons = 0;
	for (std::string actionName : userActionOrder)
	{
		agui::Label* newTitle = new agui::Label();
		agui::Button* newButton = new agui::Button();

		newTitle->setText(actionName + " ");
		for (std::pair<sf::Event::KeyEvent, std::string> keyAction : actionMap)
		{
			if (keyAction.second == actionName)
			{
				newButton->setText(KeyNames::toString(keyAction.first));
			}
		}
		newButton->addButtonListener(new SimpleButtonListener([actionName, buttons] {
			creator->remapButtons[buttons]->setText("Waiting");
			creator->actionBindWaiting = actionName;
			creator->actionBindButtonIndex = buttons;
		}));

		newTitle->setSize(1.5 * gridSize * scal, gridSize / 2);
		newButton->setSize(1.5 * gridSize * scal, gridSize / 2);

		newTitle->setLocation(0 * gridSize * scal, buttons * gridSize / 2);
		newButton->setLocation(2 * gridSize * scal, buttons * gridSize / 2);

		keyActions.emplace_back(newTitle);
		remapButtons.emplace_back(newButton);

		controlPane.add(keyActions.back());
		controlPane.add(remapButtons.back());

		buttons++;
	}
}
void WidgetCreator::UserInput(sf::Event input)
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
		else if (std::string* action = actionMap.GetValue(input.key))
		{
			if (!program.gamePaused)
			{
				if (int* timeout = heldTick.GetValue(*action))
				{
					if (*timeout == 0)
					{
						heldTick[*action] = world.tick;
					}
				}
				else
				{
					heldTick[*action] = world.tick;
				}
			}
		}
	} else if (input.type == sf::Event::KeyReleased)
	{
		if (actionBindWaiting != "")
		{
			MapNewButton(input.key);
		}
		if (std::string* action = actionMap.GetValue(input.key))
		{
			heldTick[*action] = 0;
		}
	}
	else if (input.type == sf::Event::MouseMoved)
	{
		sf::Vector2i tempPos = sf::Mouse::getPosition(*window);
		//sf::Vector2f scaledPos = window->mapPixelToCoords(tempPos, program.worldView);
		program.mousePos = Pos{ int(tempPos.x) - int(program.halfWindowWidth),int(tempPos.y) - int(program.halfWindowHeight) };
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
				program.scale -= input.mouseWheelScroll.delta;
				if (program.scale < 0.0f)
					program.scale = 0.0f;
				if (program.scale > 75.0f)
					program.scale = 75.0f;
				program.zoom = 0.5f + program.scale / 10.0f;
			}
		}
	}
}
void WidgetCreator::MapNewButton(sf::Event::KeyEvent newButton)
{
	std::string oldAction = actionMap[newButton];
	sf::Event::KeyEvent* oldButtonRemove = new sf::Event::KeyEvent;
	for (std::pair<sf::Event::KeyEvent,std::string> oldButton : actionMap)
	{
		if (oldButton.second == oldAction)
			oldButtonRemove = &oldButton.first;
	}
	if (oldButtonRemove)
		actionMap.erase(*oldButtonRemove);

	// If there was a button for the old key to function 
	// set its name to None to indicate that it is unbound now.
	for (uint16_t i = 0; i < remapButtons.size(); i++)
	{
		if (remapButtons[i]->getText() == KeyNames::toString(newButton))
		{
			remapButtons[i]->setText("None");
		}
	}

	// Set new buttons name to the string representation of the key event
	if (!actionBindButtonIndex)
		OutputDebugStringA("Error finding button that control mapping refers to.\r\n");
	remapButtons[actionBindButtonIndex]->setText(KeyNames::toString(newButton));
	actionMap[newButton] = actionBindWaiting;

	// Clean up
	actionBindWaiting = "";
	actionBindButtonIndex = -1;
}

void WidgetCreator::MouseMoved()
{
	if (!program.gamePaused)
	{
		program.prevMouseHovering = program.mouseHovering;
		program.mouseHovering = ((program.mousePos * program.zoom) + program.cameraPos) / float(GC::tileSize);
		if (Robot * robot = world.GetRobot(program.mouseHovering.CoordToEncoded()))
		{
			if (robot != program.selectedRobot)
			{
				if (program.selectedRobot)
					if (program.selectedRobot != robot)
						program.selectedRobot->stopped = true;
				program.selectedRobot = robot;
				program.selectedRobot->stopped = false;
				if (LogicTile* logic = world.GetLogicTile(program.selectedRobot->pos))
				{
					logic->DoRobotLogic(program.selectedRobot->pos);
				}
			}
		}
		else
		{
			bool foundLogic = false;
			program.hoveringHotbar = -1;
			program.selectedHotbar = nullptr;
			for (uint8_t i = 0; i < program.hotbarSlots.size(); ++i)
			{
				sf::RectangleShape rect = program.hotbarSlots[i];
				sf::FloatRect rectBox(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
				if (rectBox.contains(sf::Vector2f(float(program.mousePos.x), float(program.mousePos.y))))
				{
					program.selectedHotbar = program.hotbar[i];
					program.hoveringHotbar = i;
					foundLogic = true;
				}
			}
			if (!foundLogic)
			{
				if (LogicTile * logic = world.GetLogicTile(program.mouseHovering.CoordToEncoded()))
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

void WidgetCreator::LeftMousePressed()
{
	
}

void WidgetCreator::RightMousePressed()
{
	
}

void WidgetCreator::FinishedSelection(Pos start, Pos end)
{
	// Empty the old list when creating a new one
	program.copyList.clear();
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
				ParentTile* newElement = dynamic_cast<ParentTile*> (logic);
				program.copyList.emplace_back(newElement);
				if (program.cut)
					world.logictiles.erase(Pos{ i,j }.CoordToEncoded());
			}
			if (ItemTile* item = world.GetItemTile(Pos{ i,j }))
			{
				ItemTileWPOS* copyItem = new ItemTileWPOS();
				copyItem->pos = Pos{ i,j };
				copyItem->itemTile = item->itemTile;
				copyItem->quantity = item->quantity;
				ParentTile* newElement = dynamic_cast<ParentTile*> (copyItem);
				program.copyList.emplace_back(newElement);
				if (program.cut)
					world.items.erase(Pos{ i,j }.CoordToEncoded());
			}
		}
	}
	program.cut = false;
	program.copy = false;
	program.startedSelection = false;
	program.paste = true;
}

void WidgetCreator::PasteSelection()
{
	for (ParentTile* element : program.copyList)
	{
		if (LogicTile* newElement = dynamic_cast<LogicTile*> (element))
		{
			LogicTile* copyElement = newElement->Copy();
			copyElement->pos = copyElement->pos + program.mouseHovering - program.originSelection;
			world.logictiles.insert({ copyElement->pos.CoordToEncoded(), copyElement});
		}
		else if(ItemTileWPOS* newElement = dynamic_cast<ItemTileWPOS*> (element))
		{
			ItemTile copy;
			copy.itemTile = newElement->itemTile;
			copy.quantity = newElement->quantity;
			world.items.insert({ (newElement->pos + program.mouseHovering - program.originSelection).CoordToEncoded(), copy });
		}
	}
	program.paste = false;
}

void WidgetCreator::SaveProgramSettings()
{
	world.Serialize(world.name);
	std::ofstream myfile;
	myfile.open("saves/config.txt", std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		myfile << world.name + "\r\n";
		for (std::pair<sf::Event::KeyEvent, std::string> action : actionMap)
		{
			myfile << action.second + ":" + KeyNames::toString(action.first) + "\r\n";
		}
	}
	myfile.close();
}

// Returns true when a key mapping config has been found
bool WidgetCreator::LoadProgramSettings()
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
					actionMap.insert({ *valid, splitLine[0] });
				}
			}
		}
		i++;
	}
	myfile.close();
	world.Deserialize(world.name);
	return true;
}

void WidgetCreator::PerformActions()
{
	// There can only be one input for robot control...
	std::vector<std::string> mask = { "Move North","Move East","Move South","Move West" };
	int oldest[4] = { 0 };
	bool outerMask = false;
	for (std::pair<std::string, int> action : creator->heldTick)
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
				if ((world.tick - action.second) % actionFrequency[action.first] == 0)
				{
					if (std::function<void()>* func = creator->userActions.GetValue(action.first))
					{
						if (action.first == "Start Selection")
						{
							if(world.tick == action.second)
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
			if (std::function<void()>* func = creator->userActions.GetValue(mask[maxInd]))
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