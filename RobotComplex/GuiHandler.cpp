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

	std::vector<std::string> names = getFolderNamesInFolder("saves");
	static int currIndex = 0;

	program.anyGuiHovered = ImGui::IsAnyWindowHovered();
	program.acceptGameInput = !ImGui::IsAnyWindowFocused();
	program.acceptKeyBoardInput = program.acceptGameInput;

	const int windowWidth = 300;
	const int windowHeight = 300;
	ImGui::SetNextWindowPos(ImVec2((program.windowWidth - windowWidth) * 0.5f, (program.windowHeight - windowHeight) * 0.5f), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight), ImGuiCond_::ImGuiCond_FirstUseEver);
	switch (currentMenu)
	{
	case mainMenu:
	{
		ImGui::Begin("Main Menu");
		ImGui::TextWrapped(
			"Welcome to Robofactory!\n"
			"This is a game about automation."			
		);
		if (ImGui::Button("Play"))
			currentMenu = saveMenu;
		if (ImGui::Button("Controls"))
			currentMenu = controlMenu;
		if (ImGui::Button("Back"))
			currentMenu = noMenu;
		if (program.worldLoaded)
		{
			if (ImGui::Button("Quit Game"))
			{
				world.Serialize();
				program.worldLoaded = false;
				world.clear();
			}
		}
		else
		{
			if (ImGui::Button("Exit"))
				program.running = false;
		}
		ImGui::End();
	}break;
	case saveMenu:
	{
		ImGui::Begin("Save Menu");
		static char input[40];
		if (!program.worldLoaded)
		{
			ImGui::InputText("", input, 40);
		}
		if(std::string(input) == "" && populateResults)
		{
			size_t length = std::string(verbs[rand() % verbs.size()] + nouns[rand() % nouns.size()]).copy(input, 40);
			input[length] = '\0';
			for (int i = 0; i < names.size(); i++)
				if (program.selectedSave == names[i])
					currIndex = i;
			populateResults = false;
		}
		if (!program.worldLoaded)
		{
			if (ImGui::Button("New World"))
			{
				if (input != "")
				{
					program.selectedSave = input;
					world = WorldSave(program.selectedSave);
					program.gamePaused = false;
					world.Serialize();
					program.worldLoaded = true;
					currentMenu = noMenu;
				}
			}
		}
		ImGui::ListBox("", &currIndex, names);
		if (program.worldLoaded)
		{
			if (ImGui::Button("Save Game"))
			{
				if (names.size() > 0)
				{
					world.Serialize(names[currIndex]);
				}
				else
				{
					world.Serialize();
				}
			}
		}
		if (names.size() > 0)
		{
			if (ImGui::Button("Load Game"))
			{
				world.Serialize();
				world.clear();
				world.Deserialize(names[currIndex]);
				program.worldLoaded = true;
				currentMenu = noMenu;
			}
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
		static bool useNormalTemp;
		if (ImGui::Checkbox("Normal Controls", &useNormalTemp))
		{
			handler.useNormalMovement = useNormalTemp;
		}
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
	if (program.worldLoaded)
	{
		DrawCraftingViewer();
		DrawTechnologyViewer();
		DrawHotbar();
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
		ImGui::Text(std::string("Update Exists Delay: " + std::to_string(program.existsUpdate - program.startUpdate)).c_str());
		ImGui::End();
	}

	//ImGui::ShowDemoWindow();

	ImGui::SFML::Render(window);

	if (program.worldLoaded)
	{
		//program.DrawCrosshair(window);
		//// Crafting Viewer
		//for (auto element : program.craftingViewBacks)
		//{
		//	window.draw(element.second);
		//}
		//program.craftingViewSprites.draw(window);

		//// Technology Viewer
		//for (auto element : program.technologyViewBacks)
		//{
		//	window.draw(element.second);
		//}
		//program.technologyViewSprites.draw(window);

		//// Hotbar
		//for (auto element : program.hotbarBacks)
		//{
		//	window.draw(element.second);
		//}
		//program.hotbarSprites.draw(window);

		// Tooltips
		for (sf::RectangleShape sprite : program.textBacks)
		{
			window.draw(sprite);
		}
		for (sf::Text sprite : program.textOverlay)
		{
			window.draw(sprite);
		}
		
	}
}

void GuiHandler::DrawCraftingViewer()
{
	ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_::ImGuiCond_FirstUseEver);
	program.craftingViewShow = ImGui::Begin("Crafting Viewer");
	ImGui::TextWrapped("Arrange items on the ground in the shape of the recipe and receive the results!");
	ImGui::Text("Find recipe:");
	ImGui::SameLine();
	const int bufferSize = 40;
	static char recipe[bufferSize];
	
	ImGui::PushID(std::hash<std::string>{}("recipes"));
	if (ImGui::InputText("", recipe, bufferSize))
	{
		FindRecipes(std::string(recipe));
	}
	ImGui::PopID();
	if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
	{
		recipe[0] = '\0';
		FindUses(std::string(recipe));
	}
	ImGui::Text("Find uses:  ");
	ImGui::SameLine();
	static char uses[bufferSize];
	ImGui::PushID(std::hash<std::string>{}("uses"));
	if (ImGui::InputText("", uses, bufferSize))
	{
		FindUses(std::string(uses));
	}
	ImGui::PopID();
	if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1])
	{
		uses[0] = '\0';
		FindUses(std::string(uses));
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
	if (ImGui::Button("Next") && program.foundRecipeList.size() > 0 && program.craftingViewIndex < program.foundRecipeList.size() - 1)
	{
		program.craftingViewIndex++;
		program.craftingViewUpdate = true;
		resultsTitle = "Results: " + std::to_string(program.craftingViewIndex + 1) + "/" + std::to_string(program.foundRecipeList.size());
	}

	ImVec2 prevDimensions = program.craftingViewDimensions;
	program.craftingViewDimensions = ImGui::GetWindowSize();
	if (program.craftingViewDimensions.x != prevDimensions.x && program.craftingViewDimensions.y != prevDimensions.y)
		program.craftingViewUpdate = true;
	program.DrawCraftingView();
	for (auto back : program.craftingViewBacks)
	{
		auto tooltip = program.craftingViewTooltips.find(back.first);
		ImGui::PushID(int(std::hash<std::string>{}('c' + std::to_string(back.first.x) + ',' + std::to_string(back.first.y))));
		if (ImGui::ImageButton(back.second))
		{
			if (tooltip != program.craftingViewTooltips.end())
			{
				if (back.first.x < program.craftingViewSize.x / 2)
				{
					size_t length = std::min(int(tooltip->second.length()), bufferSize);
					for (int i = 0; i < length; i++)
					{
						if (tooltip->second[i] == ':')
						{
							length = i;
							break;
						}
						recipe[i] = tooltip->second[i];
					}
					uses[length] = '\0';
					uses[0] = '\0';
					FindRecipes(std::string(recipe));
				}
				else
				{
					size_t length = std::min(int(tooltip->second.length()), bufferSize);
					for (int i = 0; i < length; i++)
					{
						if (tooltip->second[i] == ':')
						{
							length = i;
							break;
						}
						uses[i] = tooltip->second[i];
					}
					uses[length] = '\0';
					recipe[0] = '\0';
					FindUses(std::string(uses));
				}
			}
		}
		ImGui::PopID();
		if (tooltip != program.craftingViewTooltips.end())
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(tooltip->second.c_str());
	}
	for (int i = 0; i < program.craftingViewSprites.size(); i++)
	{
		auto sprite = program.craftingViewSprites[i];
		ImGui::Image(sprite);
	}
	ImGui::End();
	
}

