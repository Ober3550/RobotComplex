#include "WidgetCreator.h"
#include "ProgramData.h"
#include "WorldSave.h"
#include <Windows.h>
#include <vector>
#include <string>

std::vector<std::string> getFileNamesInFolder(std::string folder)
{
	std::vector<std::string> names;
	std::string pattern(folder);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			names.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
	names.erase(names.begin()); // Removes . filesystem element
	names.erase(names.begin()); // Removes .. filesystem element
	return names;
}

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

WidgetCreator::WidgetCreator(agui::Gui *guiInstance)
{
	mGui = guiInstance;
	mGui->add(&mainFrame);
	// Options mainFrame
	int mainFrameWidth = 300;
	int mainFrameHeight = 400;
	mainFrame.setSize(mainFrameWidth, mainFrameHeight);
	mainFrame.setLocation((program.windowWidth / 2) - (mainFrameWidth / 2), (program.windowHeight / 2) - (mainFrameHeight / 2));
	mainFrame.setText("Main Menu");
	// Play Button changes to load and save frame
	playButton.setSize(mainFrameWidth - 10, 50);
	playButton.setText("Play");
	playButton.addButtonListener(new SimpleButtonListener([&]{
		program.showMain = false;
		program.showSave = true;
	}));
	mainFrame.add(&playButton);
	playButton.setLocation(0, 0);
	// Exit Button
	exitButton.setText("Exit Game");
	exitButton.addButtonListener(new SimpleButtonListener([&] {
		program.running = false;
	}));
	mainFrame.add(&exitButton);
	exitButton.setSize(mainFrameWidth - 10, 50);
	exitButton.setLocation(0, mainFrameHeight - 85);
	// Save Button
	saveButton.setText("Save Game");
	saveButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldmutex.lock();
		world.Serialize(program.selectedSave);
		program.worldmutex.unlock();
	}));
	saveFrame.add(&saveButton);
	saveButton.setSize(mainFrameWidth / 2 - 10, 50);
	saveButton.setLocation(0, mainFrameHeight - 85);
	// Load Button
	loadButton.setText("Load Game");
	loadButton.addButtonListener(new SimpleButtonListener([&] {
		program.worldmutex.lock();
		world.Deserialize(program.selectedSave);
		program.worldmutex.unlock();
	}));
	saveFrame.add(&loadButton);
	loadButton.setSize(mainFrameWidth / 2 - 10, 50);
	loadButton.setLocation(mainFrameWidth / 2, mainFrameHeight - 85);
	//Save Frame
	mGui->add(&saveFrame);
	saveFrame.setText("Save Menu");
	saveFrame.setSize(mainFrameWidth, mainFrameHeight);
	saveFrame.setLocation((program.windowWidth / 2) - (mainFrameWidth / 2), (program.windowHeight / 2) - (mainFrameHeight / 2));
	
	//Save Listbox
	saveFrame.add(&worldSaves);
	std::vector<std::string> names = getFileNamesInFolder("saves");
	for (std::string name : names)
	{
		worldSaves.addItem(name);
	}
	//worldSaves.setMultiselectExtended(true);
	worldSaves.addSelectionListener(new SaveSelect());
	worldSaves.setLocation(0, 0);
	worldSaves.setSize(mainFrameWidth, mainFrameHeight-100);
}
