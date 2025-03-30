#include "TextBox.h"
#include <iostream>

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
	this->pos = pos;
	this->size = size;
	this->color = color;
	this->isActive = false;

	this->font.loadFromFile(".\\font\\arial.ttf");
}



void TextBox::drawSquare(sf::RenderWindow& window) {
	sf::RectangleShape rect(this->size);
	rect.setPosition(this->pos);
	if (this->isActive) {
		rect.setOutlineColor(sf::Color(255, 0, 0));
		rect.setOutlineThickness(5);
	}
	else {
		rect.setOutlineThickness(0);
	}
	
	rect.setFillColor(this->color);
	window.draw(rect);
}

void TextBox::drawText(sf::RenderWindow& window) {
	sf::Text text(this->text, this->font, this->fontSize);
	text.setCharacterSize(this->fontSize);
	text.setFillColor(sf::Color(0, 0, 0));


	sf::Vector2f centerOfSquare = this->pos + sf::Vector2f(this->size.x / 2, this->size.y/2);

	sf::FloatRect textRect = text.getGlobalBounds();
	sf::Vector2f textSize(textRect.width, textRect.height);

	sf::Vector2f posToDraw = centerOfSquare - sf::Vector2f(textSize.x / 2, textSize.y / 2);

	text.setPosition(posToDraw);

	window.draw(text);
}

bool TextBox::isHovering(sf::Vector2i mousePos) {

	sf::Vector2f updatedMousePos = sf::Vector2f(mousePos.x, mousePos.y);
	sf::RectangleShape rect = this->getRect();
	sf::Vector2f size = rect.getSize();
	sf::Vector2f pos = rect.getPosition();
	return (pos.x <= updatedMousePos.x && updatedMousePos.x <= pos.x + size.x) && (pos.y <= updatedMousePos.y && updatedMousePos.y <= pos.y + size.y);
}

void TextBox::update(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {
		// Check if clicked inside the textbox
		this->isActive = isHovering(sf::Mouse::getPosition());

	}

	if (isActive && event.type == sf::Event::TextEntered) {
		if (event.text.unicode == '\b' && !this->text.empty()) {  // Handle Backspace
			this->text.pop_back();
		}
		else if (event.text.unicode >= 32 && event.text.unicode < 128) {  // Handle normal input
			
			this->text += static_cast<char>(event.text.unicode);
		}
	}

	if (isActive && event.type == sf::Event::KeyPressed) {
		if (event.key.control) {  // Detect Ctrl key
			if (event.key.code == sf::Keyboard::C) {  // Ctrl + C (Copy)
				sf::Clipboard::setString(this->text);
			}
			if (event.key.code == sf::Keyboard::V) {  // Ctrl + V (Paste)
				this->text += sf::Clipboard::getString();
			}
			
		}
	}
	
}

sf::RectangleShape  TextBox::getRect() {

	sf::RectangleShape rect(this->size);
	rect.setPosition(this->pos);
	return rect;
}