#include "GuiHandler.h"
#include "WorldSave.h"
#include "ProgramData.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include "KeyNames.h"
#include "GetFileNamesInFolder.h"
#include <functional>
#include <experimental/filesystem>

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

	std::vector<std::string> names = getFolderNamesInFolder("saves");
	static int currIndex = 0;

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
		static bool openDeletePopup = false;
		openDeletePopup |= ImGui::Button("Delete Save");
		if (openDeletePopup)
		{
			ImGui::OpenPopup("Delete Popup");
		}
		if (ImGui::BeginPopup("Delete Popup"))
		{
			ImGui::Text("Are you sure?");
			if (ImGui::Button("Yes")) {
				if(names[currIndex] != "")
					std::experimental::filesystem::remove_all("saves/" + names[currIndex]);
				openDeletePopup = false;
				ImGui::CloseCurrentPopup();
			} ImGui::SameLine();
			if (ImGui::Button("No")) {
				openDeletePopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
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
	if (program.selectedSave != "")
	{
		ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_::ImGuiCond_FirstUseEver);
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
		program.DrawCraftingView();

		ImGui::SetNextWindowPos(ImVec2(20, 440), ImGuiCond_::ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_::ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Technology Viewer"))
			program.technologyViewShow = true;
		else
			program.technologyViewShow = false;
		program.technologyViewDimensions = ImGui::GetWindowSize();
		program.technologyViewPos = ImGui::GetWindowPos();
		ImGui::Text(std::string("Next Objective: " + world.currentTechnology.name).c_str());
		ImGui::Text(std::string("Tips: " + world.currentTechnology.tips).c_str());
		ImGui::End();
		program.DrawTechnologyView();
	}
	if (showDebug)
	{
		ImGui::Begin("Debug Information");
		ImGui::Text(std::string("FPS/UPS: " + std::to_string(program.frameRate) + "/" + std::to_string(program.updateRate)).c_str());
		ImGui::Text(std::string("Camera x/y: " + std::to_string(program.cameraPos.x) + "/" + std::to_string(program.cameraPos.y)).c_str());
		ImGui::Text(std::string("Map    x/y: " + std::to_string(program.mouseHovering.x) + "/" + std::to_string(program.mouseHovering.y)).c_str());
		ImGui::Text(std::string("Screen x/y: " + std::to_string(program.mousePos.x) + "/" + std::to_string(program.mousePos.y)).c_str());
		ImGui::Text(std::string("Zoom: " + std::to_string(program.zoom)).c_str());
		ImGui::Text(std::string("Tiles  Rendered: " + std::to_string(program.groundSprites.size())).c_str());
		ImGui::Text(std::string("Items  Rendered: " + std::to_string(program.itemSprites.size())).c_str());
		ImGui::Text(std::string("Logic  Rendered: " + std::to_string(program.logicSprites.size())).c_str());
		ImGui::Text(std::string("Robots Rendered: " + std::to_string(program.robotSprites.size())).c_str());
		ImGui::Text(std::string("Update Exists Size: " + std::to_string(program.elementExists.size())).c_str());
		ImGui::Text(std::string("Update Exists Max Size: " + std::to_string(program.elementExists.max_size())).c_str());
		ImGui::Text(std::string("Update Exists Delay: " + std::to_string(program.existsUpdate - program.startUpdate)).c_str());
		ImGui::End();
	}

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