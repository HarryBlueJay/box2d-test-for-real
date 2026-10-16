#include "BaseCollider.h"
#include "Casts.h"
#include "tileson.hpp"
#include "CollisionGroup.h"
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

	// Properties
	if (object.getProperties().hasProperty("friction")) {
		float friction = object.get<float>("friction");
		shapeDef.material.friction = friction;
	}
	if (object.getProperties().hasProperty("density")) {
		float density = object.get<float>("density");
		shapeDef.density = density;
	}
	shapeDef.material.restitution = object.get<float>("restitution");
	shapeDef.material.tangentSpeed = object.get<float>("tangentSpeed");
	if (object.get<bool>("sensor")) {
		shapeDef.isSensor = true;
		shapeDef.filter.categoryBits = SENSOR;
		shapeDef.enableSensorEvents = true;
	}
	else {
		shapeDef.enableContactEvents = true;
	}
	if (object.get<bool>("trigger")) {
		shapeDef.enableSensorEvents = true;
		shapeDef.enableContactEvents = true;
	}

	float objectRotation = object.getRotation();
	tson::ObjectType objectType = object.getObjectType();
	tson::Vector2i objectPosition = object.getPosition();
	sf::Vector2f position = Casts::get().pixelsToMeters(sf::Vector2f(
		objectPosition.x,
		objectPosition.y
	));
	float radius = object.get<float>("radius");
	std::vector<sf::Vector2f> b2Points = {};
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = object.get<bool>("dynamic") ? b2_dynamicBody : b2_staticBody;
	switch (objectType) {
	case tson::ObjectType::Rectangle: {
		tson::Vector2i objectSize = object.getSize();
		sf::Vector2f size = sf::Vector2f(
			objectSize.x,
			objectSize.y
		);
		setupBox(size * Casts::get().scaleFactor);
		for (int i = 0; i < 4; i++) {
			b2Points.push_back(getConvexShape()->getPoint(i));
		}
		break;
	}
	case tson::ObjectType::Polygon: {
		std::vector<tson::Vector2i> points = object.getPolygons();
		for (tson::Vector2i point : points) {
			b2Points.push_back(
				sf::Vector2f(
					Casts::get().pixelsToMeters(point.x),
					Casts::get().pixelsToMeters(point.y)
				)
			);
		}
		break;
	}
	case tson::ObjectType::Point: {
		bodyDef.fixedRotation = true; // Maybe change it in the future
		float b2Radius = radius / Casts::get().scaleFactor;
		position.x -= radius;
		const float edgeSize = 0.01f;
		setupBox(sf::Vector2f(radius * 2.0f, radius * 2.0f));
		for (int i = 0; i < 4; i++) {
			b2Points.push_back({ radius, radius });
			if (i < 2) {
				b2Points[i].x -= edgeSize;
			}
			else {
				b2Points[i].x += edgeSize;
			}
			if (i % 3 == 0) {
				b2Points[i].y += edgeSize;
			}
			else {
				b2Points[i].y -= edgeSize;
			}
		}
	}
	}
	b2Hull hull = b2ComputeHull(reinterpret_cast<const b2Vec2*>(&b2Points[0]), b2Points.size());
	b2Polygon polygon = b2MakePolygon(&hull, radius);
	//setupPolygon
	bodyDef.position = Casts::get().sfVector2f_to_b2Vec2(position);
	bodyDef.rotation = b2MakeRot(objectRotation * B2_PI / 180);
	bodyId = b2CreateBody(worldId, &bodyDef);
	b2Body_EnableSleep(bodyId, false); // Gravity shifting doesn't wake
	b2CreatePolygonShape(bodyId, &shapeDef, &polygon);

	// Setup visuals
	tson::Colori objectColor = object.get<tson::Colori>("color");
	std::string texturePath = object.get<std::string>("texture");
	if (texturePath != "") {
		addTexture(texturePath);
	}
	sf::Color polygonColor = sf::Color::Black;
	polygonColor = sf::Color(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
	getConvexShape()->setFillColor(polygonColor);

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