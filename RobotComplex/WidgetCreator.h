#pragma once
#include <Agui/Agui.hpp>
#include <Agui/Backends/SFML2/SFML2.hpp>
#include <Agui/Widgets/Button/Button.hpp>
#include <Agui/Widgets/Button/ButtonListener.hpp>
#include <Agui/Widgets/CheckBox/CheckBox.hpp>
#include <Agui/Widgets/DropDown/DropDown.hpp>
#include <Agui/Widgets/TextField/TextField.hpp>
#include <Agui/Widgets/Frame/Frame.hpp>
#include <Agui/Widgets/RadioButton/RadioButton.hpp>
#include <Agui/Widgets/RadioButton/RadioButtonGroup.hpp>
#include <Agui/Widgets/Slider/Slider.hpp>
#include <Agui/Widgets/TextBox/ExtendedTextBox.hpp>
#include <Agui/Widgets/Tab/TabbedPane.hpp>
#include <Agui/Widgets/ListBox/ListBox.hpp>
#include <Agui/Widgets/ScrollPane/ScrollPane.hpp>
#include <Agui/Widgets/Label/Label.hpp>
#include <Agui/FlowLayout.hpp>
#include <stack>
#include "MyMap.h"
#include <functional>

// Create a hash function for key events to allow for key modifiers to be considered
template<>
struct std::hash<sf::Event::KeyEvent> {
	std::size_t operator()(const sf::Event::KeyEvent& k) const
	{
		//{ sf::Keyboard::Escape, /*alt*/ true, /*ctrl*/ false, /*shift*/ false, /*system*/ false };
		return (int)k.system << 7 & (int)k.shift << 6 & (int)k.control << 5 & (int)k.alt << 4 & (int)k.code;
	}
};

class WidgetCreator {
public:
	agui::Gui* mGui;
	int mainFrameWidth = 300;
	int mainFrameHeight = 250;
	bool buttonsEnabled = true;
	std::stack<agui::Frame*> guiStack;
	sf::RenderWindow* window;

	// Main Menu
	agui::Frame mainFrame;
	agui::Button playButton;
	agui::Button controlsButton;
	agui::Button exitButton;

	// Play Menu
	agui::Frame saveFrame;
	agui::ListBox worldSaves;
	agui::Button loadButton;
	agui::Button saveButton;
	agui::Button backButton;

	// Controls Menu
	agui::Frame keyFrame;
	std::vector<agui::Button*> remapButtons;
	std::vector<agui::Label*> keyActions;
	MyMap<sf::Event::KeyEvent, std::string>   actionMap;
	MyMap<std::string, std::function<void()>> userActions;
	std::vector<std::string> userActionOrder;
	std::string actionBindWaiting = "";
	int actionBindButtonIndex = 0;

	WidgetCreator(agui::Gui *guiInstance, sf::RenderWindow* window);
	~WidgetCreator()
	{
		mGui->remove(&mainFrame);
		mGui->remove(&saveFrame);
		mGui->remove(&keyFrame);
	}
	void AddMainFrame();
	void AddSaveFrame();
	void SetGuiVisibility();
	void SetDefaultKeyMapping();
	void MapNewButton(sf::Event::KeyEvent);
	void AddKeyMapFrame();
	void UserInput(sf::Event event);
	void MouseMoved();
	void RightMousePressed();
	void LeftMousePressed();
};
extern WidgetCreator* creator;