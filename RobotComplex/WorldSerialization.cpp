#include "Robot.h"
#include "WorldSave.h"
#include "ItemTile.h"
#include "Windows.h"
#include "ProgramData.h"
#include "FindInVector.h"
#include "MyStrings.h"
#include "GuiHandler.h"
#include "PrototypeLoader.h"

WorldSave::WorldSave()
{
	noiseRef = FastNoiseSIMD::NewFastNoiseSIMD();
	noiseRef->SetFractalOctaves(6);
	noiseRef->SetFractalLacunarity(2);
	noiseRef->SetFractalGain(0.5f);
	noiseRef->SetAxisScales(0.4f, 0.4f, 0.4f);
	noiseRef->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
	this->name = "";
	this->tick = 0;
}

WorldSave::WorldSave(std::string name)
{
	program.worldLoaded = true;
	noiseRef = FastNoiseSIMD::NewFastNoiseSIMD();
	noiseRef->SetFractalOctaves(6);
	noiseRef->SetFractalLacunarity(2);
	noiseRef->SetFractalGain(0.5f);
	noiseRef->SetAxisScales(0.4f, 0.4f, 0.4f);
	noiseRef->SetNoiseType(FastNoiseSIMD::NoiseType::ValueFractal);
	this->seed = int(std::hash<std::string>{}(name));
	noiseRef->SetSeed(seed);
	this->name = name;
	this->tick = 0;
	for (auto tech : program.technologyPrototypes)
	{
		tech.Lock();
	}
	if (program.technologyPrototypes.size() > 0)
	{
		currentTechnology = program.technologyPrototypes[0];
	}
	bool dowhile = true;
	while (dowhile)
	{
		if (GroundTile* ground = world.GetGroundTile(program.cameraPos >> int(GC::tileShift)))
		{
			if (ground->groundTile != 0)
				dowhile = false;
			else
			{
				program.cameraPos.y += GC::tileSize;
			}
		}
	}
	LoadLogicToHotbar();
	program.redrawGround = true;
	program.technologyViewUpdate = true;
	program.craftingViewUpdate = true;
	program.hotbarUpdate = true;
	program.forceRefresh = true;
}

void WorldSave::Serialize(std::string filename)
{
	if (world.name == "")
		return;
	if (filename != world.name)
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
	updateItemsNext.Serialize("saves/" + filename + "/updateQueueD.bin");
	craftingQueue.Serialize("saves/" + filename + "/craftingQueue.bin");
	SerializeItemNames("saves/" + filename + "/itemNames.txt");
	SerializeMisc("saves/" + filename + "/misc.txt");
	resourcesDelivered.Serialize("saves/" + filename + "/resourcesDelivered.bin");
	program.hotbar.Serialize("saves/" + filename + "/inventory.bin");
	strings::Serialize(&unlockedTechnologies, "saves/" + filename + "/unlockedTechnologies.txt");
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
	updateItemsNext.Deserialize("saves/" + filename + "/updateItem.bin");
	craftingQueue.Deserialize("saves/" + filename + "/craftingQueue.bin");
	DeserializeMisc("saves/" + filename + "/misc.txt");
	program.hotbar.Deserialize("saves/" + filename + "/inventory.bin", world.oldItemNewItem);
	resourcesDelivered.Deserialize("saves/" + filename + "/resourcesDelivered.bin");
	strings::Deserialize(&unlockedTechnologies, "saves/" + filename + "/unlockedTechnologies.txt");

	for (auto tech : program.technologyPrototypes)
	{
		for (std::string unlockTech : unlockedTechnologies)
		{
			if (tech.name == unlockTech)
			{
				tech.Unlock();
				break;
			}
		}
	}
	currentTechnology = TechProto();
	FindNextTechnology();

	// Don't forget to set the seeds so world gen stays consistent
	srand(std::hash<std::string>{}(filename));

	this->seed = int(std::hash<std::string>{}(name));
	noiseRef->SetSeed(seed);

	program.selectedSave = filename;
	program.gamePaused = false;
	program.forceRefresh = true;
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
	updateItemsNext.clear();
	program.hotbar.clear();
	unlockedTechnologies.clear();
	for (auto tech : program.technologyPrototypes)
		tech.Lock();
	name = "";
	tick = 0;
	seed = 0;
}

void WorldSave::SerializeItemNames(std::string filename)
{
	std::ofstream myfile;
	myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		for (auto item : program.itemPrototypes)
		{
			if (item.second.length() != 0)
			{
				myfile << std::to_string(item.first) << " " << item.second << "\r\n";
			}
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
		std::vector<std::string> splitStr;
		split(&splitStr, str, ' ');
		auto item = program.itemLookups.find(splitStr[1]);
		if(item != program.itemLookups.end())
			oldItemNewItem.insert({ std::stoi(splitStr[0]), item->second });
		else
		{
			//error item doesn't exist
		}
		i++;
	}
}
void WorldSave::SerializeMisc(std::string filename)
{
	std::ofstream myfile;
	myfile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (myfile.is_open())
	{
		myfile << "Camera Pos," + std::to_string(program.cameraPos.x) + "," + std::to_string(program.cameraPos.y) + '\n';
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

