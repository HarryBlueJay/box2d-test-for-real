#pragma once
#include "Object.h"

class Weld : public Object {
public:
	unsigned int objectA = 0;
	unsigned int objectB = 0;
	void start();
};