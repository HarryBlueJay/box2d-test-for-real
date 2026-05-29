#pragma once
#include "Object.h"
class LevelPolygon;

class Weld : public Object {
public:
	//unsigned int objectA = 0;
	LevelPolygon* objectA;
	unsigned int objectB = 0;
	void start();
	void update(float deltaTime);
};