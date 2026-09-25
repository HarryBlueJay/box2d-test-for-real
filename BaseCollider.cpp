#include "BaseCollider.h"
#include "Casts.h"
#include "tileson.hpp"

void BaseCollider::setup(float _parallaxFactor, tson::ObjectType objectType, sf::Vector2f position, float objectRotation, sf::Vector2f size, std::vector<sf::Vector2f> sfmlPoints) {
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	switch (objectType) {
	case tson::ObjectType::Rectangle: {
		Casts::get().makeBox(*getConvexShape(), &bodyId, shapeDef, position, size, objectRotation, b2_staticBody);
		break;
	}
	case tson::ObjectType::Polygon: {
		
		Casts::get().makePolygon(*getConvexShape(), &bodyId, shapeDef, sfmlPoints, position, objectRotation, b2_staticBody);
		break;
	}
	}
    move();
}
BaseCollider::BaseCollider(float _parallaxFactor, tson::Object object) :
	BaseCollider(_parallaxFactor)
	{
	std::vector<tson::Vector2i> points = object.getPolygons();
	std::vector<sf::Vector2f> sfmlPoints;
	for (tson::Vector2i point : points) {
		sfmlPoints.push_back(
			sf::Vector2f(
				Casts::get().pixelsToMeters(point.x),
				Casts::get().pixelsToMeters(point.y)
			)
		);
	}
	float objectRotation = object.getRotation();
	tson::ObjectType objectType = object.getObjectType();
	tson::Vector2i objectPosition = object.getPosition();
	sf::Vector2f position = sf::Vector2f(
		objectPosition.x,
		objectPosition.y
	);
	tson::Vector2i objectSize = object.getSize();
	sf::Vector2f size = sf::Vector2f(
		objectSize.x,
		objectSize.y
	);
	setup(_parallaxFactor, objectType, position, objectRotation, size, sfmlPoints);
}
BaseCollider::BaseCollider(float _parallaxFactor) :
	DrawableObject(new sf::ConvexShape(), _parallaxFactor) {}
void BaseCollider::move() {
    transform->setPosition(Casts::get().b2Vec2_to_sfVector2f(b2Body_GetPosition(bodyId)));
    transform->setRotation(sf::radians(b2Rot_GetAngle(b2Body_GetRotation(bodyId))));
}
const b2BodyId& BaseCollider::getBodyId() const {
    return bodyId;
}