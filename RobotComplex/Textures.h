#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Windows.h"
// Central function for error checking on texture loading
sf::Texture* LoadTexture(std::string filename);
// Textures
extern std::vector<sf::Texture*> groundTextures;
extern std::vector<sf::Texture*> itemTextures;
extern std::vector<sf::Texture*> animationTextures;
extern sf::Texture* robotTexture;
extern sf::Texture* font;
extern sf::Texture* buttonTexture;