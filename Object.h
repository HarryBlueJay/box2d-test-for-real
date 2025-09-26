#pragma once
#include "BasicIncludes.h"

class Object
{
public:
	//shapes//
	sf::RectangleShape rectangle;
	b2BodyId bodyId;

	virtual void update(sf::RenderWindow& window, float deltaTime) = 0;

	virtual void draw(sf::RenderWindow& window) = 0;

	virtual void collide(b2BodyId otherObject, b2Manifold manifold) = 0;
};