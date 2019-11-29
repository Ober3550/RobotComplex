#include <SFML/Graphics.hpp>
#include <Agui/Agui.hpp>
#include <Agui/Backends/SFML2/SFML2.hpp>
#include <Agui/Widgets/Button/Button.hpp>
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
#include <Agui/FlowLayout.hpp>

#include <iostream>
#include <functional>
#include <string>
#include "Windows.h"
#include <thread>

#include "WorldSave.h"
#include "ProgramData.h"
#include "Pos.h"
#include "LogicTile.h"
#include "MyMod.h"
#include "CraftingProcess.h"
#include "RedirectorColors.h"
#include "SpriteGenerator.h"
#include "TestWorld.h"
#include "PrototypeLoader.h"
#include "GameInput.h"
#include "TitleScreen.h"

agui::Gui *gui = NULL;
agui::SFML2Input* inputHandler = NULL;
agui::SFML2Graphics* graphicsHandler = NULL;
agui::Font *defaultFont = NULL;
bool running = true;

ProgramData program;
WorldSave world;
int desiredMs = 16;

class SimpleButtonListener : public agui::ActionListener
{
private:
	std::function<void()> buttonAction;
public:
	SimpleButtonListener(std::function<void()> buttonAction)
	{
		this->buttonAction = buttonAction;
	}
	virtual void actionPerformed(const agui::ActionEvent &evt)
	{
		buttonAction();
	}
};

class WidgetCreator {
private:
	agui::Button button;
	agui::CheckBox checkBox;
	agui::Frame frame;
	agui::Gui* mGui;

public:
	WidgetCreator(agui::Gui *guiInstance)
	{
		mGui = guiInstance;

		gui->add(&frame);
		frame.setSize(220, 120);
		frame.setLocation(320, 240);
		frame.setText("Example Frame");
		button.setSize(100, 100);
		button.setText("Load Game");
		button.addActionListener(&SimpleButtonListener([&] {
			world = WorldSave();
			program.onTitleScreen = false;
			program.showOptions = false;
			program.gamePaused = false;
			CreateTestWorld();
		}));
		frame.add(&button);
	}
};
WidgetCreator* creator;
void initializeAgui(sf::RenderTarget& target)
{
	//Set the image loader
	agui::Image::setImageLoader(new agui::SFML2ImageLoader());
	//Set the font loader
	agui::Font::setFontLoader(new agui::SFML2FontLoader());
	//Instance the input handler
	inputHandler = new agui::SFML2Input();
	//Instance the graphics handler
	graphicsHandler = new agui::SFML2Graphics(target);
	agui::Color::setPremultiplyAlpha(false);
	//Instance the gui
	gui = new agui::Gui();
	//Set the input handler
	gui->setInput(inputHandler);
	//Set the graphics handler
	gui->setGraphics(graphicsHandler);
	//Set the font
	defaultFont = agui::Font::load("Fonts/DejaVuSans.ttf", 16);
	//Setting a global font is required and failure to do so will crash.
	agui::Widget::setGlobalFont(defaultFont);
}

void addWidgets()
{
	creator = new WidgetCreator(gui);
}

void cleanUp()
{
	gui->getTop()->clear();
	delete creator;
	creator = NULL;
	delete gui;
	gui = NULL;
	delete inputHandler;
	delete graphicsHandler;
	inputHandler = NULL;
	graphicsHandler = NULL;
	delete defaultFont;
	defaultFont = NULL;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1920, 1010), "Agui - SFML2 Example");
	sf::View view = window.getView();
	view.setCenter(0, 0);
	window.setView(view);
	initializeAgui(window);
	addWidgets();
	running = true;
	LoadPrototypes();
	LoadLogicToHotbar();
	world = WorldSave();
	CreateTestWorld();
	program.onTitleScreen = false;
	program.gamePaused = false;
	std::thread worldUpdate([]{
		while (running) { 
			if(!program.gamePaused)
				UpdateMap(); 
			Sleep(10); 
		}
	});
	while (window.isOpen() && running)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				gui->resizeToDisplay();
			}

			inputHandler->processEvent(event);
			GameInput(window, event);
		}
		window.clear();
		clock_t beginUpdate = clock();
		program.DrawGameState(window);
		clock_t updateTime = clock();
		int padTime = desiredMs + beginUpdate - updateTime;
		if (padTime > 0)
			Sleep(padTime);
		clock_t endUpdate = clock();
		program.deltaTime += endUpdate - beginUpdate;
		program.frames++;
		if (program.deltaTime > 1000.0)
		{
			program.frameRate = double(program.frames) * 0.5 + program.frameRate * 0.5;
			program.frames = 0;
			program.deltaTime -= CLOCKS_PER_SEC;
		}
		//gui->logic();
		//gui->render();
		window.display();
	}
	cleanUp();
	return 0;
}