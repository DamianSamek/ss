#include "Menu.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <mysql.h>
#include "HighScore.h"

using namespace std;
using namespace sf;

sf::Font font5;

HighScore::~HighScore() {
	
}

HighScore::HighScore() {
	font5.loadFromFile("consola.ttf");
}

void HighScore::draw(RenderWindow& window, string highscores[]) {

	Text text;
	text.setFont(font5);
	text.setColor(Color::White);
	text.setString("Najlepsze wyniki");
	text.setPosition(Vector2f(300, 200));
	window.draw(text);
	for (int i = 1; i < MAX_NUMBER_OF_ITEMS; i++) {
		Text text;
		text.setFont(font5);
		text.setColor(Color::White);
		text.setString(highscores[i-1]);
		text.setPosition(Vector2f(window.getSize().x / 2, 200 + 50* i));
		window.draw(text);
	}
}