#include "GUI.h"
#include <iostream>
#include <optional>
#include "Button.h"
#include "GUI_Screens.h"
void GUI::changeMode(GUI_SCREENS newMode) {
    this->mode = newMode;
}

GUI::GUI(GUI_SCREENS screen) {
    this->mode = screen;
}
GUI::GUI(){}


std::optional<Button> GUI::renderButtons(sf::RenderWindow& window, bool hasClicked) {
    bool isClicked;
    std::optional<Button> clickedButton;
    for (Button& button : this->buttons) {
        isClicked = button.update(window, hasClicked);
        if (isClicked) {
            clickedButton = button;
        }
        button.draw(window);
    }
    return clickedButton;
}

void GUI::drawBackground(sf::RenderWindow& window) {
    window.draw(this->backgroundSprite);
}

void GUI::setBackground(sf::Texture& texture,sf::Vector2f windowSize) {
    this->backgroundSprite.setTexture(texture);
    sf::FloatRect bounds = this->backgroundSprite.getLocalBounds();

    // Calculate scale factors
    float scaleX = windowSize.x / bounds.width;
    float scaleY = windowSize.y / bounds.height;

    // Apply scale
    this->backgroundSprite.setScale(scaleX, scaleY);
}

void GUI::drawCircle(sf::RenderWindow& window, sf::Vector2f pos, float radius, sf::Color color) {
    sf::CircleShape circle(radius);
    circle.setPosition(pos);
    circle.setFillColor(color);
    window.draw(circle);
}

//If no relevant event, return 0
//IF button pressed return 1
//If button released return 2
// If button is currently pressed but not in the current event return 3
int GUI::processEventsAndReturnOnClick(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event))
    {

        for (TextBox& tb : this->textBoxes) {
            tb.update(event);
        }


        if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                return 1;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                return 2;
            }
        }
    }
    return 0;
}

int GUI::updateClickEvent(int clickEvent, int newEvent) {
    if (newEvent == 0 && clickEvent == 1) { clickEvent = 3; }
    else if (newEvent == 0 && clickEvent == 3) { clickEvent = 3; }
    else if (newEvent == 0 && clickEvent == 2) { clickEvent = 0; }
    else { clickEvent = newEvent; }
    return clickEvent;
}

void GUI::renderTextBoxes(sf::RenderWindow& window, bool hasClicked) {
    for (TextBox& tb : this->textBoxes) {
        tb.drawSquare(window);
        tb.drawText(window);
    }
}

