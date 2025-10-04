
#pragma once

#include "GUI_Screens.h"
#include "SFML/Graphics.hpp"
#include "Button.h"
#include <optional>
#include "../Engine/Board.h"
#include "TextBox.h"

class GUI {

public:
	GUI_SCREENS mode;
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	std::vector<Button> buttons;
	std::vector<TextBox> textBoxes;

	void changeMode(GUI_SCREENS newMode);

	GUI(GUI_SCREENS screen, sf::Texture backgroundTexture);
	GUI(GUI_SCREENS screen);
	GUI();

	std::optional<Button> renderButtons(sf::RenderWindow& window,bool hasClicked);

	void drawBackground(sf::RenderWindow& window);

	void renderTextBoxes(sf::RenderWindow& window, bool hasClicked);

	void setBackground(sf::Texture& texture,sf::Vector2f windowSize);

	void drawCircle(sf::RenderWindow& window, sf::Vector2f pos, float radius, sf::Color color);

	int updateClickEvent(int clickEvent, int newEvent);

	int processEventsAndReturnOnClick(sf::RenderWindow& window);
};