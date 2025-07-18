#pragma once
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <box2d/box2d.h>
using json = nlohmann::json;

namespace Level {
	b2Vec2 spawnLocation;
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
				std::cout << spawnLocation.x << " " << spawnLocation.y << std::endl;
			}
			std::cout << layerName << std::endl;
		}
	}
}