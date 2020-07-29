#include "GuiHandler.h"
#include "WorldSave.h"
#include "ProgramData.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "KeyNames.h"
#include "GetFileNamesInFolder.h"
#include <functional>

static std::vector<std::string> verbs = {"Steampunk ","Dystopian ","Utopian ","Hellscape ","Peaceful ","Killer ","Chaotic "};
static std::vector<std::string> nouns = {"City", "Meadows","Mountains","Valley","Desert","Skies","Space"};

void GuiHandler::HandleGui(sf::RenderWindow& window)
{
	window.setView(program.hudView);
	sf::Vector2i tempPos = sf::Mouse::getPosition(window);
	program.mousePos = Pos{ int(tempPos.x - program.halfWindowWidth) ,int(tempPos.y - program.halfWindowHeight) };
	program.unscaledBoxes.clear();
	program.scaledBoxes.clear();
	
	//program.DrawCrosshair(window);
	for (auto element : program.hotbarSlots)
	{
		window.draw(element.second);
	}
	program.hotbarSprites.draw(window);
	for (sf::RectangleShape sprite : program.unscaledBoxes)
	{
		window.draw(sprite);
	}
	program.anyGuiHovered = ImGui::IsAnyWindowHovered();
	program.acceptGameInput = !ImGui::IsAnyWindowFocused();
	const int windowWidth = 300;
	const int windowHeight = 250;
	ImGui::SetNextWindowPos(ImVec2((program.windowWidth - windowWidth) * 0.5f, (program.windowHeight - windowHeight) * 0.5f), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_::ImGuiCond_FirstUseEver);
	switch (currentMenu)
	{
	case mainMenu:
	{
		ImGui::Begin("Main Menu");
		if (ImGui::Button("Play"))
			currentMenu = saveMenu;
		if (ImGui::Button("Controls"))
			currentMenu = controlMenu;
		if (ImGui::Button("Back"))
			currentMenu = noMenu;
		if (ImGui::Button("Exit"))
			program.running = false;
		ImGui::End();
	}break;
	case saveMenu:
	{
		ImGui::Begin("Save Menu");
		static char input[40];
		ImGui::InputText("", input, 40);
		if (std::string(input) == "" && populateResults)
		{
			size_t length = std::string(verbs[rand() % verbs.size()] + nouns[rand() % nouns.size()]).copy(input, 40);
			input[length] = '\0';
			populateResults = false;
		}
		if (ImGui::Button("New World"))
		{
			program.selectedSave = input;
			world = WorldSave(program.selectedSave);
			program.gamePaused = false;
			world.Serialize();
			currentMenu = noMenu;
		}
		std::vector<std::string> names = getFolderNamesInFolder("saves");
		static int currIndex = 0;
		ImGui::ListBox("", &currIndex, names);
		if (ImGui::Button("Save Game"))
			world.Serialize(names[currIndex]);
		if (ImGui::Button("Load Game"))
		{
			world.Serialize();
			world.clear();
			world.Deserialize(names[currIndex]);
			currentMenu = noMenu;
		}
		if (ImGui::Button("Back"))
		{
			populateResults = true;
			currentMenu = mainMenu;
		}
		ImGui::End();
	}break;
	case controlMenu:
	{
		ImGui::Begin("Control Menu");
		for (auto action : userActionOrder)
		{
			ImGui::Text(action.c_str());
			auto eventName = actionToEvent.find(action);
			if (eventName != actionToEvent.end())
			{
				ImGui::SameLine();
				if (ImGui::Button(KeyNames::toString(eventName->second).c_str()))
				{
					actionToEvent.erase(eventName);
					actionBindWaiting = action;
				}
			}
			else
			{
				ImGui::SameLine();
				if (actionBindWaiting != "")
					ImGui::Button("Waiting");
				else
					ImGui::Button("None");
			}
		}
		if (ImGui::Button("Back"))
			currentMenu = mainMenu;
		ImGui::End();
	}break;
	}
	if (craftingViewShow)
	{
		ImGui::SetNextWindowPos(ImVec2(20,20), ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Crafting Viewer"))
			program.craftingViewShow = true;
		else
			program.craftingViewShow = false;
		program.craftingViewDimensions = ImGui::GetWindowSize();
		program.craftingViewPos = ImGui::GetWindowPos();
		ImGui::Text("Find:");
		ImGui::SameLine();
		static char input[20];
		if (ImGui::InputText("", input, 20))
		{
			FindRecipes(std::string(input));
		}
		ImGui::Text(resultsTitle.c_str());
		if (program.craftingViewUnlocked)
			ImGui::Text("Recipe: Unlocked");
		else
			ImGui::Text("Recipe: Locked");
		if (ImGui::Button("Previous") && program.craftingViewIndex > 0)
		{
			program.craftingViewIndex--;
			program.craftingViewUpdate = true;
			resultsTitle = "Results: " + std::to_string(program.craftingViewIndex + 1) + "/" + std::to_string(program.foundRecipeList.size());
		}
		ImGui::SameLine();
		if (ImGui::Button("Next") && program.craftingViewIndex < program.foundRecipeList.size() - 1)
		{
			program.craftingViewIndex++;
			program.craftingViewUpdate = true;
			resultsTitle = "Results: " + std::to_string(program.craftingViewIndex + 1) + "/" + std::to_string(program.foundRecipeList.size());
		}
		
		ImGui::End();
	}
	program.DrawCraftingView();
	
	ImGui::SetNextWindowPos(ImVec2(20, 440), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_::ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Technology Viewer"))
		program.technologyViewShow = true;
	else
		program.technologyViewShow = false;
	program.technologyViewDimensions = ImGui::GetWindowSize();
	program.technologyViewPos = ImGui::GetWindowPos();
	ImGui::Text(std::string("Next Objective: "+world.currentTechnology.name).c_str());
	ImGui::Text(std::string("Tips: " + world.currentTechnology.tips).c_str());
	ImGui::End();
	program.DrawTechnologyView();

	if (program.showDebugInfo)
		program.DrawDebugHUD();

	//ImGui::ShowDemoWindow();

	ImGui::SFML::Render(window);
	// These elements will be ontop of the gui
	for (auto element : program.craftingViewBacks)
	{
		window.draw(element.second);
	}
	program.craftingViewSprites.draw(window);

	for (auto element : program.technologyViewBacks)
	{
		window.draw(element.second);
	}
	program.technologyViewSprites.draw(window);

	for (sf::Text sprite : program.textOverlay)
	{
		window.draw(sprite);
	}
}