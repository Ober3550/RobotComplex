// Terraforma.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <SFML/Graphics.hpp>
#include "WorldSave.h"
#include "ProgramData.h"
#include "Pos.h"
#include "LogicTile.h"
#include <string>
#include "MyMod.h"
#include "Windows.h"
#include "CraftingProcess.h"
#include "RedirectorColors.h"
#include "SpriteGenerator.h"
#include "TestWorld.h"
#include "PrototypeLoader.h"
#include "GameInput.h"
#include "TitleScreen.h"

ProgramData program;
WorldSave world;
int desiredMs = 16;

int main()
{
	//Setup Window Size
	sf::RenderWindow window(sf::VideoMode(program.windowWidth, program.windowHeight), "Terraforma");
	sf::View view = window.getView();
	view.setCenter(0, 0);
	window.setView(view);
	LoadPrototypes();
	LoadLogicToHotbar();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
			{
				/*
				// Get current window size
				WINDOWINFO WindowInfo;
				GetWindowInfo(window.getSystemHandle(), &WindowInfo);
				program.windowWidth = WindowInfo.rcClient.right - WindowInfo.rcClient.left;
				program.windowHeight = WindowInfo.rcClient.bottom - WindowInfo.rcClient.top;
				*/
			}
			GameInput(window, event);
		}
		clock_t beginUpdate = clock();
		if(!program.gamePaused)
		UpdateMap();
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
		++world.tick;
	}
	return 0;
}
