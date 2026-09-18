#pragma once
#include "BasicIncludes.h"
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};
class LevelPolygon : public BaseCollider {
public:
	// Maybe there's a better way to do this???
	LevelPolygon(sf::Transformable* _transform, float _parallaxFactor, b2BodyId _bodyId) : BaseCollider(_transform, _parallaxFactor, _bodyId) {};
	tson::ObjectType objectType;
	int nextLevel;
	bool isKillbrick;
	float gravityStrength;
	void update(float deltaTime) override;
};