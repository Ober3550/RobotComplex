#pragma once
#include <Agui/Agui.hpp>
#include <Agui/Backends/SFML2/SFML2.hpp>
#include <Agui/Widgets/Button/Button.hpp>
#include <Agui/Widgets/Button/ButtonListener.hpp>
#include <Agui/Widgets/CheckBox/CheckBox.hpp>
#include <Agui/Widgets/DropDown/DropDown.hpp>
#include <Agui/Widgets/TextField/TextField.hpp>
#include <Agui/Widgets/Frame/Frame.hpp>
#include <Agui/Widgets/RadioButton/RadioButton.hpp>
#include <Agui/Widgets/RadioButton/RadioButtonGroup.hpp>
#include <Agui/Widgets/Slider/Slider.hpp>
#include <Agui/Widgets/TextBox/ExtendedTextBox.hpp>
#include <Agui/Widgets/Tab/TabbedPane.hpp>
#include <Agui/Widgets/ListBox/ListBox.hpp>
#include <Agui/Widgets/ScrollPane/ScrollPane.hpp>
#include <Agui/FlowLayout.hpp>
class WidgetCreator {
public:
	agui::Button loadButton;
	agui::Button saveButton;
	agui::Button exitButton;
	agui::Frame frame;
	agui::Gui* mGui;
	WidgetCreator(agui::Gui *guiInstance);
};
extern WidgetCreator* creator;