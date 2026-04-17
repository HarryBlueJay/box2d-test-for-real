#pragma once
#include "Level.h"
#include "tileson.hpp"

class MovingPlatform : public LevelPolygon {
private:
	b2Vec2 startPoint = {};
	b2Vec2 endPoint = {};
	b2Vec2 velocity = {};
	float waitTime = 0.0f;
	float waitCounter = 0.0f;
public:
	void parse(tson::Object object);
	void update(float deltaTime) override;
};