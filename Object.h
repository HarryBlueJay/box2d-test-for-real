#pragma once
#include "BaseCollider.h"

class Object : public BaseCollider
{
public:
	virtual void update(sf::RenderWindow& window, float deltaTime) = 0;

	virtual void collide(b2BodyId otherObject, b2Vec2 normal) = 0;
};