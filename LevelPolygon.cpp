#include "LevelPolygon.h"
#include "tileson.hpp"
#include "Casts.h"

LevelPolygon::LevelPolygon(float _parallaxFactor, uint64_t collisionLayer, uint64_t layerMask, tson::Object object): BaseCollider(_parallaxFactor, collisionLayer, layerMask, object) {
	isKillbrick = object.get<bool>("isKillbrick");
	gravityStrength = object.getProperties().hasProperty("gravityStrength") ? object.get<float>("gravityStrength") : -1;
}
void LevelPolygon::update(float deltaTime) {
	if (b2Body_GetType(bodyId) == b2_staticBody) { return; }
	move();
}