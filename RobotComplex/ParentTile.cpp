#include "ParentTile.h"
#include "LogicTile.h"
#include "Robot.h"
#include "ItemTile.h"
ParentTile* ParentTile::Factory(ElementTypes element, std::ifstream* myfile)
{
	if (element == ElementTypes::logic)
	{
		LogicTypes* logictype = new LogicTypes();
		myfile->read((char*)logictype, sizeof(LogicTypes));
		LogicTile* newLogic = LogicTile::Factory(*logictype);
		int temp = 100;
		newLogic->Deserialize(myfile,&temp);
		return newLogic;
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