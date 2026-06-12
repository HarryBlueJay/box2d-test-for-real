#pragma once
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};
enum CollisionGroup: uint64_t {
	PLAYER		= 0b0001,
};

class LevelPolygon : public BaseCollider {
public:
	tson::ObjectType objectType;
	int nextLevel;
	bool isKillbrick;
};


namespace Level {
	sf::Color tiledHexToSfColor(std::string color);
	void loadLevel(int levelNumber);
	void loadLevelList();
	void completeCurrentLevel();
	int getCurrentLevel();
}