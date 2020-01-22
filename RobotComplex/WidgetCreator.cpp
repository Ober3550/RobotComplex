#include "WidgetCreator.h"
#include "ProgramData.h"
#include "WorldSave.h"
#include <Windows.h>
#include <vector>
#include <string>
#include "FindInVector.h"
#include "KeyNames.h"
#include "GetFileNamesInFolder.h"

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
	userActionOrder = {
		"Menu step back",
		"Move Forward",
		"Turn Left",
		"Turn Around",
		"Turn Right",
		"Rotate Placement",
		"Copy Hovered",
		"Remove Logic",
		"Set Robot Auto",
		"Change ColorClass",
		"Hotbar 1",
		"Hotbar 2",
		"Hotbar 3",
		"Hotbar 4",
		"Hotbar 5",
		"Hotbar 6",
		"Hotbar 7",
		"Hotbar 8",
		"Hotbar 9"
	};
	sf::Event::KeyEvent keyPress;
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
	userActions.insert({ "Move Forward",[&] {
		if (program.selectedRobot)
			{
				program.selectedRobot->Move();
			}
			else
			{
				program.cameraPos.y -= GC::cameraSpeed;
			}
	} });
	keyPress = { sf::Keyboard::W, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Move Forward" });

	//A
	userActions.insert({ "Turn Left",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->Rotate(-1);
		}
		else
		{
			program.cameraPos.x -= GC::cameraSpeed;
		}
	} });
	keyPress = { sf::Keyboard::A, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Turn Left" });

	//S
	userActions.insert({ "Turn Around",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->Rotate(2);
		}
		else
		{
			program.cameraPos.y += GC::cameraSpeed;
		}
	} });
	keyPress = { sf::Keyboard::S, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Turn Around" });

	//D
	userActions.insert({ "Turn Right",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->Rotate(1);
		}
		else
		{
			program.cameraPos.x += GC::cameraSpeed;
		}
	} });
	keyPress = { sf::Keyboard::D, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Turn Right" });

	//R
	userActions.insert({ "Rotate Placement",[&] {
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
	} });
	keyPress = { sf::Keyboard::R, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Rotate Placement" });

	//Q
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
	userActions.insert({ "Remove Logic",[&] {
		if (program.selectedLogicTile)
		{
			world.logictiles.erase(program.selectedLogicTile->pos.CoordToEncoded());
			for (int i = 0; i < 4; i++)
			{
				world.updateQueueC.insert(program.selectedLogicTile->pos.FacingPosition(Facing(i)).CoordToEncoded());
			}
			program.selectedLogicTile = nullptr;
		}
	} });
	keyPress = { sf::Keyboard::Delete, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Remove Logic" });

	//X
	userActions.insert({ "Set Robot Auto",[&] {
		if (program.selectedRobot)
		{
			program.selectedRobot->stopped = false;
			program.selectedRobot = nullptr;
		}
	} });
	keyPress = { sf::Keyboard::X, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Set Robot Auto" });

	//C
	userActions.insert({ "Change ColorClass",[&] {
		if (program.placeColor == 1)
			program.placeColor = 4;
		else
			program.placeColor = 1;
	} });
	keyPress = { sf::Keyboard::C, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Change ColorClass" });

	userActions.insert({ "Hotbar 1",[&] {
		program.hotbarIndex = 0;
	} });
	keyPress = { sf::Keyboard::Num1, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 1" });
	userActions.insert({ "Hotbar 2",[&] {
		program.hotbarIndex = 1;
	} });
	keyPress = { sf::Keyboard::Num2, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 2" });
	userActions.insert({ "Hotbar 3",[&] {
		program.hotbarIndex = 2;
	} });
	keyPress = { sf::Keyboard::Num3, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 3" });
	userActions.insert({ "Hotbar 4",[&] {
		program.hotbarIndex = 3;
	} });
	keyPress = { sf::Keyboard::Num4, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 4" });
	userActions.insert({ "Hotbar 5",[&] {
		program.hotbarIndex = 4;
	} });
	keyPress = { sf::Keyboard::Num5, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 5" });
	userActions.insert({ "Hotbar 6",[&] {
		program.hotbarIndex = 5;
	} });
	keyPress = { sf::Keyboard::Num6, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 6" });
	userActions.insert({ "Hotbar 7",[&] {
		program.hotbarIndex = 6;
	} });
	keyPress = { sf::Keyboard::Num7, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 7" });
	userActions.insert({ "Hotbar 8",[&] {
		program.hotbarIndex = 7;
	} });
	keyPress = { sf::Keyboard::Num8, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 8" });
	userActions.insert({ "Hotbar 9",[&] {
		program.hotbarIndex = 8;
	} });
	keyPress = { sf::Keyboard::Num9, /*alt*/ false, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
	actionMap.insert({ keyPress, "Hotbar 9" });
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
		program.mousePos = Pos{ tempPos.x - (program.windowWidth >> 1),tempPos.y - (program.windowHeight >> 1) };
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
			program.zoom += input.mouseWheelScroll.delta;
			// Clamp between 1 and 15
			if (program.zoom < 1)
				program.zoom = 1.0f;
			if (program.zoom > 15)
				program.zoom = 15.0f;
			// Change to range between 1/2 and 2
			program.scale = 0.5f + (program.zoom / 10.0f);
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
		program.mouseHovering = (program.mousePos + program.cameraPos) / int(float(GC::tileSize) * program.scale);
		if (Robot * robot = world.GetRobot(program.mouseHovering.CoordToEncoded()))
		{
			program.selectedRobot = robot;
			program.selectedRobot->stopped = false;
			program.cameraPos = program.mouseHovering << GC::tileShift;
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
		if (Robot * robot = world.robots.GetValue(program.mouseHovering.CoordToEncoded()))
		{
		}
		else
		{
			if(program.selectedRobot)
				program.selectedRobot->stopped = true;
			program.selectedRobot = nullptr;
		}
		if (GroundTile * withinMap = world.GetGroundTile(program.mouseHovering))
		{
			if (program.hotbarIndex < (int)program.hotbar.size())
			{
				if (program.hotbar[program.hotbarIndex])
				{
					LogicTile* hotbarElement = program.hotbar[program.hotbarIndex];
					LogicTile* logicPlace = hotbarElement->Copy();
					logicPlace->colorClass = program.placeColor;
					logicPlace->pos = program.mouseHovering;
					if (Robot* robot = world.GetRobot(program.mouseHovering))
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

void WidgetCreator::RightMousePressed()
{
	if (LogicTile * deleteLogic = world.GetLogicTile(program.mouseHovering.CoordToEncoded()))
	{
		world.logictiles.erase(program.mouseHovering.CoordToEncoded());
		if (Robot* robot = world.GetRobot(program.mouseHovering))
		{
			robot->stopped = false;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		world.updateQueueC.insert(program.mouseHovering.FacingPosition(Facing(i)).CoordToEncoded());
	}
	program.selectedLogicTile = nullptr;
}