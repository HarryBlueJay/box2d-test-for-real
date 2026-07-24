#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class DrawableObject : public Object {
public:
	//shapes//
	sf::Transformable* transform;
	sf::Texture* texture;
	float parallaxFactor = 1.0f;

	void draw(sf::RenderWindow& window);
	~DrawableObject() {
		if (texture) {
			delete texture;
			texture = nullptr;
		}
		if (transform) {
			delete transform;
			transform = nullptr;
		}
	}
	sf::Shape* getShape() {
		return dynamic_cast<sf::Shape*>(transform);
	}
	sf::ConvexShape* getConvexShape() {
		return dynamic_cast<sf::ConvexShape*>(transform);
	}
};