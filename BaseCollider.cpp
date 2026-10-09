#include "BaseCollider.h"
#include "Casts.h"
#include "tileson.hpp"
extern b2WorldId worldId;

void BaseCollider::setupBox(sf::Vector2f size) {
	getConvexShape()->setPointCount(4);
	getConvexShape()->setPoint(0, sf::Vector2f());
	getConvexShape()->setPoint(1, sf::Vector2f(size.x, 0));
	getConvexShape()->setPoint(2, size);
	getConvexShape()->setPoint(3, sf::Vector2f(0, size.y));
}
BaseCollider::BaseCollider(float _parallaxFactor, uint64_t collisionLayer, uint64_t layerMask, tson::Object object) :
	DrawableObject(new sf::ConvexShape(), _parallaxFactor)
	{
	b2ShapeDef shapeDef = b2DefaultShapeDef(); // possibly required
	float objectRotation = object.getRotation();
	tson::ObjectType objectType = object.getObjectType();
	tson::Vector2i objectPosition = object.getPosition();
	sf::Vector2f position = Casts::get().pixelsToMeters(sf::Vector2f(
		objectPosition.x,
		objectPosition.y
	));
	float radius = object.get<float>("radius");
	std::vector<sf::Vector2f> sfmlPoints = {};
	switch (objectType) {
	case tson::ObjectType::Rectangle: {
		tson::Vector2i objectSize = object.getSize();
		sf::Vector2f size = sf::Vector2f(
			objectSize.x,
			objectSize.y
		);
		setupBox(size);
		for (int i = 0; i < 4; i++) {
			sfmlPoints.push_back(getConvexShape()->getPoint(i));
		}
		break;
	}
	case tson::ObjectType::Polygon: {
		std::vector<tson::Vector2i> points = object.getPolygons();
		for (tson::Vector2i point : points) {
			sfmlPoints.push_back(
				sf::Vector2f(
					Casts::get().pixelsToMeters(point.x),
					Casts::get().pixelsToMeters(point.y)
				)
			);
		}
		break;
	}
	case tson::ObjectType::Point: {
		position.x -= radius;
		const float edgeSize = 0.01f;
		setupBox(sf::Vector2f(radius, radius));
		for (int i = 0; i < 4; i++) {
			sfmlPoints.push_back({ radius / 2.0f, radius / 2.0f });
			if (i < 2) {
				sfmlPoints[i].x -= edgeSize;
			}
			else {
				sfmlPoints[i].x += edgeSize;
			}
			if (i % 3 == 0) {
				sfmlPoints[i].y += edgeSize;
			}
			else {
				sfmlPoints[i].y -= edgeSize;
			}
		}
	}
	}
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_staticBody;
	b2Hull hull = b2ComputeHull(reinterpret_cast<const b2Vec2*>(&sfmlPoints[0]), sfmlPoints.size());
	b2Polygon polygon = b2MakePolygon(&hull, radius);
	//setupPolygon
	bodyDef.position = Casts::get().sfVector2f_to_b2Vec2(position);
	bodyDef.rotation = b2MakeRot(objectRotation * B2_PI / 180);
	bodyId = b2CreateBody(worldId, &bodyDef);
	b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
	move();
}
void BaseCollider::move() {
    transform->setPosition(Casts::get().b2Vec2_to_sfVector2f(b2Body_GetPosition(bodyId)));
    transform->setRotation(sf::radians(b2Rot_GetAngle(b2Body_GetRotation(bodyId))));
}
void BaseCollider::teleport(b2Vec2 position) {
	b2Body_SetTransform(bodyId, position, b2Body_GetRotation(bodyId));
	move();
}
const b2BodyId& BaseCollider::getBodyId() const {
    return bodyId;
}