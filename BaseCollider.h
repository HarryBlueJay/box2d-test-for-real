#pragma once
#include "BasicIncludes.h"
#include "DrawableObject.h"

class BaseCollider : public DrawableObject {
protected:
	//shapes//
	b2BodyId bodyId;
public:
	BaseCollider(sf::Transformable* _transform, float _parallaxFactor, b2BodyId _bodyId);
	virtual void collide(Object* otherObject, b2Vec2 normal) {};
	virtual void touch(Object* otherObject) {};
	void move();
};