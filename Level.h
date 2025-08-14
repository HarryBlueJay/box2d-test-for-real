#pragma once
#include "json.hpp"
#include <filesystem>
#include "tileson.hpp"
#include <iostream>
#include <fstream>
#include <box2d/box2d.h>
using json = nlohmann::json;
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
	b2Vec2 spawnLocation;
	std::vector<LevelRectangle> currentLevel;
	sf::Color tiledHexToSfColor(std::string color) {
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
	void loadLevel(std::string level) {
		tson::Tileson t;
		std::unique_ptr<tson::Map> map = t.parse("levels\\" + level);
		if (map->getStatus() != tson::ParseStatus::OK) {
			std::cout << "Failed to parse" << std::endl;
			return;
		}
		//SpawnLocation
		tson::Layer* spawnLocationLayer = map->getLayer("SpawnLocation");
		tson::Object* spawnLocationObject = spawnLocationLayer->firstObj("");
		tson::Vector2i spawnLocationPosition = spawnLocationObject->getPosition();
		spawnLocation.x = pixelsToMeters(spawnLocationPosition.x);
		spawnLocation.y = pixelsToMeters(spawnLocationPosition.y);

		//Collision
		tson::Layer* collisionLayer = map->getLayer("Collision");
		for (auto& object : collisionLayer->getObjects()) {

			tson::Vector2i objectPosition = object.getPosition();
			tson::Vector2i objectSize = object.getSize();
			tson::Colori objectColor = object.get<tson::Colori>("color");

			RectangleType type = NORMAL;
			type = object.get<RectangleType>("type");
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
				makeBox(rectangle, bodyId, b2DefaultShapeDef(), position, size, b2_staticBody);

				LevelRectangle actualRectangle;
				actualRectangle.rectangle = rectangle;
				actualRectangle.bodyId = bodyId;
				actualRectangle.type = type;
				currentLevel.push_back(actualRectangle);
			}

			//object.getObjectType();
			//tson::ObjectType::
			

			
		}
		std::cout << currentLevel.size() << std::endl;
	}
}