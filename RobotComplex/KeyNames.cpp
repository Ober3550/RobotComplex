#include "KeyNames.h"
#include "MyStrings.h"

namespace KeyNames
{
	std::string toString(sf::Event::KeyEvent input)
	{
		std::string newString = "";
		if (input.control)
			newString += "Control + ";
		if (input.shift)
			newString += "Shift + ";
		if (input.alt)
			newString += "Alt + ";
		if (input.system)
			newString += "Func + ";
		newString += keyNames[input.code];
		return newString;
	}

	sf::Event::KeyEvent* toEvent(std::string input)
	{
		sf::Event::KeyEvent* event = new sf::Event::KeyEvent();
		std::vector<std::string> splitLine;
		split(&splitLine, input, '+');
		if (splitLine.size() != 0)
		{
			for (size_t i = 0; i < keyNames.size(); i++)
			{
				if (splitLine[splitLine.size()-1] == keyNames[i])
				{
					event->code = (sf::Keyboard::Key)i;
					break;
				}
			}
			for (size_t i = 0; i < splitLine.size()-1; i++)
			{
				if (splitLine[i] == "Control")
					event->control = true;
				else if (splitLine[i] == "Shift")
					event->shift = true;
				else if (splitLine[i] == "Alt")
					event->alt = true;
				else if (splitLine[i] == "Func")
					event->system = true;
			}
		}
		return event;
	}
};
