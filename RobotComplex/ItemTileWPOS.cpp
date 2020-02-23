#include "ItemTileWPOS.h"
#include "ProgramData.h"
std::string ItemTileWPOS::GetTooltip()
{
	return program.itemTooltips[this->itemTile];
}
void ItemTileWPOS::Serialize(std::ofstream* writer)
{
	writer->write((char*)&this->pos, sizeof(Pos));
	writer->write((char*)&this->itemTile, sizeof(uint16_t));
	writer->write((char*)&this->quantity, sizeof(uint8_t));
}
void ItemTileWPOS::Deserialize(std::ifstream* reader)
{
	reader->read((char*)&this->pos, sizeof(Pos));
	reader->read((char*)&this->itemTile, sizeof(uint16_t));
	reader->read((char*)&this->quantity, sizeof(uint8_t));
}
ItemTileWPOS* ItemTileWPOS::Copy()
{
	ItemTileWPOS* copy = new ItemTileWPOS();
	copy->pos = this->pos;
	copy->itemTile = this->itemTile;
	copy->quantity = this->quantity;
	return copy;
}