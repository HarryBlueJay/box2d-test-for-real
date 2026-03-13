#pragma once
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};

class LevelPolygon : public BaseCollider {
public:
	tson::ObjectType objectType;
	bool isDoor;
	bool isKillbrick;
};


namespace Level {
	sf::Color tiledHexToSfColor(std::string color);
	void loadLevel(int levelNumber);
	void loadLevelList();
	int getCurrentLevel();
}