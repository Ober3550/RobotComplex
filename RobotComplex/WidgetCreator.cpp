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
	window = windowRef;
	AddMainFrame();
	AddSaveFrame();
	SetDefaultKeyMapping();
	AddKeyMapFrame();
}
void WidgetCreator::AddMainFrame()
{
	mGui->add(&mainFrame);
	// Options mainFrame
	mainFrame.setSize(mainFrameWidth, mainFrameHeight);
	mainFrame.setLocation((int)(program.windowWidth / 2) - (mainFrameWidth / 2), (int)(program.windowHeight / 2) - (mainFrameHeight / 2));
	mainFrame.setText("Main Menu");
	// Play Button changes to load and save frame
	playButton.setSize(mainFrameWidth - 10, 50);
	playButton.setText("Play");
	playButton.addButtonListener(new SimpleButtonListener([&] {
		// Don't push duplicates to the stack
		if (!creator->guiStack.empty())
			if (creator->guiStack.top() != &creator->saveFrame)
				creator->guiStack.push(&saveFrame);
		creator->worldSaves.clearItems();
		std::vector<std::string> names = getFileNamesInFolder("saves");
		for (std::string name : names)
		{
			worldSaves.addItem(name);
		}
	}));
	mainFrame.add(&playButton);
	playButton.setLocation(0, 0);
	// 'Controls' Button changes key mapping
	controlsButton.setSize(mainFrameWidth - 10, 50);
	controlsButton.setText("Controls");
	controlsButton.addButtonListener(new SimpleButtonListener([&] {
		// Don't push duplicates to the stack
		if (!creator->guiStack.empty())
			if (creator->guiStack.top() != &creator->keyFrame)
				creator->guiStack.push(&keyFrame);
	}));
	mainFrame.add(&controlsButton);
	controlsButton.setLocation(0, 50);
	// Exit Button
	exitButton.setText("Exit Game");
	exitButton.addButtonListener(new SimpleButtonListener([&] {
		program.running = false;
	}));
	mainFrame.add(&exitButton);
	exitButton.setSize(mainFrameWidth - 10, 50);
	exitButton.setLocation(0, mainFrameHeight - 85);
	mainFrame.setVisibility(true);
	// Ensure that top of gui stack is the main frame
	guiStack.push(&mainFrame);
}
void WidgetCreator::AddSaveFrame()
{
	//Save Frame
	mGui->add(&saveFrame);
	saveFrame.setText("Save Menu");
	saveFrame.setSize(mainFrameWidth, mainFrameHeight);
	saveFrame.setLocation((int)(program.windowWidth / 2) - (mainFrameWidth / 2), (int)(program.windowHeight / 2) - (mainFrameHeight / 2));

	// Save Button
	saveButton.setText("Save Game");
	saveButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldMutex.lock();
		world.Serialize(program.selectedSave);
		program.worldMutex.unlock();
	}));
	saveFrame.add(&saveButton);
	saveButton.setSize(mainFrameWidth / 2, 50);
	saveButton.setLocation(0, mainFrameHeight - 85 - 50);

	// Load Button
	loadButton.setText("Load Game");
	loadButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldMutex.lock();
		world.Deserialize(program.selectedSave);
		program.worldMutex.unlock();
	}));
	saveFrame.add(&loadButton);
	loadButton.setSize(mainFrameWidth / 2, 50);
	loadButton.setLocation(mainFrameWidth / 2, mainFrameHeight - 85 - 50);

	//Save Listbox
	saveFrame.add(&worldSaves);
	worldSaves.addSelectionListener(new SaveSelect());
	worldSaves.setLocation(0, 0);
	worldSaves.setSize(mainFrameWidth, mainFrameHeight - 85 - 50);

	// Back button exists on every frame 
	// such that those that aren't good with keyboard and mouse have easy navigation
	backButton.setSize(mainFrameWidth - 10, 50);
	backButton.setText("Back");
	backButton.addButtonListener(new SimpleButtonListener([&] {
		if (!creator->guiStack.empty()) {
			creator->guiStack.pop();
		}
		else {
			creator->guiStack.push(&creator->mainFrame);
		}
	}));
	saveFrame.add(&backButton);
	backButton.setLocation(0, mainFrameHeight - 85);

	saveFrame.setVisibility(true);
}
void WidgetCreator::SetGuiVisibility()
{
	buttonsEnabled = false;
	mainFrame.setVisibility(false);
	saveFrame.setVisibility(false);
	keyFrame.setVisibility(false);
	if (!guiStack.empty())
	{
		agui::Frame* visibleFrame = guiStack.top();
		visibleFrame->setVisibility(true);
	}
	buttonsEnabled = true;
}
void WidgetCreator::SetDefaultKeyMapping()
{
	sf::Event::KeyEvent keyPress;
	userActionOrder.push_back( "Menu step back" );
	userActions.insert({ "Menu step back",[&] {
		if (!creator->guiStack.empty()) {
			creator->guiStack.pop();
		}
		else {
			creator->guiStack.push(&creator->mainFrame);
		}
		if (creator->guiStack.empty())
			program.gamePaused = false;
		else
			program.gamePaused = true;
	} });
	keyPress = { sf::Keyboard::Escape, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Menu step back" });

	//W
	userActionOrder.push_back("Move Forward");
	userActions.insert({ "Move Forward",[&] {
		if (program.selectedRobot)
			{
				program.selectedRobot->Move();
			}
			else
			{
				world.MovePlatform(program.mouseHovering,north);
				program.cameraPos.y -= int(GC::cameraSpeed * program.zoom);
			}
	} });
	keyPress = { sf::Keyboard::W, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Move Forward" });

	//A
	userActionOrder.push_back("Turn Left");
	userActions.insert({ "Turn Left",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->Rotate(-1);
		}
		else
		{
			world.MovePlatform(program.mouseHovering,west);
			program.cameraPos.x -= int(GC::cameraSpeed * program.zoom);
		}
	} });
	keyPress = { sf::Keyboard::A, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Turn Left" });

	//S
	userActionOrder.push_back("Turn Around");
	userActions.insert({ "Turn Around",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->Rotate(2);
		}
		else
		{
			world.MovePlatform(program.mouseHovering,south);
			program.cameraPos.y += int(GC::cameraSpeed * program.zoom);
		}
	} });
	keyPress = { sf::Keyboard::S, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Turn Around" });

	//D
	userActionOrder.push_back("Turn Right");
	userActions.insert({ "Turn Right",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->Rotate(1);
		}
		else
		{
			world.MovePlatform(program.mouseHovering,east);
			program.cameraPos.x += int(GC::cameraSpeed * program.zoom);
		}
	} });
	keyPress = { sf::Keyboard::D, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Turn Right" });

	//R
	userActionOrder.push_back("Rotate Clockwise");
	userActions.insert({ "Rotate Clockwise",[&] {
		program.placeRotation = Facing((int(program.placeRotation) + 1) & 3);
		if (program.selectedLogicTile)
		{
			program.selectedLogicTile->facing = program.placeRotation;
			for (int i = 0; i < 4; i++)
			{
				world.updateQueueC.insert({program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded(), 1});
			}
		}
	} });
	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Rotate Clockwise" });

	//R
	userActionOrder.push_back("Rotate Anti-Clockwise");
	userActions.insert({ "Rotate Anti-Clockwise",[&] {
		program.placeRotation = Facing((int(program.placeRotation) - 1) & 3);
		if (program.selectedLogicTile)
		{
			program.selectedLogicTile->facing = program.placeRotation;
			for (int i = 0; i < 4; i++)
			{
				world.updateQueueC.insert({ program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded(), 1 });
			}
		}
	} });
	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ true, /*system*/ false };
	actionMap.insert({ keyPress, "Rotate Anti-Clockwise" });

	//Q
	userActionOrder.push_back("Copy Hovered");
	userActions.insert({ "Copy Hovered",[&] {
		if (program.selectedLogicTile)
		{
			program.hotbar[0] = program.selectedLogicTile->Copy();
			program.placeRotation = program.selectedLogicTile->facing;
		}
		program.hotbarIndex = 0;
	} });
	keyPress = { sf::Keyboard::Q, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Copy Hovered" });

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
	keyPress = { sf::Keyboard::Delete, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Remove Logic" });

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
	keyPress = { sf::Keyboard::X, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Set Robot Auto" });

	//C
	userActionOrder.push_back("Change ColorClass");
	userActions.insert({ "Change ColorClass",[&] {
		if (program.placeColor == 1)
			program.placeColor = 4;
		else
			program.placeColor = 1;
	} });
	keyPress = { sf::Keyboard::C, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Change ColorClass" });

	for (int i = 0; i < 20; i++)
	{
		userActionOrder.push_back("Hotbar " + std::to_string(i + 1));
		userActions.insert({ "Hotbar "+std::to_string(i+1),[i] {program.hotbarIndex = i; } });
		keyPress = { sf::Keyboard::Key(int(sf::Keyboard::Num0) + MyMod(i+1,10)), /*alt*/ false, /*ctrl*/ false, /*shift*/ i>9, /*system*/ false };
		actionMap.insert({ keyPress, "Hotbar "+std::to_string(i+1) });
	}
}

void WidgetCreator::AddKeyMapFrame()
{
	// Key Frame
	mGui->add(&keyFrame);
	keyFrame.setText("Controls");
	keyFrame.setSize(mainFrameWidth, 600);
	keyFrame.setLocation((int)(program.windowWidth / 2) - (mainFrameWidth / 2), (int)(program.windowHeight / 2) - (600 / 2));
	keyFrame.setVisibility(false);

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
				newButton->setText(toString(keyAction.first));
			}
		}
		newButton->addButtonListener(new SimpleButtonListener([actionName, buttons] {
			creator->remapButtons[buttons]->setText("Waiting");
			creator->actionBindWaiting = actionName;
			creator->actionBindButtonIndex = buttons;
		}));

		newTitle->setSize(mainFrameWidth / 2, 22);
		newButton->setSize(mainFrameWidth / 2 - 15, 22);

		newTitle->setLocation(0, buttons * 25);
		newButton->setLocation(mainFrameWidth / 2, buttons * 25);

		keyActions.emplace_back(newTitle);
		remapButtons.emplace_back(newButton);

		keyFrame.add(keyActions.back());
		keyFrame.add(remapButtons.back());

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
			if (std::function<void()>* func = this->userActions.GetValue(*action))
			{
				(*func)();
			}
		}
	} else if (input.type == sf::Event::KeyReleased)
	{
		if (actionBindWaiting != "")
		{
			MapNewButton(input.key);
		}
	}
	else if (input.type == sf::Event::MouseMoved)
	{
		sf::Vector2i tempPos = sf::Mouse::getPosition(*window);
		//sf::Vector2f scaledPos = window->mapPixelToCoords(tempPos, program.worldView);
		program.mousePos = Pos{ int(tempPos.x) - int(program.halfWindowWidth),int(tempPos.y) - int(program.halfWindowHeight) };
		MouseMoved();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			LeftMousePressed();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			RightMousePressed();
	}
	else if (input.type == sf::Event::MouseButtonPressed)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			LeftMousePressed();
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			RightMousePressed();
	}
	else if (input.type == sf::Event::MouseButtonReleased)
	{

	}
	else if (input.type == sf::Event::MouseWheelScrolled)
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
		if (remapButtons[i]->getText() == toString(newButton))
		{
			remapButtons[i]->setText("None");
		}
	}

	// Set new buttons name to the string representation of the key event
	if (!actionBindButtonIndex)
		OutputDebugStringA("Error finding button that control mapping refers to.\r\n");
	remapButtons[actionBindButtonIndex]->setText(toString(newButton));
	actionMap[newButton] = actionBindWaiting;

	// Clean up
	actionBindWaiting = "";
	actionBindButtonIndex = -1;
}

