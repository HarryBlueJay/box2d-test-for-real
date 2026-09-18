#pragma once
#include "tileson.hpp"
#include "LevelPolygon.h"

class MovingPlatform : public LevelPolygon {
private:
	b2Vec2 startPoint = {};
	b2Vec2 endPoint = {};
	b2Vec2 velocity = {};
	float waitTime = 0.0f;
	float waitCounter = 0.0f;
public:
	MovingPlatform(sf::Transformable* _transform, float _parallaxFactor, b2BodyId _bodyId): LevelPolygon(_transform, _parallaxFactor, _bodyId) {};
	void parse(tson::Object object);
	void update(float deltaTime) override;
};