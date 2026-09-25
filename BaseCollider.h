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
public:
	void setup(float _parallaxFactor, tson::ObjectType objectType, sf::Vector2f position, float objectRotation, sf::Vector2f size, std::vector<sf::Vector2f> sfmlPoints);
	void setResitution()
	BaseCollider(float _parallaxFactor, tson::Object object);
	BaseCollider(float _parallaxFactor);
	virtual void collide(Object* otherObject, b2Vec2 normal) {};
	virtual void touch(Object* otherObject) {};
	void move();
	const b2BodyId& getBodyId() const;
};