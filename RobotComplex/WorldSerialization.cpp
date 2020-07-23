#include "Robot.h"
#include "WorldSave.h"
#include "ItemTile.h"
#include "Windows.h"
#include "ProgramData.h"
#include "FindInVector.h"
#include "SplitString.h"

void WorldSave::Serialize(std::string filename)
{
	if (!CreateDirectory("saves", NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	if (!CreateDirectory(("saves/" + filename).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	// Before saving the robots make sure to stop the one that the player's meant to be controlling
	if (program.selectedRobot)
		program.selectedRobot->stopped = true;
	worldChunks.Serialize("saves/" + filename + "/chunks.bin");
	//platforms.Serialize("saves/" + filename + "/platforms.bin");
	nextPlatforms.Serialize("saves/" + filename + "/platformsNext.bin");
	items.Serialize("saves/" + filename + "/items.bin");
	nextItemPos.Serialize("saves/" + filename + "/itemsNext.bin");
	robots.Serialize("saves/" + filename + "/robots.bin");
	nextRobotPos.Serialize("saves/" + filename + "/robotsNext.bin");
	logicTiles.Serialize("saves/" + filename + "/logic.bin");
	updateNext.Serialize("saves/" + filename + "/updateWire.bin");
	updateQueueD.Serialize("saves/" + filename + "/updateQueueD.bin");
	craftingQueue.Serialize("saves/" + filename + "/craftingQueue.bin");
	SerializeItemNames("saves/" + filename + "/itemNames.txt");
	
	SerializeMisc("saves/" + filename + "/misc.txt");
	program.hotbar.Serialize("saves/" + filename + "/inventory.bin");
}
void WorldSave::Deserialize(std::string filename)
{
	this->clear();
	world.name = filename;
	worldChunks.Deserialize("saves/" + filename + "/chunks.bin");
	DeserializeItemNames("saves/" + filename + "/itemNames.txt");
	//platforms.Deserialize("saves/" + filename + "/platforms.bin");
	nextPlatforms.Deserialize("saves/" + filename + "/platformsNext.bin");
	items.Deserialize("saves/" + filename + "/items.bin", world.oldItemNewItem);
	nextItemPos.Deserialize("saves/" + filename + "/itemsNext.bin");
	robots.Deserialize("saves/" + filename + "/robots.bin");
	nextRobotPos.Deserialize("saves/" + filename + "/robotsNext.bin");
	logicTiles.Deserialize("saves/" + filename + "/logic.bin");
	updateNext.Deserialize("saves/" + filename + "/updateWire.bin");
	updateQueueD.Deserialize("saves/" + filename + "/updateItem.bin");
	craftingQueue.Deserialize("saves/" + filename + "/craftingQueue.bin");
	DeserializeMisc("saves/" + filename + "/misc.txt");
	program.hotbar.Deserialize("saves/" + filename + "/inventory.bin");
}

void WorldSave::clear()
{
	platforms.clear();
	nextPlatforms.clear();
	items.clear();
	nextItemPos.clear();
	itemPrevMoved.clear();
	robots.clear();
	nextRobotPos.clear();
	logicTiles.clear();
	worldChunks.clear();
	craftingQueue.clear();
	updateCurr.clear();
	updateProp.clear();
	updateNext.clear();
	updateQueueD.clear();
	program.hotbar.clear();
	name = "New World";
	tick = 0;
	seed = 0;
}

void WorldSave::SerializeItemNames(std::string filename)
{
	std::ofstream myfile;
	myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		for (std::string item : program.itemPrototypes)
		{
			myfile << (item + "\r\n");
		}
		myfile.close();
	}
}

void WorldSave::DeserializeItemNames(std::string filename)
{
	std::ifstream file(filename);
	std::string str;
	int i = 0;
	while (std::getline(file, str))
	{
		std::pair<bool,int> map = findInVector(program.itemPrototypes, str);
		if(map.first)
			oldItemNewItem.insert({ i, map.second });
		else
		{
			//error item doesn't exist
		}
		i++;
	}
}
/*
void WorldSave::SerializeLogicStructure(std::string filename)
{
	std::ofstream myfile;
	myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		for (int i=0; i<logicTypes.size();i++)
		{
			LogicTile* tempLogic = LogicTile::Factory(i);
			if (tempLogic)
			{
				int logicSize = tempLogic->MemorySize();
				myfile << (logicTypes[i] + ":" + std::to_string(logicSize) + "\r\n");
			}
		}
		myfile.close();
	}
}

void WorldSave::DeserializeLogicStructure(std::string filename)
{
	std::ifstream file(filename);
	std::string str;
	int i = 0;
	while (std::getline(file, str))
	{
		std::vector<std::string> splitLine;
		split(&splitLine, str, ':');
		if (splitLine.size() != 0)
		{
			std::pair<bool, int> map = findInVector(logicTypes, splitLine[0]);
			oldLogicSize.emplace_back(std::stoi(splitLine[1]));
			if (map.first)
			{
				oldLogicNewLogic.insert({ i, map.second }); 
			}
			i++;
		}
	}
}
*/
void WorldSave::SerializeMisc(std::string filename)
{
	std::ofstream myfile;
	myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		myfile << "Camera Pos," + std::to_string(program.cameraPos.x) + "," + std::to_string(program.cameraPos.y);
	}
}

void WorldSave::DeserializeMisc(std::string filename)
{
	std::ifstream myfile(filename);
	std::string str;
	while (std::getline(myfile, str))
	{
		std::vector<std::string> splitLine;
		split(&splitLine, str, ',');
		if (splitLine.size() != 0)
		{
			if (splitLine[0] == "Camera Pos")
			{
				program.cameraPos.x = std::stoi(splitLine[1]);
				program.cameraPos.y = std::stoi(splitLine[2]);
			}
		}
	}
}

