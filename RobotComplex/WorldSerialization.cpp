#include "Robot.h"
#include "WorldSave.h"
#include "ItemTile.h"
#include "Windows.h"

void WorldSave::Serialize(std::string filename)
{
	if (!CreateDirectory("saves", NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	if (!CreateDirectory(("saves/" + filename).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	worldChunks.Serialize("saves/" + filename + "/chunks.bin");
	items.Serialize("saves/" + filename + "/items.bin");
	robots.Serialize("saves/" + filename + "/robots.bin");
	logictiles.Serialize("saves/" + filename + "/logic.bin");
	updateQueueB.Serialize("saves/" + filename + "/updateQueueA.bin");
	updateQueueC.Serialize("saves/" + filename + "/updateQueueB.bin");
	craftingQueue.Serialize("saves/" + filename + "/craftingQueue.bin");
}
void WorldSave::Deserialize(std::string filename)
{
	this->clear();
	worldChunks.Deserialize("saves/" + filename + "/chunks.bin");
	items.Deserialize("saves/" + filename + "/items.bin");
	robots.Deserialize("saves/" + filename + "/robots.bin");
	logictiles.Deserialize("saves/" + filename + "/logic.bin");
	updateQueueB.Deserialize("saves/" + filename + "/updateQueueA.bin");
	updateQueueC.Deserialize("saves/" + filename + "/updateQueueB.bin");
	craftingQueue.Deserialize("saves/" + filename + "/craftingQueue.bin");
}