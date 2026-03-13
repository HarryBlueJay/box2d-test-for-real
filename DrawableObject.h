#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class DrawableObject : public Object {
public:
	//shapes//
	sf::ConvexShape rectangle;

	void draw(sf::RenderWindow& window) {
		window.draw(rectangle);
	}
};