void WidgetCreator::MouseMoved()
{
	if (!program.gamePaused)
	{
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
			program.hoveringHotbar = false;
			for (uint8_t i = 0; i < program.hotbarSlots.size(); ++i)
			{
				sf::RectangleShape rect = program.hotbarSlots[i];
				sf::FloatRect rectBox(rect.getPosition().x, rect.getPosition().y, rect.getSize().x, rect.getSize().y);
				if (rectBox.contains(sf::Vector2f(float(program.mousePos.x), float(program.mousePos.y))))
				{
					program.selectedLogicTile = program.hotbar[i];
					program.hoveringHotbar = true;
					if (program.selectedLogicTile)
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
	if (!program.gamePaused)
	{
		if (GroundTile * withinMap = world.GetGroundTile(program.mouseHovering))
		{
			if (program.hotbarIndex < (int)program.hotbar.size())
			{
				if (program.hotbar[program.hotbarIndex])
				{
					LogicTile* logicPlace = program.hotbar[program.hotbarIndex]->Copy();
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
				}
				else
				{
					program.selectedLogicTile = nullptr;
					if (Robot * robot = world.robots.GetValue(program.mouseHovering.CoordToEncoded()))
					{
					}
					else
					{
						if (program.selectedRobot)
							program.selectedRobot->stopped = true;
						program.selectedRobot = nullptr;
					}
				}

			}
		}
	}
}

void WidgetCreator::RightMousePressed()
{
	if (LogicTile * deleteLogic = world.GetLogicTile(program.mouseHovering.CoordToEncoded()))
	{
		world.logictiles.erase(program.mouseHovering.CoordToEncoded());
		if (Robot* robot = world.GetRobot(program.mouseHovering))
		{
			robot->stopped = false;
			if (LogicTile* logic = world.GetLogicTile(program.mouseHovering))
			{
				logic->DoRobotLogic(program.mouseHovering);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			world.updateQueueC.insert({ program.mouseHovering.FacingPosition(Facing(i)).CoordToEncoded(),1 });
		}
		program.selectedLogicTile = nullptr;
	}
}