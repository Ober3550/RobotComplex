#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Windows.h"
// Central function for error checking on texture loading
sf::Texture* LoadTexture(std::string filename);
// Textures
extern sf::Texture* groundTexture;
extern std::vector<sf::Texture*> itemTextures;
extern std::vector<sf::Texture*> animationTextures;
extern sf::Texture* robotTexture;
extern sf::Texture* font;