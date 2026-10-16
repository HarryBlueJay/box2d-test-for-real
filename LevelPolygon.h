#pragma once
#include "BasicIncludes.h"
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};
class LevelPolygon : public BaseCollider {
public:
	LevelPolygon(float _parallaxFactor, uint64_t collisionLayer, uint64_t layerMask, tson::Object object);
	tson::ObjectType objectType;
	int nextLevel;
	bool isKillbrick;
	float gravityStrength;
	void update(float deltaTime) override;
};