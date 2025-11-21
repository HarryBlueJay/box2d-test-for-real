#pragma once
#include "BasicIncludes.h"
#include "Object.h"

class BaseCollider : public Object {
public:
	//shapes//
	sf::RectangleShape rectangle;
	b2BodyId bodyId;

	virtual void collide(Object* otherObject, b2Vec2 normal) {};
};