#include "BasicIncludes.h"
#include "Level.h"
#include <filesystem>
#include "tileson.hpp"
#include <fstream>
#include <cmath>
#include "Casts.h"
#include "json.hpp"
extern b2WorldId worldId;
extern b2WorldDef worldDef;
b2Vec2 spawnLocation;
std::vector<LevelRectangle> currentLevel;
int currentLevelNumber = 0;
std::vector<std::string> levelList;
sf::Color Level::tiledHexToSfColor(std::string color) {
	std::string alpha = color.substr(1, 2);
	std::string red = color.substr(3, 2);
	std::string green = color.substr(5, 2);
	std::string blue = color.substr(7, 2);

	// std::strtoul(alpha.c_str(), nullptr, 16)

	return sf::Color(
		std::strtoul(red.c_str(), nullptr, 16),
		std::strtoul(green.c_str(), nullptr, 16),
		std::strtoul(blue.c_str(), nullptr, 16),
		std::strtoul(alpha.c_str(), nullptr, 16)
	);
}
void LevelRectangle::draw(sf::RenderWindow& window) {
	window.draw(rectangle);
}
int Level::getCurrentLevel() {
	return currentLevelNumber;
}
void Level::loadLevelList() {
	std::ifstream f("levels\\levels.json");
	nlohmann::json data = nlohmann::json::parse(f);
	nlohmann::json object = data["levels"];
	for (nlohmann::json::iterator it = object.begin(); it != object.end(); ++it) {
		levelList.push_back(*it);
	}
}
void Level::loadLevel(int levelNumber) {
	currentLevelNumber = levelNumber;
	tson::Tileson t;
	std::unique_ptr<tson::Map> map = t.parse("levels\\" + levelList[levelNumber]);
	if (map->getStatus() != tson::ParseStatus::OK) {
		std::cout << "Failed to parse" << std::endl;
		return;
	}
	if (worldId.index1 > 0) {
		b2DestroyWorld(worldId);
	}
	worldId = b2CreateWorld(&worldDef);
	//SpawnLocation
	tson::Layer* spawnLocationLayer = map->getLayer("SpawnLocation");
	tson::Object* spawnLocationObject = spawnLocationLayer->firstObj("");
	tson::Vector2i spawnLocationPosition = spawnLocationObject->getPosition();
	spawnLocation.x = Casts::pixelsToMeters(spawnLocationPosition.x);
	spawnLocation.y = Casts::pixelsToMeters(spawnLocationPosition.y);

	//Collision
	tson::Layer* collisionLayer = map->getLayer("Collision");
	currentLevel.resize(collisionLayer->getObjects().size());
	for (int i = 0; i < collisionLayer->getObjects().size(); i++) {
		auto& object = collisionLayer->getObjects()[i];
		tson::Vector2i objectPosition = object.getPosition();
		tson::Vector2i objectSize = object.getSize();
		float objectRotation = object.getRotation();
		if (objectRotation != 0.0f) {
			float angleBetweenMidpoints = objectRotation * B2_PI / 180;
			sf::Vector2i offset(objectSize.x / 2.0f, objectSize.y / 2.0f);
			sf::Vector2i rotatedOffset(
				offset.x * std::cos(angleBetweenMidpoints) - offset.y * std::sin(angleBetweenMidpoints),
				offset.x * std::sin(angleBetweenMidpoints) + offset.y * std::cos(angleBetweenMidpoints)
			);
			objectPosition.x -= offset.x;
			objectPosition.y -= offset.y;
			objectPosition.x += rotatedOffset.x;
			objectPosition.y += rotatedOffset.y;
		}



		tson::Colori objectColor = object.get<tson::Colori>("color");

		RectangleType type = NORMAL;
		type = object.get<RectangleType>("type");
		tson::Property* typeProperty = object.getProp("type");
		if (typeProperty) {
		//	uint32_t typeEnum = std::any_cast<uint32_t>(typeProperty->getValue());
		//	std::cout << typeEnum << std::endl;
		}
		currentLevel[i].type = type;

		tson::ObjectType objectType = object.getObjectType();
		if (objectType == tson::ObjectType::Rectangle) {
			sf::RectangleShape rectangle;
			sf::Color rectangleColor = sf::Color::Black;
			rectangleColor = sf::Color(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
			sf::Vector2f size = sf::Vector2f(
				objectSize.x,
				objectSize.y
			);
			sf::Vector2f position = sf::Vector2f(
				objectPosition.x + (size.x / 2),
				objectPosition.y + (size.y / 2)
			);

			rectangle.setFillColor(rectangleColor);
			b2BodyId bodyId{};
			Casts::makeBox(rectangle, bodyId, b2DefaultShapeDef(), position, size, objectRotation, b2_staticBody);
			b2Body_SetUserData(bodyId, &currentLevel[i]);

			currentLevel[i].rectangle = rectangle;
			currentLevel[i].bodyId = bodyId;
		}
	}
}