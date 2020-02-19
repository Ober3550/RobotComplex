#include "ItemTile.h"
#include "ProgramData.h"
std::string ItemTile::GetTooltip()
{
	return program.itemTooltips[this->itemTile];
}