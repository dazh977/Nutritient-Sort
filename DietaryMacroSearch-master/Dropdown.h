#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Data.h"
using namespace std;

class Dropdown {
	vector<string> items;
	int selectedItemIndex;
	bool isExpanded;
	sf::Font font;
	sf::RectangleShape box;
	float width, height;
	int scrollOffset;
	int maxDisplayItems;
public:
	Dropdown(float width, float height, const vector<string>& macros) : width(width), height(height), selectedItemIndex(-1), isExpanded(false), scrollOffset(0), maxDisplayItems(3) {
		box.setSize(sf::Vector2f(width, height));
		box.setFillColor(sf::Color::White);
		box.setOutlineColor(sf::Color::Black);
		box.setOutlineThickness(1.0f);

		box.setPosition(sf::Vector2f(100, 400));

		if (!font.openFromFile("font.ttf")) {
			cout << "Error loading font in Dropdown!" << endl;
		}

		for (const auto& macro : macros) {
			addItem(macro);
		}
	}

	void addItem(const string& item) {
		items.push_back(item);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(box);

		sf::Text text(font);
		text.setFont(font);
		text.setCharacterSize(20);
		text.setFillColor(sf::Color::Black);

		if (selectedItemIndex >= 0 && !isExpanded) {
			text.setString(items[selectedItemIndex]);
			text.setPosition(sf::Vector2f(box.getPosition().x, box.getPosition().y + 10.5));
			// text.setPosition(box.getPosition().x, box.getPosition().y + 10.5);
			window.draw(text);
		}

		if (isExpanded) {
			int displayCount = 0;
			for (size_t i = scrollOffset; i < items.size() && displayCount < maxDisplayItems; ++i) {
				text.setString(items[i]);
				text.setPosition(sf::Vector2f(box.getPosition().x, box.getPosition().y + height * (displayCount + 1)));
				window.draw(text);
				++displayCount;
			}
		}
	}

	void clear (sf::RenderWindow& window){
		window.draw(box);
		selectedItemIndex = -1;
	}

	void handleEvent(const sf::Event& event) {
		if (event.is<sf::Event::MouseButtonPressed>()) {
            const auto& mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
            sf::Vector2f mousePos(mouseEvent->position.x, mouseEvent->position.y);

            // if (box.getGlobalBounds().contains(mouseEvent->position.x, mouseEvent->position.y)) {
            if (box.getGlobalBounds().contains(mousePos)) {
                isExpanded = !isExpanded;
            }
            else if (isExpanded) {
                float itemHeight = height;
                sf::Vector2f boxPos = box.getPosition();
                for (int i = 0; i < maxDisplayItems; ++i) {
                    sf::Vector2f position(boxPos.x, boxPos.y + itemHeight * (i + 1));
                    sf::Vector2f size(width, itemHeight);
                    sf::FloatRect itemRect(position, size);
                    // sf::FloatRect itemRect(boxPos.x, boxPos.y + itemHeight * (i + 1), width, itemHeight);
                    if (itemRect.contains(mousePos)) {
                        selectedItemIndex = scrollOffset + i;
                        isExpanded = false;
                        break;
            }
        }
    }
}

		/*if (event.type == sf::Event::MouseButtonPressed) {
			sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
			if (box.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
				isExpanded = !isExpanded;
			}
			else if (isExpanded) {
				float itemHeight = height;
				sf::Vector2f boxPos = box.getPosition();
				for (int i = 0; i < maxDisplayItems; ++i) {
					sf::FloatRect itemRect(boxPos.x, boxPos.y + itemHeight * (i + 1), width, itemHeight);
					if (itemRect.contains(mousePos)) {
						selectedItemIndex = scrollOffset + i;
						isExpanded = false;
						break;
					}
				}
			}
		}*/
        
        
        else if (event.is<sf::Event::MouseWheelScrolled>() && isExpanded) {
            const auto& mouseWheelEvent = event.getIf<sf::Event::MouseWheelScrolled>();
            if (mouseWheelEvent && mouseWheelEvent->delta > 0) {
                scrollOffset = max(0, scrollOffset - 1);
            }
            else if (mouseWheelEvent && mouseWheelEvent->delta < 0) {
                scrollOffset = min(static_cast<int>(items.size()) - maxDisplayItems, scrollOffset + 1);
            }
        }
       
	}

	sf::Vector2f getPosition() const {
		return box.getPosition();
	}

	float getWidth() const {
		return box.getSize().x;
	}

	void setPosition(float x, float y) {
		box.setPosition(sf::Vector2f(x, y));
	}
	const std::string& getSelectedItem() const {
		if (selectedItemIndex >= 0 && selectedItemIndex < items.size()) {
			return items[selectedItemIndex];
		}
		static std::string empty = "";
		return empty;
	}
};
