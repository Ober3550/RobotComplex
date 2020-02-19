#include "ItemTile.h"
#include "ProgramData.h"
std::string ItemTile::GetTooltip()
{
	return program.itemTooltips[this->itemTile];
}

void ItemTile::Serialize(std::ofstream* writer)
{
	writer->write((char*)&this->itemTile, sizeof(uint16_t));
	writer->write((char*)&this->quantity, sizeof(uint8_t));
}
void ItemTile::Deserialize(std::ifstream* reader)
{
	reader->read((char*)&this->itemTile, sizeof(uint16_t));
	reader->read((char*)&this->quantity, sizeof(uint8_t));
}