void GuiHandler::DrawTechnologyViewer()
{
	
	ImGui::SetNextWindowPos(ImVec2(20, 440), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_::ImGuiCond_FirstUseEver);
	program.technologyViewShow = ImGui::Begin("Technology Viewer");
	ImGui::TextWrapped(std::string("Next Objective: " + world.currentTechnology.name).c_str());
	ImGui::TextWrapped(std::string("Tips: " + world.currentTechnology.tips).c_str());

	ImVec2 prevDimensions = program.technologyViewDimensions;
	program.technologyViewDimensions = ImGui::GetWindowSize();
	if (program.technologyViewDimensions.x != prevDimensions.x && program.technologyViewDimensions.y != prevDimensions.y)
		program.technologyViewUpdate = true;
	program.DrawTechnologyView();
	for (auto back : program.technologyViewBacks)
	{
		auto tooltip = program.technologyViewTooltips.find(back.first);
		ImGui::PushID(int(std::hash<std::string>{}('t' + std::to_string(back.first.x) + ',' + std::to_string(back.first.y))));
		if (ImGui::ImageButton(back.second))
		{
			
		}
		ImGui::PopID();
		if (tooltip != program.technologyViewTooltips.end())
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(tooltip->second.c_str());
	}
	for (int i=0;i<program.technologyViewSprites.size();i++)
	{
		auto sprite = program.technologyViewSprites[i];
		ImGui::Image(sprite);
	}
	ImGui::End();
	
}

void GuiHandler::DrawHotbar()
{
	ImGui::SetNextWindowPos(ImVec2(program.halfWindowWidth - 300, program.windowHeight - 180), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600, 160), ImGuiCond_::ImGuiCond_FirstUseEver);
	program.hotbarShow = ImGui::Begin("Hotbar");
	if (ImGui::IsWindowFocused())
		program.acceptKeyBoardInput = true;
	ImVec2 prevDimensions = program.hotbarDimensions;
	program.hotbarDimensions = ImGui::GetWindowSize();
	if(program.hotbarDimensions.x != prevDimensions.x && program.hotbarDimensions.y != prevDimensions.y)
		program.hotbarUpdate = true;
	program.DrawHotbar();
	for (auto back : program.hotbarBacks)
	{
		auto tooltip = program.hotbarTooltips.find(back.first);
		ImGui::PushID(int(std::hash<std::string>{}('h'+std::to_string(back.first.x)+','+std::to_string(back.first.y))));
		if (ImGui::ImageButton(back.second))
		{
			world.MoveInventory(program.hotbarIndex, back.first);
			program.hotbarIndex = back.first;
		}
		ImGui::PopID();
		if (tooltip != program.hotbarTooltips.end())
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(tooltip->second.c_str());
	}
	for (int i = 0; i < program.hotbarSprites.size(); i++)
	{
		auto sprite = program.hotbarSprites[i];
		ImGui::Image(sprite);
	}
	ImGui::End();
}