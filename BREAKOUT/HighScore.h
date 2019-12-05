#pragma once
#include "SFML/Graphics.hpp"

using namespace std;

#define MAX_NUMBER_OF_ITEMS 6
class HighScore
{
public:
	HighScore();
	HighScore(float width, float height);
	~HighScore();

	void draw(sf::RenderWindow& window, string highscores[]);

private:
	sf::Text highscore[MAX_NUMBER_OF_ITEMS];
};

