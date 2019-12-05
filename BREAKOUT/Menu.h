#pragma once
#include "SFML/Graphics.hpp"

#define MAX_NUMBER_OF_ITEMS 4
class Menu
{
public: 
	Menu();
	Menu(float width, float height);
	~Menu();

	void draw(sf::RenderWindow &window);

private:
	int selectedItemIndex;
	sf::Text menu[MAX_NUMBER_OF_ITEMS];
};

