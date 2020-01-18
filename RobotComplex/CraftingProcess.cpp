#include <SFML/Graphics.hpp>
#include "CraftingProcess.h"
#include "ProgramData.h"
#include "Textures.h"
#include "MyMod.h"
#include "Animation.h"

void CraftingProcess::DrawAnimation(int x, int y)
{
	if (animationReference != 0)	// Default animation is none
	{
		Animation animationProperties = program.animationTemplates[animationReference - 1];
		sf::Sprite sprite;
		sprite.setTexture(*animationTextures[animationReference - 1]);
		int numSlides = int(sprite.getGlobalBounds().width) / animationProperties.frameWidth;
		if (MyMod(ticks, animationProperties.animationSpeed) == 0)
		{
			switch (animationProperties.animationType)
			{
			case ping:
			{
				int rotation = MyMod(ticks / animationProperties.animationSpeed, (numSlides * 2) - 2);
				if (rotation > numSlides - 1)
				{
					animationSlide = (numSlides * 2) - 2 - rotation;
				}
				else
				{
					animationSlide = rotation;
				}
			}break;
			case forward:
			{
				MyMod(ticks / animationProperties.animationSpeed, numSlides);
			}break;
			case backward:
			{
				MyMod(-int(ticks) / animationProperties.animationSpeed, numSlides);
			}break;
			}
		}
		sf::IntRect animationRect(animationProperties.frameWidth *animationSlide, 0, animationProperties.frameWidth, int(sprite.getGlobalBounds().height));
		sprite.setTextureRect(animationRect);
		sprite.setPosition(float(x + animationProperties.animationOffset.x), float(y + animationProperties.animationOffset.y));
		program.animationSprites.emplace_back(sprite);
	}
}