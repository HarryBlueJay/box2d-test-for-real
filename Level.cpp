#include "BasicIncludes.h"
#include "Level.h"
#include <filesystem>
#include "tileson.hpp"
#include <fstream>
#include <cmath>
#include "Casts.h"
#include "json.hpp"
#include "Player.h"
#include "Camera.h"
extern b2WorldId worldId;
extern b2WorldDef worldDef;
extern sf::RenderWindow window;
int currentLevelNumber = 0;
std::vector<std::string> levelList;
extern std::vector<Object*> objectList;
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
void updateBounds(sf::Vector2f& topLeft, sf::Vector2f& bottomRight, tson::Vector2i coordinate, sf::Vector2i rotatedOffset) {
	coordinate.x += rotatedOffset.x;
	coordinate.y += rotatedOffset.y;
	if (topLeft.x > coordinate.x) {
		topLeft.x = coordinate.x;
	}
	if (topLeft.y > coordinate.y) {
		topLeft.y = coordinate.y;
	}
	if (bottomRight.x < coordinate.x) {
		bottomRight.x = coordinate.x;
	}
	if (bottomRight.y < coordinate.y) {
		bottomRight.y = coordinate.y;
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
		for (int i = 0; i < objectList.size(); i++) {
			delete objectList[i];
		}
		b2DestroyWorld(worldId);
		objectList.clear();
	}
	worldId = b2CreateWorld(&worldDef);

	//Level bounds
	sf::Vector2f topLeft = sf::Vector2f(FLT_MAX, FLT_MAX);
	sf::Vector2f bottomRight = sf::Vector2f(FLT_MIN, FLT_MIN);

	//SpawnLocation
	b2Vec2 spawnLocation;
	tson::Layer* spawnLocationLayer = map->getLayer("SpawnLocation");
	tson::Object* spawnLocationObject = spawnLocationLayer->firstObj("");
	tson::Vector2i spawnLocationPosition = spawnLocationObject->getPosition();
	spawnLocation.x = Casts::pixelsToMeters(spawnLocationPosition.x);
	spawnLocation.y = Casts::pixelsToMeters(spawnLocationPosition.y);

	//Collision
	tson::Layer* collisionLayer = map->getLayer("Collision");
	for (int i = 0; i < collisionLayer->getObjects().size(); i++) {
		auto& object = collisionLayer->getObjects()[i];
		tson::Vector2i objectPosition = object.getPosition();
		tson::Vector2i objectSize = object.getSize();
		float objectRotation = object.getRotation();
		//Rotate (differences in the anchor point between tiled and box2d)
		float angleBetweenMidpoints = objectRotation * B2_PI / 180;
		sf::Vector2i offset(objectSize.x / 2.0f, objectSize.y / 2.0f);
		sf::Vector2i rotatedOffset(
			offset.x * std::cos(angleBetweenMidpoints) - offset.y * std::sin(angleBetweenMidpoints),
			offset.x * std::sin(angleBetweenMidpoints) + offset.y * std::cos(angleBetweenMidpoints)
		);
		objectPosition.x += rotatedOffset.x;
		objectPosition.y += rotatedOffset.y;
		updateBounds(topLeft, bottomRight, objectPosition, rotatedOffset);
		updateBounds(topLeft, bottomRight, objectPosition, -rotatedOffset);
		updateBounds(topLeft, bottomRight, objectPosition, sf::Vector2i(-rotatedOffset.x, rotatedOffset.y));
		updateBounds(topLeft, bottomRight, objectPosition, sf::Vector2i(rotatedOffset.x, -rotatedOffset.y));

		tson::Colori objectColor = object.get<tson::Colori>("color");
		LevelRectangle* levelRectangle = new LevelRectangle;
		levelRectangle->isDoor = object.get<bool>("isDoor");
		levelRectangle->isKillbrick = object.get<bool>("isKillbrick");
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
				objectPosition.x,
				objectPosition.y
			);

			rectangle.setFillColor(rectangleColor);
			b2BodyId bodyId{};
			Casts::makeBox(rectangle, bodyId, b2DefaultShapeDef(), position, size, objectRotation, b2_staticBody);
			b2Body_SetUserData(bodyId, reinterpret_cast<void*>(i));

			levelRectangle->rectangle = rectangle;
			levelRectangle->bodyId = bodyId;
		}
		objectList.push_back(levelRectangle);
	}
	topLeft = sf::Vector2f(
		Casts::pixelsToMeters(topLeft.x),
		Casts::pixelsToMeters(topLeft.y)
	);
	bottomRight = sf::Vector2f(
		Casts::pixelsToMeters(bottomRight.x),
		Casts::pixelsToMeters(bottomRight.y)
	);
	Player* player = new Player(spawnLocation);
	Camera* camera = new Camera(window);
	objectList.push_back(camera);
	camera->setBounds(topLeft, bottomRight);
	camera->setTarget(player);
	objectList.push_back(player);
}