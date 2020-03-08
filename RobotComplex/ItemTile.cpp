#include "ItemTile.h"
#include "ProgramData.h"
#include "Textures.h"
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
void ItemTile::DrawTile(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color)
{
	sf::Sprite sprite;
	sprite.setTexture(*itemTextures[this->itemTile]);
	sprite.setOrigin(GC::halfItemSprite, GC::halfItemSprite);
	sprite.setPosition(x + 16.f, y + 16.f);
	sprite.setScale(sf::Vector2f(1.5f, 1.5f));
	sprite.setColor(color);
	appendTo->emplace_back(sprite);
}