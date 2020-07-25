#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
class SpriteVector : public std::vector<sf::Sprite>
{
public:
	void draw(sf::RenderWindow& window)
	{
		for (auto sprite: *this)
		{
			window.draw(sprite);
		}
	}
};