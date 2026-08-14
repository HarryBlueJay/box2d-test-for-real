#pragma once
#include "Object.h"
template<typename T>
class Singleton : public Object {
public:
	static T& get() {
		static T singleton;
		return singleton;
	};
};