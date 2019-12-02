//#include <Windows.h>

#include "Robot.h"
#include "WorldSave.h"
#include "ItemTile.h"

void WorldSave::Serialize(std::string filename)
{
	/*
	if (!CreateDirectory("saves", NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	if (!CreateDirectory(("saves/" + filename).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
		*/
	worldChunks.Serialize("saves/" + filename + "/chunks.bin");
	items.Serialize("saves/" + filename + "/items.bin");
	robots.Serialize("saves/" + filename + "/robots.bin");
	updateQueue.Serialize("saves/" + filename + "/updateQueue.bin");
}
void WorldSave::Deserialize(std::string filename)
{
	this->clear();
	worldChunks.Deserialize("saves/" + filename + "/chunks.bin");
	items.Deserialize("saves/" + filename + "/items.bin");
	robots.Deserialize("saves/" + filename + "/robots.bin");
	updateQueue.Deserialize("saves/" + filename + "/updateQueue.bin");
}