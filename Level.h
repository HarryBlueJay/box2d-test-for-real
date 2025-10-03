#pragma once
namespace tson {
	enum class ObjectType : unsigned char;
};
enum RectangleType {
	NORMAL,
	KILLBRICK,
	DOOR
};
struct LevelRectangle {
	tson::ObjectType objectType;
	sf::RectangleShape rectangle;
	sf::CircleShape circle;
	sf::ConvexShape polygon;
	sf::Text text;
	b2BodyId bodyId;
	RectangleType type;
};


namespace Level {
	sf::Color tiledHexToSfColor(std::string color);
	void loadLevel(int levelNumber);
	void loadLevelList();
	int getCurrentLevel();
}