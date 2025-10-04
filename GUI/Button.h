#pragma once
#include "SFML/Graphics.hpp"
#include <SFML/Audio.hpp>


class Button
{
public:
	sf::Vector2f textCenterPos;
	sf::Font font;
	std::string textString;
	sf::Text text;
	int fontSize;
	float rectAmplifyingValue;
	sf::Color color;
	sf::Color colorOnHover;
	sf::Color currentColor;

	Button(sf::Vector2f textCenterPos, sf::Font font, std::string text, int fontSize, sf::Color color,sf::Color colorOnHover, float rectAmplifyingValue);

	bool isHovering(sf::Vector2i mousePos);

	bool isClicking();

	void setText();

	void setTextCenterPos(sf::Vector2f newPos);

	void draw(sf::RenderWindow& window);

	bool update(sf::RenderWindow& window, bool hasClicked);


	sf::RectangleShape getRect();

	sf::Vector2f getUpdatedRectPos();

	~Button();
};

