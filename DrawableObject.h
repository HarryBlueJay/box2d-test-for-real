#pragma once
#include "BasicIncludes.h"
#include "Object.h" 
#include <string>

class DrawableObject : public Object {
protected:
	//shapes//
	sf::Transformable* transform;
	sf::Texture* texture;
	float parallaxFactor = 1.0f;
public:
	DrawableObject(sf::Transformable* _transform, float _parallaxFactor);
	void addTexture(const std::string& texturePath);
	void draw(sf::RenderWindow& window);
	~DrawableObject();
	sf::ConvexShape* getConvexShape() {
		return dynamic_cast<sf::ConvexShape*>(transform);
	}
	sf::Transformable* getTransformable() {
		return transform;
	}
};