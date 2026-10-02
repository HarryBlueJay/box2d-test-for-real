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
	struct SetupParameters {
		uint64_t collisionLayer;
		uint64_t layerMask;
		tson::ObjectType objectType;
		sf::Vector2f position;
		float objectRotation;
		sf::Vector2f size;
		std::vector<sf::Vector2f> sfmlPoints;
		b2ShapeDef shapeDef;
	};
	void setup(const SetupParameters& params);
	BaseCollider(float _parallaxFactor, uint64_t collisionLayer, uint64_t layerMask, tson::Object object);
	BaseCollider(float _parallaxFactor);
	virtual void collide(Object* otherObject, b2Vec2 normal) {};
	virtual void touch(Object* otherObject) {};
	void move();
	void teleport(b2Vec2 position);
	const b2BodyId& getBodyId() const;
};