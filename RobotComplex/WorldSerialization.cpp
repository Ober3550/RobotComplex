#include "Robot.h"
#include "WorldSave.h"
#include "ItemTile.h"
#include "Windows.h"
#include "ProgramData.h"

void WorldSave::Serialize(std::string filename)
{
	if (!CreateDirectory("saves", NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	if (!CreateDirectory(("saves/" + filename).c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
		return;
	worldChunks.Serialize("saves/" + filename + "/chunks.bin");
	items.Serialize("saves/" + filename + "/items.bin");
	nextItemPos.Serialize("saves/" + filename + "/itemsNext.bin");
	robots.Serialize("saves/" + filename + "/robots.bin");
	nextRobotPos.Serialize("saves/" + filename + "/robotsNext.bin");
	logictiles.Serialize("saves/" + filename + "/logic.bin");
	updateQueueB.Serialize("saves/" + filename + "/updateQueueB.bin");
	updateQueueC.Serialize("saves/" + filename + "/updateQueueC.bin");
	updateQueueD.Serialize("saves/" + filename + "/updateQueueD.bin");
	craftingQueue.Serialize("saves/" + filename + "/craftingQueue.bin");
}
void WorldSave::Deserialize(std::string filename)
{
	this->clear();
	worldChunks.Deserialize("saves/" + filename + "/chunks.bin");
	items.Deserialize("saves/" + filename + "/items.bin");
	nextItemPos.Deserialize("saves/" + filename + "/itemsNext.bin");
	robots.Deserialize("saves/" + filename + "/robots.bin");
	nextRobotPos.Deserialize("saves/" + filename + "/robotsNext.bin");
	logictiles.Deserialize("saves/" + filename + "/logic.bin");
	updateQueueB.Deserialize("saves/" + filename + "/updateQueueB.bin");
	updateQueueC.Deserialize("saves/" + filename + "/updateQueueC.bin");
	updateQueueD.Deserialize("saves/" + filename + "/updateQueueD.bin");
	craftingQueue.Deserialize("saves/" + filename + "/craftingQueue.bin");
}