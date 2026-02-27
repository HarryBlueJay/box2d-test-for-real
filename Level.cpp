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
void LevelPolygon::draw(sf::RenderWindow& window) {
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
void updateBounds(sf::Vector2f& topLeft, sf::Vector2f& bottomRight, sf::Vector2f coordinate) {
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
	//spawnLocation.x = Casts::pixelsToMeters(spawnLocationPosition.x);
	//spawnLocation.y = Casts::pixelsToMeters(spawnLocationPosition.y);
	spawnLocation.x = spawnLocationPosition.x;
	spawnLocation.y = spawnLocationPosition.y;

	//Collision
	tson::Layer* collisionLayer = map->getLayer("Collision");
	for (int i = 0; i < collisionLayer->getObjects().size(); i++) {
		auto& object = collisionLayer->getObjects()[i];
		tson::Vector2i objectPosition = object.getPosition();
		float objectRotation = object.getRotation();
		tson::Colori objectColor = object.get<tson::Colori>("color");
		sf::Color polygonColor = sf::Color::Black;
		polygonColor = sf::Color(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
		LevelPolygon* levelPolygon = new LevelPolygon;
		levelPolygon->rectangle.setFillColor(polygonColor);
		levelPolygon->isDoor = object.get<bool>("isDoor");
		levelPolygon->isKillbrick = object.get<bool>("isKillbrick");
		b2BodyId bodyId{};
		
		tson::ObjectType objectType = object.getObjectType();
		sf::Vector2f position = sf::Vector2f(
			objectPosition.x,
			objectPosition.y
		);
		switch (objectType) {
		case tson::ObjectType::Rectangle: {
			tson::Vector2i objectSize = object.getSize();
			sf::Vector2f size = sf::Vector2f(
				objectSize.x,
				objectSize.y
			);
			Casts::makeBox(levelPolygon->rectangle, bodyId, b2DefaultShapeDef(), position, size, objectRotation, b2_staticBody);
			break;
		}
		case tson::ObjectType::Polygon: {
			std::vector<tson::Vector2i> points = object.getPolygons();
			std::vector<sf::Vector2f> sfmlPoints;
			for (tson::Vector2i point : points) {
				sfmlPoints.push_back(
					sf::Vector2f(
						Casts::pixelsToMeters(point.x), 
						Casts::pixelsToMeters(point.y)
					)
				);
			}
			Casts::makePolygon(levelPolygon->rectangle, bodyId, b2DefaultShapeDef(), sfmlPoints, position, objectRotation, b2_staticBody);
			break;
		}
		}
		b2Body_SetUserData(bodyId, reinterpret_cast<void*>(i));
		levelPolygon->bodyId = bodyId;
		for (int i = 0; i < levelPolygon->rectangle.getPointCount(); i++) {
			sf::FloatRect rect = levelPolygon->rectangle.getGlobalBounds();
			updateBounds(topLeft, bottomRight, sf::Vector2f(rect.position.x, rect.position.y));
			updateBounds(topLeft, bottomRight, sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y));
		}
		objectList.push_back(levelPolygon);
	}
	Player* player = new Player(spawnLocation);
	Camera* camera = new Camera(window);
	objectList.push_back(camera);
	camera->setBounds(topLeft, bottomRight);
	camera->setTarget(player);
	objectList.push_back(player);
}