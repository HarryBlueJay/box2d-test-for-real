#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class DrawableObject : public Object {
public:
	//shapes//
	sf::ConvexShape rectangle;
	sf::Texture* texture;
	float parallaxFactor;

	void draw(sf::RenderWindow& window);
	~DrawableObject() {
		if (texture) {
			delete texture;
		}
	}
};