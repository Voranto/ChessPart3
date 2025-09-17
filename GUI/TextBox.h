#pragma once
#include "Button.h"

class TextBox
{
public:


	sf::Vector2f pos;
	sf::Vector2f size;

	sf::Color color;
	std::string text = "";

	sf::Font font;
	int fontSize = 50;
	bool isActive;

	TextBox(sf::Vector2f pos, sf::Vector2f size, sf::Color color);

	void drawSquare(sf::RenderWindow& window);

	void drawText(sf::RenderWindow& window);

	void update(sf::Event event);

	bool isHovering(sf::Vector2i mousePos);

	sf::RectangleShape getRect();
};

