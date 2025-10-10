#pragma once
#include "BaseCollider.h"

namespace tson {
	enum class ObjectType : unsigned char;
};
enum RectangleType {
	NORMAL,
	KILLBRICK,
	DOOR
};
class LevelRectangle : public BaseCollider {
public:
	tson::ObjectType objectType;
	sf::CircleShape circle;
	sf::ConvexShape polygon;
	sf::Text text;
	RectangleType type;

	void draw(sf::RenderWindow& window) override;
};


namespace Level {
	sf::Color tiledHexToSfColor(std::string color);
	void loadLevel(int levelNumber);
	void loadLevelList();
	int getCurrentLevel();
}