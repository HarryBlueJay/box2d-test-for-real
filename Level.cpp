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
#include "LevelPolygon.h"
#include "TextObject.h"
#include "Weld.h"
extern b2WorldId worldId;
extern b2WorldDef worldDef;
extern sf::RenderWindow window;
int currentLevelNumber = 0;
sf::Font font = sf::Font("resources\\sansation.ttf");
std::vector<std::string> levelList;
std::vector<bool> levelCompletions;
extern std::vector<Object*> objectList;
std::vector<b2BodyId> objectIds;
sf::Vector2f topLeft;
sf::Vector2f bottomRight;
sf::Vector2f Level::getTopLeft() {
	return topLeft;
}
sf::Vector2f Level::getBottomRight() {
	return bottomRight;
}
b2Vec2 Level::rotateByGravity(const b2Vec2& vector) {
	if (vector == b2Vec2_zero) { return vector; }
	b2Rot gravityAngle = b2ComputeRotationBetweenUnitVectors(b2Vec2{ 0, 1 }, b2Normalize(b2World_GetGravity(worldId)));
	return b2RotateVector(gravityAngle, vector);
}
b2Vec2 Level::unrotateByGravity(const b2Vec2& vector) {
	if (vector == b2Vec2_zero) { return vector; }
	b2Rot gravityAngle = b2ComputeRotationBetweenUnitVectors(b2Vec2{ 0, 1 }, b2Normalize(b2World_GetGravity(worldId)));
	return b2InvRotateVector(gravityAngle, vector);
}
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
	std::ifstream levelListJSON("resources\\levels\\levels.json");
	nlohmann::json data = nlohmann::json::parse(levelListJSON);
	levelListJSON.close();
	nlohmann::json object = data["levels"];
	for (nlohmann::json::iterator it = object.begin(); it != object.end(); ++it) {
		levelList.push_back(*it);
		levelCompletions.push_back(false);
	}
	std::ifstream completedLevelList("resources\\levels\\completedLevels.txt");
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
	std::ofstream completedLevelList("resources\\levels\\completedLevels.txt");
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
static void updateBounds(sf::Vector2f coordinate) {
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
static void loadPolygon(tson::Object& object, DrawableObject* levelPolygon, b2BodyId& bodyId, b2ShapeDef shapeDef) {

	tson::Colori objectColor = object.get<tson::Colori>("color");
	std::string texturePath = object.get<std::string>("texture");
	if (texturePath != "") {
		levelPolygon->addTexture(texturePath);
	}
	sf::Color polygonColor = sf::Color::Black;
	polygonColor = sf::Color(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
	levelPolygon->getConvexShape()->setFillColor(polygonColor);
	
	if (object.getId() >= objectIds.size()) {
		objectIds.resize(object.getId() + 1);
	}
	objectIds[object.getId()] = bodyId;
	objectList.push_back(levelPolygon);
}
static Object* loadObject(tson::Object& object, b2BodyId& bodyId, uint64_t layerMask, uint64_t hitsPlayer) {
	tson::ObjectType objectType = object.getObjectType();
	switch (objectType) {
	case tson::ObjectType::Rectangle:
	case tson::ObjectType::Polygon: {
		LevelPolygon* levelPolygon = nullptr;
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.filter.categoryBits = layerMask;
		shapeDef.filter.maskBits = layerMask | hitsPlayer;
		if (object.get<float>("moveSpeed") != 0 || object.get<float>("spinSpeed") != 0) {
			levelPolygon = new MovingPlatform;
		}
		else {
			levelPolygon = new LevelPolygon;
		}
		unsigned int weldNumber = object.get<unsigned int>("weld");
		if (weldNumber > 0) {
			Weld* weld = new Weld();
			weld->objectA = levelPolygon;
			weld->objectB = weldNumber;
			shapeDef.filter.maskBits = hitsPlayer;
			objectList.push_back(weld);
		}
		// default is exactly zero
		if (float friction = object.get<float>("friction"); friction > 0) {
			shapeDef.material.friction = friction;
		}
		if (float density = object.get<float>("density"); density > 0) {
			shapeDef.density = density;
		}
		shapeDef.material.restitution = object.get<float>("restitution");
		shapeDef.material.tangentSpeed = object.get<float>("tangentSpeed");
		if (object.get<bool>("sensor")) {
			shapeDef.isSensor = true;
			shapeDef.filter.categoryBits = SENSOR;
			shapeDef.enableSensorEvents = true;
		}
		else {
			shapeDef.enableContactEvents = true;
		}
		loadPolygon(object, levelPolygon, bodyId, shapeDef);
		b2Body_SetUserData(bodyId, reinterpret_cast<void*>(objectList.size() - 1));
		if (object.getProperties().hasProperty("nextLevel")) {
			levelPolygon->nextLevel = object.get<int>("nextLevel");
			if (levelPolygon->nextLevel > 0) {
				if (levelPolygon->nextLevel == currentLevelNumber) {
					//spawnLocation = Casts::get().sfVector2f_to_b2Vec2((levelPolygon->getTransformable()->getPosition() / Casts::get().scaleFactor) + sf::Vector2f(96, 192));
				}
				TextObject* text = new TextObject(font);
				text->text.setString(" " + std::to_string(levelPolygon->nextLevel));
				text->text.setCharacterSize(60);
				text->text.setScale(sf::Vector2f(Casts::get().scaleFactor, Casts::get().scaleFactor));
				//text->text.setOrigin(text->text.getLocalBounds().size * 0.5f + sf::Vector2f(0, 30));
				text->text.setPosition(levelPolygon->getTransformable()->getPosition());
				text->text.setRotation(levelPolygon->getTransformable()->getRotation());
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
		levelPolygon->gravityStrength = object.getProperties().hasProperty("gravityStrength") ? object.get<float>("gravityStrength") : -1;

		levelPolygon->bodyId = bodyId;
		if (object.get<bool>("dynamic")) {
			b2Body_SetType(bodyId, b2_dynamicBody);
			b2Body_EnableSleep(bodyId, false);
		}
		if (MovingPlatform* platform = dynamic_cast<MovingPlatform*>(levelPolygon)) {
			platform->parse(object);
		}
		return levelPolygon;
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
		textObject->text.setPosition(Casts::get().pixelsToMeters(position));
		sf::Color polygonColor = sf::Color(text.color.r, text.color.g, text.color.b, text.color.a);
		textObject->text.setScale(sf::Vector2f(Casts::get().scaleFactor, Casts::get().scaleFactor));
		textObject->text.setFillColor(polygonColor);
		objectList.push_back(textObject);
		return textObject;
	}
	}
	return nullptr;
}
void Level::loadLevel(int levelNumber) {
	tson::Tileson t;
	std::unique_ptr<tson::Map> map = t.parse("resources\\levels\\" + levelList[levelNumber]);
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
	topLeft = sf::Vector2f(FLT_MAX, FLT_MAX);
	bottomRight = sf::Vector2f(FLT_MIN, FLT_MIN);

	std::vector<tson::Layer>& layers = map->getLayers();
	tson::Layer* collisionLayer = nullptr;
	for (tson::Layer& layer : layers) {
		float parallaxFactor = layer.getParallax().x;
		if (parallaxFactor == 1.0f) {
			collisionLayer = &layer;
			break;
		}
	}
	std::vector<tson::Object> collisionObjects = collisionLayer->getObjects();
	for (int i = 0; i < collisionObjects.size(); i++) {
		if (collisionObjects[i].get<bool>("dynamic")) {
			collisionObjects[i] = collisionObjects[collisionObjects.size() - 1];
			collisionObjects.resize(collisionObjects.size() - 1);
			i--;
		}
	}
	uint64_t mask = 0x80'00'00'00'00'00'00'00;
	for (tson::Layer& layer : layers) {
		float parallaxFactor = layer.getParallax().x;
		uint64_t layerMask = mask;
		uint64_t hitsPlayer = 0;
		std::vector<tson::Object>& objects = layer.getObjects();
		if (parallaxFactor == 1.0f) {
			layerMask = LEVEL;
			hitsPlayer = PLAYER;
		}
		else if (parallaxFactor < 1.0f) {
			objects.insert(objects.end(), collisionObjects.begin(), collisionObjects.end());
		}
		for (int i = 0; i < objects.size(); i++) {
			tson::Object& object = objects[i];
			if (object.isPoint()) {
				Player* player = new Player(object);
				objectList.push_back(player);
				b2Body_SetUserData(player->bodyId, reinterpret_cast<void*>(objectList.size() - 1));
				continue;
			}
			b2BodyId bodyId;
			Object* loadedObject = loadObject(object, bodyId, layerMask, hitsPlayer);
			DrawableObject* levelPolygon = dynamic_cast<DrawableObject*>(loadedObject);
			if (levelPolygon == nullptr) { continue; }
			levelPolygon->parallaxFactor = parallaxFactor;
			if (levelPolygon->getConvexShape() == nullptr) { continue; }
			if (parallaxFactor < 1.0f) {
				uint8_t shade = abs(1.0f - parallaxFactor) * 1000 + 50;
				levelPolygon->getConvexShape()->setFillColor(levelPolygon->getConvexShape()->getFillColor() + sf::Color(shade, shade, shade, 0));
			}
			
			for (int i = 0; i < levelPolygon->getConvexShape()->getPointCount(); i++) {
				sf::FloatRect rect = levelPolygon->getConvexShape()->getGlobalBounds();
				updateBounds(sf::Vector2f(rect.position.x, rect.position.y));
				updateBounds(sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y));
			}
		}
		mask >>= 1;
	}
	for (int i = 0; i < objectList.size(); i++) {
		objectList[i]->start();
	}
	currentLevelNumber = levelNumber;
}