#pragma once
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};
enum CollisionGroup: uint64_t {
	PLAYER	= 0b0001,
	LEVEL	= 0b0010,
};

class LevelPolygon : public BaseCollider {
public:
	tson::ObjectType objectType;
	int nextLevel;
	bool isKillbrick;
	float gravityStrength;
};


namespace Level {
	sf::Vector2f getTopLeft();
	sf::Vector2f getBottomRight();
	b2Vec2 rotateByGravity(const b2Vec2& vector);
	b2Vec2 unrotateByGravity(const b2Vec2& vector);
	sf::Color tiledHexToSfColor(std::string color);
	void loadLevel(int levelNumber);
	void loadLevelList();
	void completeCurrentLevel();
	int getCurrentLevel();
}