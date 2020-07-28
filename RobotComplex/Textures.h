#pragma once
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Windows.h"
// Central function for error checking on texture loading
sf::Texture* LoadTexture(std::string filename);
// Textures
extern sf::Texture* groundTexture;
extern std::vector<sf::Texture*> groundTextures;
extern std::vector<sf::Texture*> itemTextures;
extern std::vector<sf::Texture*> animationTextures;
extern std::unordered_map<uint8_t, sf::Texture*> logicTextures;
extern sf::Texture* robotTexture;
extern sf::Texture* font;