#pragma once
#include "BasicIncludes.h"
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};
class LevelPolygon : public BaseCollider {
public:
	using BaseCollider::BaseCollider;
	tson::ObjectType objectType;
	int nextLevel;
	bool isKillbrick;
	float gravityStrength;
	void update(float deltaTime) override;
};