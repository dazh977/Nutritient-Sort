#include <SFML/Graphics.hpp> 
//#include <iomanip>
#include <chrono>
#include "Text.h"
#include "Data.h"
#include "Dropdown.h"

using namespace std;

void adjustDropdownTextPosition(const Dropdown& dropdown, sf::Text& text, float offset) {
    sf::Vector2f dropdownPos = dropdown.getPosition();
    float textYPos = dropdownPos.y - offset;
    float textWidth = text.getLocalBounds().size.x;
    float textXPos = dropdownPos.x + (dropdown.getWidth() / 4.0f);
    text.setPosition(sf::Vector2f(textXPos, textYPos));
}

int main() {
    int width = 1200;
    int height = 1200;
    int resultScrollOffset = 1;
    const int lineHeight = 9;
    const int MAX_RESULT_LINES = 30;
    sf::FloatRect resultArea(sf::Vector2f(10.f, 600.f), sf::Vector2f(width - 20, MAX_RESULT_LINES * lineHeight));
  
    // Create data object
    Data data("2019-2020-FNDDS-At-A-Glance-Ingredient-Nutrient-Values.csv");
    data.organizeData();

    // Get font
    sf::Font font;
    if (!font.openFromFile("font.ttf")) {
        cout << "Error loading font!" << endl;
    }

    Text text;
    sf::Text header = text.setText(font, sf::String("Dietary Macro Search!"), 40, sf::Color::White, sf::Vector2f(width / 4.0f, height / 50 + 25), true, false, false);
    
    // Macro dropdown
    Dropdown dropdown(300, 50, data.getMacros());
    dropdown.setPosition(width / 2.0f - 500, height / 50 + 150);
    sf::Text dropdownOne = text.setText(font, sf::String("Choose a Macro:"), 20, sf::Color::White, sf::Vector2f(0, 0), false, false, true);
    adjustDropdownTextPosition(dropdown, dropdownOne, 25);

    // Don't include macro dropdown
    Dropdown dropdown2(300, 50, data.getMacros());
    dropdown2.setPosition(width / 2.0f + 50, height / 50 + 150);
    sf::Text dropdownTwo = text.setText(font, sf::String("Exclude Macro (optional):"), 20, sf::Color::White, sf::Vector2f(0, 0), false, false, true);
    adjustDropdownTextPosition(dropdown2, dropdownTwo, 25);

    // Dropdown for merge sort or heap sort
    vector<string> sortingMethods = { "Heap", "Merge" };
    Dropdown sortMethodDropdown(300, 50, sortingMethods);
    sortMethodDropdown.setPosition(width / 2 - 75, height / 50 + 375);
    sf::Text sortMethodText = text.setText(font, sf::String("Choose Sorting Method:"), 20, sf::Color::White, sf::Vector2f(0, 0), false, false, true);
    adjustDropdownTextPosition(sortMethodDropdown, sortMethodText, 25);

    // Button and text for search button
    sf::RectangleShape searchButton(sf::Vector2f(200, 50));
    searchButton.setPosition(sf::Vector2f(width / 2.0f - 300, height / 50 + 500));
    searchButton.setFillColor(sf::Color::White);
    sf::Text searchButtonText = text.setText(font, "Search", 20, sf::Color(32, 161, 146), sf::Vector2f(width / 2.0f - 250, height / 50 + 525), false, false, false);

    // Button and text for clear button
    sf::RectangleShape clearButton(sf::Vector2f(200, 50));
    clearButton.setPosition(sf::Vector2f(width / 2.0f + 100, height / 50 + 500));
    clearButton.setFillColor(sf::Color::White);
    sf::Text clearButtonText = text.setText(font, "Clear", 20, sf::Color(32, 161, 146), sf::Vector2f(width / 2.0f + 200, height / 50 + 525), false, false, false);

    // Result and error text and result texture
    sf::Text resultText = text.setText(font, "", 15, sf::Color::Black, sf::Vector2f(100, height / 50 + 750), false, false, false);
    sf::RenderTexture resultTexture(sf::Vector2u(resultArea.size.x, resultArea.size.y));
    // resultTexture.create(resultArea.width, resultArea.height);
    sf::Text errorText = text.setText(font, "", 20, sf::Color::Black, sf::Vector2f(width / 2 - 250, 800), false, false, false);

    // Window
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(width, height)), "Dietary Macro Search!");
    sf::Event event(sf::Event::KeyPressed{});

    while (window.isOpen()) {
        // sf::Event event;
     //sf::Event event(sf::Event::KeyPressed{});
        // while (window.pollEvent(event)) {
        while (auto optionalEvent = window.pollEvent()) {
        // while (window.pollEvent()) {
            const sf::Event& event = *optionalEvent;
            // if (event.type == sf::Event::MouseWheelScrolled) {
            if (event.is<sf::Event::MouseWheelScrolled>()) {
                const auto& mouseEvent = event.getIf<sf::Event::MouseWheelScrolled>();
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                // if (resultArea.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                if (resultArea.contains(sf::Vector2f(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y)))) {
                    int totalLines = std::count(resultText.getString().begin(), resultText.getString().end(), '\n') + 1;
                    int totalHeight = totalLines * lineHeight - resultArea.size.y;

                    // resultScrollOffset -= static_cast<int>(event.mouseWheelScroll.delta) * lineHeight;
                    resultScrollOffset -= static_cast<int>(event.is<sf::Event::MouseWheelScrolled>()) * lineHeight;
                    
                    resultScrollOffset = std::max(0, resultScrollOffset);
                    resultScrollOffset = std::min(totalHeight, resultScrollOffset);
                }
            }

            dropdown.handleEvent(event);
            dropdown2.handleEvent(event);
            sortMethodDropdown.handleEvent(event);

            // if (event.type == sf::Event::MouseButtonPressed) {
            if (event.is<sf::Event::MouseButtonPressed>()) {
                // if (searchButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                const auto& mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
                // if (searchButton.getGlobalBounds().contains(event.mouseButtonPressed.x, event.mouseButtonPressed.y)) {
                if (searchButton.getGlobalBounds().contains(sf::Vector2f(mouseEvent->position.x, mouseEvent->position.y))) {
                    string selectedMacro1 = dropdown.getSelectedItem();
                    string selectedMacro2 = dropdown2.getSelectedItem();
                    string selectedSortMethod = sortMethodDropdown.getSelectedItem();

                    if (selectedMacro1 == selectedMacro2) {
                        resultText.setString("");
                        errorText.setString("Error: Cannot include and exclude a Macro.");
                        errorText.setPosition(sf::Vector2f(50.0 / 2.0f - errorText.getGlobalBounds().size.x / 32, 600));
                    }
                    else if (!selectedMacro1.empty() && !selectedSortMethod.empty()) {
                        vector<pair<string, float>> results;
                        errorText.setString("");

                        if (selectedSortMethod == "Heap") {
                            auto starttime = chrono::high_resolution_clock::now();
                            results = data.heapSort(selectedMacro1, selectedMacro2);
                            auto stoptime = chrono::high_resolution_clock::now();
                            int total = chrono::duration_cast<chrono::nanoseconds>(stoptime - starttime).count();
                            cout << "Heap sort runtime: " << total << " nanoseconds" << endl;
                        }
                        else if (selectedSortMethod == "Merge") {
                            auto starttime = chrono::high_resolution_clock::now();
                            results = data.mergeSort(selectedMacro1, selectedMacro2);
                            auto stoptime = chrono::high_resolution_clock::now();
                            int total = chrono::duration_cast<chrono::nanoseconds>(stoptime - starttime).count();
                            cout << "Merge sort runtime: " << total << " nanoseconds" << endl;
                        }

                        stringstream resultStream;
                        int num = 1;
                        for (const auto& pair : results) {
                            resultStream << num << ". " << pair.first << ": " << pair.second << '\n';
                            num++;
                            if (num == 32) {
                                
                                break;
                            }
                        }
                        resultText.setString(resultStream.str());
                    }
                    else if (selectedMacro1.empty()) {
                        resultText.setString("");
                        errorText.setString("Error: Please select a macro to sort by.");
                        // errorText.setPosition(sf::Vector2f(width / 2.0f - errorText.getGlobalBounds().width / 2, 700));
                        errorText.setPosition(sf::Vector2f(100.0 / 2.0f - errorText.getGlobalBounds().size.x / 64, 600));
                    }
                    else if (selectedSortMethod.empty()) {
                        resultText.setString("");
                        errorText.setString("Error: Please select a sort method.");
                        // errorText.setPosition(sf::Vector2f(width / 2.0f - errorText.getGlobalBounds().width / 2, 700));
                        errorText.setPosition(sf::Vector2f(20.0 / 2.0f - errorText.getGlobalBounds().size.x / 64, 600));
                    }
                }
                // else if (clearButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                else if (clearButton.getGlobalBounds().contains(sf::Vector2f(mouseEvent->position.x, mouseEvent->position.y))) {
                    dropdown.clear(window);
                    dropdown2.clear(window);
                    sortMethodDropdown.clear(window);
                    resultText.setString("");
                }
            }
            
            
            
            /*if (event.is<sf::Event::MouseButtonPressed>()) {
                window.close();
            }*/
            /*if (event.type == sf::Event::Closed) {
                window.close();
            }*/
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color(32, 144, 169));
        window.draw(header);
        //window.draw(names);
        window.draw(dropdownOne);
        window.draw(dropdownTwo);
        window.draw(sortMethodText);
        dropdown.draw(window);
        dropdown2.draw(window);
        sortMethodDropdown.draw(window);
        window.draw(searchButton);
        window.draw(searchButtonText);
        window.draw(clearButton);
        window.draw(clearButtonText);
        float lineHeight = resultText.getCharacterSize() + 5;
        resultTexture.clear(sf::Color::Transparent);
        resultText.setPosition(sf::Vector2f(0, -resultScrollOffset));
        resultTexture.draw(resultText);
        window.draw(errorText);
        resultTexture.display();

        sf::Sprite resultSprite(resultTexture.getTexture());
        // resultSprite.setPosition(resultArea.left, resultArea.top);
        resultSprite.setPosition(sf::Vector2f(resultArea.position.x, resultArea.position.y));
        window.draw(resultSprite);
        window.display();
    }
    return 0;
}

