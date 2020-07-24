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
#include "MyStrings.h"

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
	AddCraftingFrame();
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

	for (int i = 1; i < 21; i++)
	{
		keyPress = { sf::Keyboard::Key(int(sf::Keyboard::Num0) + MyMod(i + 1,10)), /*alt*/ false, /*ctrl*/ false, /*shift*/ i > 10, /*system*/ false };
		actionMap.insert({ keyPress, "Hotbar " + std::to_string(i) });
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

	//Delete
	userActionOrder.push_back("Remove Logic");
	userActions.insert({ "Remove Logic",[&] {
		if (program.selectedLogicTile)
		{
			world.logicTiles.erase(program.mouseHovering.CoordToEncoded());
			for (int i = 0; i < 4; i++)
			{
				world.updateNext.insert({ program.mouseHovering.FacingPosition(Facing(i)).CoordToEncoded(),1 });
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

	//F3
	userActionOrder.push_back("Show Debug Info");
	userActions.insert({ "Show Debug Info",[&] {
		program.showDebugInfo = !program.showDebugInfo;
	} });
	
	for (int i = 1; i < 21; i++)
	{
		userActionOrder.push_back("Hotbar " + std::to_string(i));
		userActions.insert({ "Hotbar " + std::to_string(i),[i] {program.hotbarIndex = {(uint8_t)MyMod(i-1,10),(uint8_t)(i < 11)}; } });
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
					if(kv != program.hotbar.end())
					{
						if (world.PlaceElement(program.mouseHovering, kv->second.itemTile))
						{
							CraftingClass::TryCrafting(kv->second.itemTile, program.mouseHovering);
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
		if(!elementFound)
			if (element = world.ChangeItem(program.mouseHovering, -1, 0))
				elementFound = true;
		if (!elementFound)
			if (element = world.ChangeLogic(program.mouseHovering, -1, 0))
				elementFound = true;
		if(element)
			ChangeInventory(element, 1);
	} });

	// Every tick
	actionFrequency["Move North"] = 1;
	actionFrequency["Move East"] = 1;
	actionFrequency["Move South"] = 1;
	actionFrequency["Move West"] = 1;
	actionFrequency["Remove Element"] = 8;
	actionFrequency["Place Element"] = 10;
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
		else if (std::string* action = actionMap.GetValue(input.key))
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
				if (program.scale > 35.0f)
					program.scale = 35.0f;
				program.zoom = 0.5f + program.scale / 10.0f;
				program.RecalculateMousePos();
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

int WidgetCreator::ChangeInventory(uint16_t item, int quantity)
{
	if (quantity > 0)
	{
		bool addedToInv = false;
		for (auto kv : program.hotbar)
		{
			if (kv.second.itemTile == item)
			{
				int subQuantity = quantity;
				if (kv.second.quantity + quantity > 255)
					subQuantity = 255 - kv.second.quantity;
				auto modify = program.hotbar.find(kv.first);
				modify->second.quantity += subQuantity;
				quantity -= subQuantity;
				if (quantity == 0)
				{
					addedToInv = true;
					break;
				}
			}
		}
		if (!addedToInv)
		{
			for (int j = 1; j >= 0; j--)
			{
				for (int i = 0; i < 10; i++)
				{
					auto kv = program.hotbar.find(SmallPos{ (uint8_t)i,(uint8_t)j });
					if (kv == program.hotbar.end())
					{
						ItemTile newItem = ItemTile(item);
						int subQuantity = quantity;
						if (subQuantity > 255)
							subQuantity = 255;
						newItem.quantity = subQuantity;
						quantity -= subQuantity;
						program.hotbar.insert({ SmallPos{(uint8_t)i,(uint8_t)j},newItem });
						if (quantity == 0)
							break;
					}
				}
			}
		}
	}
	else if(quantity < 0)
	{
		std::vector<SmallPos> removeList;
		for (auto kv : program.hotbar)
		{
			if (kv.second.itemTile == item)
			{
				auto element = program.hotbar.find(kv.first);
				if (element != program.hotbar.end())
				{
					int subQuantity = quantity;
					if ((int)element->second.quantity + subQuantity < 0)
						subQuantity = -(int)element->second.quantity;
					element->second.quantity += subQuantity;
					if (element->second.quantity == 0)
						removeList.emplace_back(element->first);
					quantity -= subQuantity;
					if (quantity == 0)
						break;
				}
			}
		}
		for (auto k : removeList)
		{
			program.hotbar.erase(k);
		}
	}
	return quantity;
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
		else
		{
			bool foundLogic = false;
			program.hoveringHotbar = { 255,255 };
			program.selectedHotbar = nullptr;
			for (auto element : program.hotbarSlots)
			{
				sf::RectangleShape rect = element.second;
				sf::FloatRect rectBox(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
				if (rectBox.contains(sf::Vector2f(float(program.mousePos.x), float(program.mousePos.y))))
				{
					auto kv = program.hotbar.find(element.first);
					if (kv != program.hotbar.end())
					{
						program.selectedHotbar = &kv->second;
						foundLogic = true;
					}
					program.hoveringHotbar = element.first;
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
				program.copyMap.insert({(Pos{ i,j } -start).CoordToEncoded(), copyLogic});
				if (program.cut)
				{
					uint16_t element = 0;
					if (program.cut)
						element = world.ChangeLogic(Pos{ i,j }, -1, 0);
					if (element)
					{
						ChangeInventory(element, 1);
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

void WidgetCreator::PasteSelection()
{
	for (auto kv : program.copyMap)
	{
		uint16_t element = world.ChangeLogic(Pos::EncodedToCoord(kv.first) + program.mouseHovering, 1, kv.second.logicType);
		if(element > 1)
			ChangeInventory(element, -1);
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
	//world.Deserialize(world.name);
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
				// If an action has a frequency it'll be repeated at that frequency while the action is held
				if (int* frequency = actionFrequency.GetValue(action.first))
				{
					if ((world.tick - action.second) % actionFrequency[action.first] == 0)
					{
						if (std::function<void()>* func = creator->userActions.GetValue(action.first))
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
						if (std::function<void()>* func = creator->userActions.GetValue(action.first))
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

class SimpleTextFieldListener : public agui::ActionListener
{
private:
	std::function<void()> enterAction;
public:
	SimpleTextFieldListener(std::function<void()> enterAction) {
		this->enterAction = enterAction;
	};
	void actionPerformed(const agui::ActionEvent& evt)
	{
		enterAction();
	}
	~SimpleTextFieldListener(void) {};
};

void WidgetCreator::AddCraftingFrame()
{
	mGui->add(&craftingFrame);
	craftingFrame.setSize(agui::Dimension(mainFrameWidth, 110));
	searchTitle.setText("Find: ");
	craftingFrame.add(&searchTitle);

	searchBar.addActionListener(new SimpleTextFieldListener([&] {
			FindRecipes(creator->searchBar.getText(),0);
		}));
	searchBar.setSize(agui::Dimension(150, 20));
	searchBar.setLocation(agui::Point(50,0));
	craftingFrame.add(&searchBar);

	recipeResults.setLocation(agui::Point(0, 20));
	craftingFrame.add(&recipeResults);

	craftingNextButton.setText("Next");
	craftingNextButton.addButtonListener(new SimpleButtonListener([&] {
		if (program.craftingViewIndex < program.foundRecipeList.size() - 1)
			program.craftingViewIndex++;
			program.craftingViewUpdate = true;
		}));
	craftingFrame.add(&craftingNextButton);
	craftingNextButton.setSize(150, 50);
	craftingNextButton.setLocation(150, 30);

	craftingPrevButton.setText("Prev");
	craftingPrevButton.addButtonListener(new SimpleButtonListener([&] {
		if (program.craftingViewIndex > 0)
			program.craftingViewIndex--;
			program.craftingViewUpdate = true;
		}));
	craftingFrame.add(&craftingPrevButton);
	craftingPrevButton.setSize(150, 50);
	craftingPrevButton.setLocation(0, 30);

}

void WidgetCreator::FindRecipes(std::string name, int state)
{
	program.foundRecipeList.clear();
	name = lowercase(swapChar(name, ' ', '_'));
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
	program.craftingViewIndex = 0;
	program.craftingViewUpdate = true;
}