#pragma once
#include "BaseCollider.h"

namespace Level {
	sf::Vector2f getTopLeft();
	sf::Vector2f getBottomRight();
	b2Vec2 rotateByGravity(const b2Vec2& vector);
	b2Vec2 unrotateByGravity(const b2Vec2& vector);
	sf::Color tiledHexToSfColor(std::string color);
	void loadLevel(int levelNumber);
	bool setupLevels();
	void completeCurrentLevel();
	int getCurrentLevel();
}