#include "Robot.h"
#include "WorldSave.h"
#include "ItemTile.h"
#include "Windows.h"
#include "ProgramData.h"
#include "FindInVector.h"

void WorldSave::Serialize(std::string filename)
{
	if (!CreateDirectory("saves", NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	if (!CreateDirectory(("saves/" + filename).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	// Before saving the robots make sure to stop the one that the player's meant to be controlling
	if (program.selectedRobot)
		program.selectedRobot->stopped = true;
	SerializeLogicStructure("saves/" + filename + "/logicStructure.txt");
	worldChunks.Serialize("saves/" + filename + "/chunks.bin");
	//platforms.Serialize("saves/" + filename + "/platforms.bin");
	nextPlatforms.Serialize("saves/" + filename + "/platformsNext.bin");
	items.Serialize("saves/" + filename + "/items.bin");
	nextItemPos.Serialize("saves/" + filename + "/itemsNext.bin");
	robots.Serialize("saves/" + filename + "/robots.bin");
	nextRobotPos.Serialize("saves/" + filename + "/robotsNext.bin");
	logictiles.Serialize("saves/" + filename + "/logic.bin");
	//updateQueueB.Serialize("saves/" + filename + "/updateQueueB.bin");
	updateQueueC.Serialize("saves/" + filename + "/updateQueueC.bin");
	updateQueueD.Serialize("saves/" + filename + "/updateQueueD.bin");
	craftingQueue.Serialize("saves/" + filename + "/craftingQueue.bin");
	SerializeItemNames("saves/" + filename + "/itemNames.txt");
	
	SerializeMisc("saves/" + filename + "/misc.txt");
	program.hotbar.Serialize("saves/" + filename + "/inventory.bin");
}
void WorldSave::Deserialize(std::string filename)
{
	this->clear();
	DeserializeLogicStructure("saves/" + filename + "/logicStructure.txt");
	worldChunks.Deserialize("saves/" + filename + "/chunks.bin");
	DeserializeItemNames("saves/" + filename + "/itemNames.txt");
	//platforms.Deserialize("saves/" + filename + "/platforms.bin");
	nextPlatforms.Deserialize("saves/" + filename + "/platformsNext.bin");
	items.Deserialize("saves/" + filename + "/items.bin", world.oldItemNewItem);
	nextItemPos.Deserialize("saves/" + filename + "/itemsNext.bin");
	robots.Deserialize("saves/" + filename + "/robots.bin");
	nextRobotPos.Deserialize("saves/" + filename + "/robotsNext.bin");
	logictiles.Deserialize("saves/" + filename + "/logic.bin", world.oldLogicNewLogic, world.oldLogicSize);
	//updateQueueB.Deserialize("saves/" + filename + "/updateQueueB.bin");
	updateQueueC.Deserialize("saves/" + filename + "/updateQueueC.bin");
	updateQueueD.Deserialize("saves/" + filename + "/updateQueueD.bin");
	craftingQueue.Deserialize("saves/" + filename + "/craftingQueue.bin");
	DeserializeMisc("saves/" + filename + "/misc.txt");
	program.hotbar.Deserialize("saves/" + filename + "/inventory.bin");
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
				myfile << (logicTypes[i] + " : " + std::to_string(logicSize) + "\r\n");
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
		std::string name;
		std::string size;
		bool split = false;
		for (char c : str)
		{
			if (c == ':')
				split = true;
			else
			{
				if (c != ' ')
				{
					if (split)
						size += c;
					else
						name += c;
				}
			}
		}
		std::pair<bool, int> map = findInVector(logicTypes, name);
		oldLogicSize.emplace_back(std::stoi(size));
		if (map.first)
		{
			oldLogicNewLogic.insert({ i, map.second });
		}
		i++;
	}
}

void WorldSave::SerializeMisc(std::string filename)
{

}

void WorldSave::DeserializeMisc(std::string filename)
{

}

