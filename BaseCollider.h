#pragma once
#include "BasicIncludes.h"
#include "DrawableObject.h"

class BaseCollider : public DrawableObject {
public:
	//shapes//
	b2BodyId bodyId;

	virtual void collide(Object* otherObject, b2Vec2 normal) {};
	virtual void touch(Object* otherObject) {};
};