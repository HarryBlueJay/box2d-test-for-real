#include "MovingPlatform.h"
#include "Casts.h"

void MovingPlatform::parse(tson::Object object) {
	startPoint = b2Body_GetPosition(bodyId);
	b2Vec2 offset = Casts::pixelsToMeters(b2Vec2{ object.get<float>("offsetX"), object.get<float>("offsetY") });
	endPoint = startPoint + offset;
	b2Body_SetType(bodyId, b2_kinematicBody);
	velocity = b2Normalize(offset) * object.get<float>("moveSpeed") * Casts::scaleFactor;
	b2Body_SetLinearVelocity(bodyId, velocity);
	waitTime = object.get<float>("waitTime");
}

void MovingPlatform::update(float deltaTime) {
	if (waitCounter > 0.0f) {
		waitCounter -= deltaTime;
		if (waitCounter <= 0.0f) {
			b2Body_SetLinearVelocity(bodyId, velocity);
		}
		return;
	}
	b2Vec2 position = b2Body_GetPosition(bodyId);
	if (b2Dot(endPoint - position, endPoint - startPoint) < 0) {
		b2Body_SetLinearVelocity(bodyId, {});
		std::swap(startPoint, endPoint);
		waitCounter = waitTime;
		velocity *= -1;
	}
	Casts::move(rectangle, bodyId);
}
