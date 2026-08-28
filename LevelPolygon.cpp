#include "LevelPolygon.h"
#include "Casts.h"

void LevelPolygon::update(float deltaTime) {
	if (b2Body_GetType(bodyId) == b2_staticBody) { return; }
	Casts::get().move(*transform, bodyId);
}