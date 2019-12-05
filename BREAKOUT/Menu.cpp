#include "Menu.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


sf::Font font2;
Menu::Menu(float width, float height) {
	
	font2.loadFromFile("consola.ttf");
	menu[0].setColor(sf::Color::White);
	menu[0].setString("Menu");
	menu[0].setFont(font2);
	menu[0].setPosition(sf::Vector2f(350, height / (MAX_NUMBER_OF_ITEMS +1) *1));

	menu[1].setFont(font2);
	menu[1].setColor(sf::Color::White);
	menu[1].setString("1 - Graj");
	menu[1].setPosition(sf::Vector2f(350, height / (MAX_NUMBER_OF_ITEMS + 1) * 2));

	menu[2].setFont(font2);
	menu[2].setColor(sf::Color::White);
	menu[2].setString("2 - Wyniki");
	menu[2].setPosition(sf::Vector2f(350, height / (MAX_NUMBER_OF_ITEMS + 1) * 3));

	menu[3].setFont(font2);
	menu[3].setColor(sf::Color::White);
	menu[3].setString("3 - Koniec");
	menu[3].setPosition(sf::Vector2f(350, height / (MAX_NUMBER_OF_ITEMS + 1) * 4));
}

Menu::~Menu() {

}

void Menu::draw(sf::RenderWindow &window) {
	for (int i = 0; i < MAX_NUMBER_OF_ITEMS; i++) {
		window.draw(menu[i]);
	}
}