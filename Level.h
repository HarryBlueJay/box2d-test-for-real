#pragma once
#include "json.hpp"
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
	sf::RectangleShape rectangle;
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
		std::ifstream f("levels\\" + level);
		json data = json::parse(f);
		json layers = data["layers"];
		for (json::iterator layer = layers.begin(); layer != layers.end(); ++layer) {
			std::string layerName = (*layer)["name"];
			if (layerName == "SpawnLocation") {
				json spawnLocationJSON = (*layer)["objects"][0];
				spawnLocation.x = pixelsToMeters(spawnLocationJSON["x"]);
				spawnLocation.y = pixelsToMeters(spawnLocationJSON["y"]);
			}
			if (layerName == "Collision") {
				json objects = (*layer)["objects"];
				for (json::iterator object = objects.begin(); object != objects.end(); ++object) {
					sf::RectangleShape rectangle;
					sf::Color rectangleColor = sf::Color::Black;
					sf::Vector2f size = sf::Vector2f(
						(*object)["width"],
						(*object)["height"]
					);
					sf::Vector2f position = sf::Vector2f(
						(*object)["x"] + (size.x / 2),
						(*object)["y"] + (size.y / 2)
					);
					b2BodyId bodyId{};
					makeBox(rectangle, bodyId, b2DefaultShapeDef(), position, size, b2_staticBody);
					RectangleType type = NORMAL;
					json properties = (*object)["properties"];
					for (json::iterator property = properties.begin(); property != properties.end(); ++property) {
						std::string name = (*property)["name"];
						//if (name == "r") {
						//	r = (*it2)["value"];
						//}
						//else if (name == "g") {
						//	g = (*it2)["value"];
						//}
						//else if (name == "b") {
						//	b = (*it2)["value"];
						//}
						if (name == "type") {
							type = (*property)["value"];
						}
						else if (name == "color") {
							rectangleColor = tiledHexToSfColor((*property)["value"]);
						}
					}
					//rectangle.setFillColor(sf::Color(r, g, b, a));
					rectangle.setFillColor(rectangleColor);
					LevelRectangle actualRectangle;
					actualRectangle.rectangle = rectangle;
					actualRectangle.bodyId = bodyId;
					actualRectangle.type = type;
					currentLevel.push_back(actualRectangle);
				}
			}
			std::cout << layerName << std::endl;
		}
	}
}