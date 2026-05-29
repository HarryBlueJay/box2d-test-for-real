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
#include "DrawableObject.h"
#include "MovingPlatform.h"
#include "TextObject.h"
#include "Weld.h"
extern b2WorldId worldId;
extern b2WorldDef worldDef;
extern sf::RenderWindow window;
int currentLevelNumber = 0;
sf::Font font = sf::Font("resources\\sansation.ttf");
std::vector<std::string> levelList;
std::vector<bool> levelCompletions;
b2Vec2 spawnLocation;
extern std::vector<Object*> objectList;
std::vector<b2BodyId> objectIds;
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
int Level::getCurrentLevel() {
	return currentLevelNumber;
}
void Level::loadLevelList() {
	std::ifstream levelListJSON("levels\\levels.json");
	nlohmann::json data = nlohmann::json::parse(levelListJSON);
	levelListJSON.close();
	nlohmann::json object = data["levels"];
	for (nlohmann::json::iterator it = object.begin(); it != object.end(); ++it) {
		levelList.push_back(*it);
		levelCompletions.push_back(false);
	}
	std::ifstream completedLevelList("levels\\completedLevels.txt");
	if (!completedLevelList.fail()) {
		std::string line;
		int index = 0;
		while (std::getline(completedLevelList, line)) {
			if (index == 0) {
				currentLevelNumber = std::stoi(line);
			} else {
				levelCompletions[index] = line == "1";
			}
			index++;
		}
		completedLevelList.close();
	}
}
void Level::completeCurrentLevel() {
	levelCompletions[currentLevelNumber] = true;
	std::ofstream completedLevelList("levels\\completedLevels.txt");
	if (!completedLevelList.fail()) {
		for (int i = 0; i < levelCompletions.size(); i++) {
			if (i == 0) {
				completedLevelList << currentLevelNumber << "\n";
				continue;
			}
			completedLevelList << levelCompletions[i] << "\n";
		}
		completedLevelList.close();
	}
}
static void updateBounds(sf::Vector2f& topLeft, sf::Vector2f& bottomRight, sf::Vector2f coordinate) {
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
static void loadPolygon(tson::Object& object, DrawableObject* levelPolygon, b2BodyId* bodyId = nullptr, b2ShapeDef shapeDef = b2DefaultShapeDef()) {
	tson::Vector2i objectPosition = object.getPosition();
	float objectRotation = object.getRotation();
	tson::Colori objectColor = object.get<tson::Colori>("color");
	std::string texturePath = object.get<std::string>("texture");
	unsigned int weldNumber = object.get<unsigned int>("weld");
	if (texturePath != "") {
		levelPolygon->texture = new sf::Texture(texturePath);
		levelPolygon->texture->setSmooth(false);
		levelPolygon->rectangle.setTexture(levelPolygon->texture);
	}
	sf::Color polygonColor = sf::Color::Black;
	polygonColor = sf::Color(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
	levelPolygon->rectangle.setFillColor(polygonColor);

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
		Casts::makeBox(levelPolygon->rectangle, bodyId, shapeDef, position, size, objectRotation, b2_staticBody);
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
		Casts::makePolygon(levelPolygon->rectangle, bodyId, shapeDef, sfmlPoints, position, objectRotation, b2_staticBody);
		break;
	}
	}
	if (bodyId == nullptr) {
		levelPolygon->rectangle.setPosition(Casts::pixelsToMeters(position));
		levelPolygon->rectangle.setRotation(sf::degrees(objectRotation));
	}
	else {
		if (object.getId() >= objectIds.size()) {
			objectIds.resize(object.getId() + 1);
		}
		objectIds[object.getId()] = *bodyId;
		if (weldNumber > 0) {
			Weld* weld = new Weld();
			weld->objectA = dynamic_cast<LevelPolygon*>(levelPolygon);
			weld->objectB = weldNumber;
			objectList.push_back(weld);
		}
	}
	objectList.push_back(levelPolygon);
}
static Object* loadObject(tson::Object& object, b2BodyId* bodyId = nullptr) {
	tson::ObjectType objectType = object.getObjectType();
	switch (objectType) {
	case tson::ObjectType::Rectangle:
	case tson::ObjectType::Polygon: {
		if (bodyId) {
			LevelPolygon* levelPolygon = nullptr;
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.filter.categoryBits = LEVEL;
			shapeDef.filter.maskBits = PLAYER;
			if (object.get<float>("moveSpeed") != 0 || object.get<float>("spinSpeed") != 0) {
				levelPolygon = new MovingPlatform;
			}
			else {
				levelPolygon = new LevelPolygon;
			}
			// default is exactly zero
			if (float friction = object.get<float>("friction") > 0) {
				shapeDef.material.friction = friction;
			}
			shapeDef.material.restitution = object.get<float>("restitution");
			shapeDef.material.tangentSpeed = object.get<float>("tangentSpeed");
			loadPolygon(object, levelPolygon, bodyId, shapeDef);
			b2Body_SetUserData(*bodyId, reinterpret_cast<void*>(objectList.size() - 1));
			if (object.getProperties().hasProperty("nextLevel")) {
				levelPolygon->nextLevel = object.get<int>("nextLevel");
				if (levelPolygon->nextLevel > 0) {
					if (levelPolygon->nextLevel == currentLevelNumber) {
						spawnLocation = Casts::sfVector2f_to_b2Vec2((levelPolygon->rectangle.getPosition() / Casts::scaleFactor) + sf::Vector2f(96, 192));
					}
					TextObject* text = new TextObject(font);
					text->text.setString(" " + std::to_string(levelPolygon->nextLevel));
					text->text.setCharacterSize(60);
					text->text.setScale(sf::Vector2f(Casts::scaleFactor, Casts::scaleFactor));
					//text->text.setOrigin(text->text.getLocalBounds().size * 0.5f + sf::Vector2f(0, 30));
					text->text.setPosition(levelPolygon->rectangle.getPosition());
					text->text.setRotation(levelPolygon->rectangle.getRotation());
					text->text.setOutlineThickness(1.0f);
					if (!levelCompletions[levelPolygon->nextLevel]) {
						text->text.setFillColor(sf::Color::Red);
					}
					objectList.push_back(text);
				}
			}
			else {
				levelPolygon->nextLevel = -1;
			}
			levelPolygon->isKillbrick = object.get<bool>("isKillbrick");

			levelPolygon->bodyId = *bodyId;
			if (MovingPlatform* platform = dynamic_cast<MovingPlatform*>(levelPolygon)) {
				platform->parse(object);
			}
			return levelPolygon;
		}
		DrawableObject* drawableObject = new DrawableObject;
		loadPolygon(object, drawableObject);
		return drawableObject;
	}
	case tson::ObjectType::Text: {		
		TextObject* textObject = new TextObject(font);
		tson::Text text = object.getText();
		//textObject->text.setFont(font);
		textObject->text.setString(text.text);
		textObject->text.setCharacterSize(text.pixelSize);
		unsigned int style = sf::Text::Regular;
		if (text.bold) {
			style |= sf::Text::Bold;
		}
		if (text.italic) {
			style |= sf::Text::Italic;
		}
		if (text.strikeout) {
			style |= sf::Text::StrikeThrough;
		}
		if (text.underline) {
			style |= sf::Text::Underlined;
		}
		textObject->text.setStyle(style);
		tson::Vector2i objectPosition = object.getPosition();
		sf::Vector2f position = sf::Vector2f(
			objectPosition.x,
			objectPosition.y
		);
		textObject->text.setPosition(Casts::pixelsToMeters(position));
		sf::Color polygonColor = sf::Color(text.color.r, text.color.g, text.color.b, text.color.a);
		textObject->text.setScale(sf::Vector2f(Casts::scaleFactor, Casts::scaleFactor));
		textObject->text.setFillColor(polygonColor);
		objectList.push_back(textObject);
		return textObject;
	}
	}
	return nullptr;
}
void Level::loadLevel(int levelNumber) {
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

	auto parseLayer = [&topLeft, &bottomRight, map = std::move(map)](std::string layerName, bool hasCollision) {
		tson::Layer* collisionLayer = map->getLayer("Collision");
		for (int i = 0; i < collisionLayer->getObjects().size(); i++) {
			auto& object = collisionLayer->getObjects()[i];
			b2BodyId bodyId;
			LevelPolygon* levelPolygon = dynamic_cast<LevelPolygon*>(loadObject(object, &bodyId));
			for (int i = 0; i < levelPolygon->rectangle.getPointCount(); i++) {
				sf::FloatRect rect = levelPolygon->rectangle.getGlobalBounds();
				updateBounds(topLeft, bottomRight, sf::Vector2f(rect.position.x, rect.position.y));
				updateBounds(topLeft, bottomRight, sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y));
			}
		}
		};

	//SpawnLocation
	tson::Layer* spawnLocationLayer = map->getLayer("SpawnLocation");
	tson::Object* spawnLocationObject = spawnLocationLayer->firstObj("");
	tson::Vector2i spawnLocationPosition = spawnLocationObject->getPosition();
	spawnLocation.x = spawnLocationPosition.x;
	spawnLocation.y = spawnLocationPosition.y;

	//Background
	tson::Layer* backgroundLayer = map->getLayer("Background");
	if (backgroundLayer) {
		for (int i = 0; i < backgroundLayer->getObjects().size(); i++) {
			loadObject(backgroundLayer->getObjects()[i]);
		}
	}
	//Collision
	tson::Layer* collisionLayer = map->getLayer("Collision");
	for (int i = 0; i < collisionLayer->getObjects().size(); i++) {
		auto& object = collisionLayer->getObjects()[i];
		b2BodyId bodyId;
		LevelPolygon* levelPolygon = dynamic_cast<LevelPolygon*>(loadObject(object, &bodyId));
		for (int i = 0; i < levelPolygon->rectangle.getPointCount(); i++) {
			sf::FloatRect rect = levelPolygon->rectangle.getGlobalBounds();
			updateBounds(topLeft, bottomRight, sf::Vector2f(rect.position.x, rect.position.y));
			updateBounds(topLeft, bottomRight, sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y));
		}
	}
	Player* player = new Player(spawnLocation);
	Camera* camera = new Camera(window);
	objectList.push_back(camera);
	camera->setBounds(topLeft, bottomRight);
	camera->setTarget(player);
	objectList.push_back(player);
	//Foreground
	tson::Layer* foregroundLayer = map->getLayer("Foreground");
	if (foregroundLayer) {
		for (int i = 0; i < foregroundLayer->getObjects().size(); i++) {
			loadObject(foregroundLayer->getObjects()[i]);
		}
	}
	for (int i = 0; i < objectList.size(); i++) {
		objectList[i]->start();
	}
	currentLevelNumber = levelNumber;
}