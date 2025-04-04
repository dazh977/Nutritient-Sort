#pragma once
#include <SFML/Graphics.hpp>
using namespace std;

/*struct Text {
    Text() {}

    sf::Text setText(sf::Font& font, sf::String message, int size, sf::Color color, sf::Vector2f position, bool bold, bool italic, bool underlined) {
        sf::Text text(font, message, size); // Corrected constructor
        text.setFillColor(color);
        sf::FloatRect rect = text.getLocalBounds();
        text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
        text.setPosition(position);
        if (bold) { text.setStyle(sf::Text::Bold); }
        if (italic) { text.setStyle(text.getStyle() | sf::Text::Italic); } // Combine styles
        if (underlined) { text.setStyle(text.getStyle() | sf::Text::Underlined); } // Combine styles
        return text;
    }
};*/


struct Text {
    Text() {}

    sf::Text setText(sf::Font& font, sf::String message, int size, sf::Color color, sf::Vector2f position, bool bold, bool italic, bool underlined) {
        sf::Text text(font, message, size); // Correct constructor
        text.setFillColor(color);

        // Get the bounding box and center the origin
        sf::FloatRect rect = text.getLocalBounds();
        //text.setOrigin(rect.getPosition().x + rect.getSize().x / 2.0f, rect.getPosition().y + rect.getSize().y / 2.0f);

        text.setPosition(position);

        // Set styles
        if (bold) { text.setStyle(sf::Text::Bold); }
        if (italic) { text.setStyle(text.getStyle() | sf::Text::Italic); }
        if (underlined) { text.setStyle(text.getStyle() | sf::Text::Underlined); }

        return text;
    }
};



