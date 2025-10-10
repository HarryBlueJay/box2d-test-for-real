#pragma once
#include "BasicIncludes.h"

class BaseCollider {
public:
	//shapes//
	sf::RectangleShape rectangle;
	b2BodyId bodyId;

	virtual void draw(sf::RenderWindow& window) = 0;
};