#pragma once
#include <SFML/Graphics.hpp>

class Ball
{
public:
	float speed = 500.f;
	float angle;
	sf::CircleShape picture;
	void initiate();
	void setSize(float radius);
	void setPosition(float x, float y);
};

