#pragma once
#include "BasicIncludes.h"
#include "DrawableObject.h"
namespace tson {
	enum class ObjectType : uint8_t;
	class Object;
}

class BaseCollider : public DrawableObject {
protected:
	//shapes//
	b2BodyId bodyId;
	sf::ConvexShape convexShape;
private:
	void setupBox(sf::Vector2f size);
	BaseCollider();
public:
	BaseCollider(float _parallaxFactor, uint64_t collisionLayer, uint64_t layerMask, tson::Object object);
	virtual void collide(Object* otherObject, b2Vec2 normal) {};
	virtual void touch(Object* otherObject) {};
	void move();
	void teleport(b2Vec2 position);
	const b2BodyId& getBodyId() const;
};