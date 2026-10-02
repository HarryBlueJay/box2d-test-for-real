#include "BaseCollider.h"
#include "Casts.h"
#include "tileson.hpp"

void BaseCollider::setup(const SetupParameters& params) {
	switch (params.objectType) {
	case tson::ObjectType::Rectangle: {
		for (int i = 0; i < box.getPointCount(); i++) {
			offsets[i] = sfVector2f_to_b2Vec2(box.getPoint(i));
		}
		Casts::get().makeBox(*getConvexShape(), &bodyId, params.shapeDef, params.position, params.size, params.objectRotation, b2_staticBody);
		break;
	}
	case tson::ObjectType::Polygon: {
		Casts::get().makePolygon(*getConvexShape(), &bodyId, params.shapeDef, params.sfmlPoints, params.position, params.objectRotation, b2_staticBody);
		break;
	}
	case tson::ObjectType::Point: {
		Casts::get().makeCircleWithBodyDef(*getConvexShape(), bodyId, params.shapeDef, params.position, params.size, 0, bodyDef);
	}
	}
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_staticBody;
	b2Hull hull = b2ComputeHull(reinterpret_cast<const b2Vec2*>(&params.sfmlPoints[0]), params.sfmlPoints.size());
	b2Polygon polygon = b2MakePolygon(&hull, 0);
	setupPolygon(polygon, *id, shapeDef, shape, position, rotation, bodyDef);
    move();
}
BaseCollider::BaseCollider(float _parallaxFactor, uint64_t collisionLayer, uint64_t layerMask, tson::Object object) :
	BaseCollider(_parallaxFactor)
	{
	SetupParameters params;
	params.shapeDef = b2DefaultShapeDef(); // possibly required
	std::vector<tson::Vector2i> points = object.getPolygons();
	for (tson::Vector2i point : points) {
		params.sfmlPoints.push_back(
			sf::Vector2f(
				Casts::get().pixelsToMeters(point.x),
				Casts::get().pixelsToMeters(point.y)
			)
		);
	}
	params.objectRotation = object.getRotation();
	params.objectType = object.getObjectType();
	tson::Vector2i objectPosition = object.getPosition();
	params.position = sf::Vector2f(
		objectPosition.x,
		objectPosition.y
	);
	tson::Vector2i objectSize = object.getSize();
	params.size = sf::Vector2f(
		objectSize.x,
		objectSize.y
	);
	setup(params);
}
BaseCollider::BaseCollider(float _parallaxFactor) :
	DrawableObject(new sf::ConvexShape(), _parallaxFactor) {}
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