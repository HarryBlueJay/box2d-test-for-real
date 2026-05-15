#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class DrawableObject : public Object {
public:
	//shapes//
	sf::ConvexShape rectangle;
	sf::Texture* texture;

	void draw(sf::RenderWindow& window) {
		window.draw(rectangle);
	}
	~DrawableObject() {
		if (texture) {
			delete texture;
		}
	}
};