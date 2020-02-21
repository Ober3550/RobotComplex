#include "ParentTile.h"
#include "LogicTile.h"
#include "Robot.h"
#include "ItemTile.h"
#include "WorldSave.h"

ParentTile* ParentTile::Factory(ElementTypes element, std::ifstream* myfile)
{
	if (element == ElementTypes::logic)
	{
		LogicTypes* logictype = new LogicTypes();
		myfile->read((char*)logictype, sizeof(LogicTypes));
		int elementSize = world.oldLogicSize[*logictype];
		if (uint8_t* newlogicType = world.oldLogicNewLogic.GetValue(*logictype))
		{
			LogicTile* newLogic = LogicTile::Factory(*newlogicType);
			int temp = 100;
			newLogic->Deserialize(myfile, &temp);
			return newLogic;
		}
		else
			return nullptr;
	}
	else if (element == ElementTypes::robot)
	{
		Robot* newRobot = new Robot();
		newRobot->Deserialize(myfile);
		return newRobot;
	}
	else if (element == ElementTypes::item)
	{
		ItemTile* newItem = new ItemTile();
		newItem->Deserialize(myfile);
		return newItem;
	}
}