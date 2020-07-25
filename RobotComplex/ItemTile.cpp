#include "ItemTile.h"
#include "ProgramData.h"
#include "Textures.h"
std::string ItemTile::GetTooltip()
{
	auto tooltip = program.itemTooltips.find(this->itemTile);
	if (tooltip != program.itemTooltips.end())
		return tooltip->second;
	return "";
}
void ItemTile::DrawItem(SpriteVector* appendTo, float x, float y, float s, uint8_t flags, sf::Color color)
{
	if (this->itemTile < itemTextures.size())
	{
		sf::Sprite sprite;
		sprite.setTexture(*itemTextures[this->itemTile]);
		sprite.setOrigin(GC::halfItemSprite, GC::halfItemSprite);
		sprite.setPosition(x + GC::halfTileSize, y + GC::halfTileSize);
		sprite.setScale(sf::Vector2f(s, s));
		sprite.setColor(color);
		appendTo->emplace_back(sprite);
	}
}