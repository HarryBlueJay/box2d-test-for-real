#include "Player.h"
#include "Level.h"
#include "Casts.h"
#include "TransitionManager.h"
sf::Texture bodyTexture;
sf::Texture eyeTexture;


Player::Player(b2Vec2 spawnLocation) {
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;

	b2ShapeDef playerShapeDef = b2DefaultShapeDef();
	b2SurfaceMaterial bodyIdMaterial = b2DefaultSurfaceMaterial();
	playerShapeDef.density /= 4;
	bodyIdMaterial.friction = 0;
	playerShapeDef.material = bodyIdMaterial;
	Casts::makeBoxWithBodyDef(rectangle, bodyId, playerShapeDef, sf::Vector2f(50, 100), sf::Vector2f(64, 64), 0, bodyDef);

	bodyTexture.loadFromFile("resources/body.png");
	rectangle.setTexture(&bodyTexture);
	eyeTexture.loadFromFile("resources/eye.png");
	eye.setTexture(&eyeTexture);
	eye.setSize(rectangle.getSize());
	eye.setOrigin(rectangle.getOrigin());
	playerSize = rectangle.getSize();
	b2Body_SetLinearVelocity(bodyId, { 0,0 });
	b2Body_SetTransform(bodyId, spawnLocation, b2Body_GetRotation(bodyId));
	Casts::move(rectangle, bodyId);
	dying = false;
}
void Player::die() {
	b2Body_SetLinearVelocity(bodyId, { 0,0 });
	dying = true;
}

sf::Vector2f Player::getCameraPosition(sf::View& view) {
	sf::Vector2f position = rectangle.getPosition();
	position.y -= view.getSize().y / 8;
	return position;
}

void Player::update(float deltaTime) {
	const float delta = 0.05f;
	const float deathRotationSpeed = 360;
	const float deathAnimationTime = 0.75f;
	if (dying) {
		deltaTime /= deathAnimationTime;
		rectangle.setScale(rectangle.getScale() - sf::Vector2f(deltaTime, deltaTime));
		rectangle.rotate(deltaTime * deathRotationSpeed);
		eyeOffset = Casts::rotate(eyeOffset, -deathRotationSpeed * deltaTime * B2_PI/ 180.0f);
		if (rectangle.getScale().x < delta) {
			TransitionManager::get().restartLevel();
		}
		eye.setScale(rectangle.getScale());
		return;
	}
	float xForce = 0;
	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
	
	sf::Vector2f end = Casts::b2Vec2_to_sfVector2f(b2Normalize(linearVelocity)) / 2.5f;
	eyeOffset = end + (eyeOffset - end) * std::exp(-deltaTime * 10);
	
	b2Body_SetLinearVelocity(bodyId, { std::clamp(linearVelocity.x,-maxWalkingSpeed,maxWalkingSpeed), linearVelocity.y });
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		xForce -= walkForce;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		xForce += walkForce;
	}
	if (xForce == 0) {
		if (abs(linearVelocity.x) >= delta) {
			float friction = canJump ? groundFriction : airFriction;
			friction *= maxWalkingSpeed;
			if (std::signbit(linearVelocity.x)) {
				xForce = friction;
			}
			else {
				xForce = -friction;
			}
		}
		else {
			b2Body_SetLinearVelocity(bodyId, { 0, linearVelocity.y });
		}
	}

	b2Body_ApplyForceToCenter(bodyId, { xForce,0 }, true);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && canJump) {
		if (!touchingLeft && !touchingRight) {
			b2Body_SetLinearVelocity(bodyId, b2Vec2{ linearVelocity.x, -jumpSpeed });
		}
		else {
			float touchingWall = 0;
			if (touchingLeft) {
				touchingWall += 1;
			}
			if (touchingRight) {
				touchingWall -= 1;
			}
			if (touchingFloor) {
				touchingWall /= 10;
			}
			std::cout << touchingWall << std::endl;
			if (std::signbit(wallJumps) == std::signbit(touchingWall)) {
				wallJumps += touchingWall;
			}
			else {
				wallJumps = touchingWall;
			}
			b2Body_SetLinearVelocity(bodyId, b2Normalize({ touchingWall * -1.0f, -2.0f + abs(wallJumps / 4.0f) }) * jumpSpeed);
		}
		canJump = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::End)) {
		b2Body_ApplyForceToCenter(bodyId, { 0,-100 }, true);
	}
	canJump = false;
	touchingLeft = false;
	touchingRight = false;
	touchingFloor = false;
	Casts::move(rectangle, bodyId);
}
void Player::collide(Object* otherObject, b2Vec2 normal) {
	LevelRectangle* levelRectangle = dynamic_cast<LevelRectangle*>(otherObject);
	if (levelRectangle) {
		// might not be a levelrectangle, but it does exist
		if (levelRectangle->isDoor) {
			TransitionManager::get().finishLevel();
		}
		if (levelRectangle->isKillbrick) {
			die();
		}
	}

	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
	if (normal.y < -0.5f) {
		canJump = true;
		touchingFloor = true;
		wallJumps = 0;
		lastGroundedPosition = rectangle.getPosition();
	}
	if (linearVelocity.y >= -20.0f) {
		if (normal.x > 0.9) {
			canJump = true;
			touchingRight = true;
		}
		else if (normal.x < -0.9) {
			canJump = true;
			touchingLeft = true;
		}
	}
}

//void Player::onGround(sf::RenderWindow& window) {
//	b2Vec2 topLeft = b2Body_GetWorldPoint(bodyId, { -0.5 * 2,-0.5 * 2 });
//	b2Vec2 topRight = b2Body_GetWorldPoint(bodyId, { 0.5 * 2,-0.5 * 2 });
//	b2Vec2 bottomLeft = b2Body_GetWorldPoint(bodyId, { -0.5 * 2,0.5 * 2 });
//	b2Vec2 bottomRight = b2Body_GetWorldPoint(bodyId, { 0.5 * 2,0.5 * 2 });
//
//	Casts::OverlapResult bottomResult = Casts::lineOverlap(bottomLeft + b2Vec2{ 0.1,0.1 }, bottomRight + b2Vec2{ -0.1,0.1 }, b2DefaultQueryFilter(), window);
//	Casts::OverlapResult topResult = Casts::lineOverlap(topLeft + b2Vec2{ 0.1,-0.1 }, topRight + b2Vec2{ -0.1,-0.1 }, b2DefaultQueryFilter(), window);
//	Casts::OverlapResult leftResult = Casts::lineOverlap(topLeft + b2Vec2{ -0.1,0.1 }, bottomLeft + b2Vec2{ -0.1,-0.1 }, b2DefaultQueryFilter(), window);
//	Casts::OverlapResult rightResult = Casts::lineOverlap(topRight + b2Vec2{ 0.1,0.1 }, bottomRight + b2Vec2{ 0.1,-0.1 }, b2DefaultQueryFilter(), window);
//	bottomLine = bottomResult.hit;
//	topLine = topResult.hit;
//	leftLine = leftResult.hit;
//	rightLine = rightResult.hit;
//
//	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
//	canJump = bottomLine;// || leftLine || rightLine;
//	touchingWall = 0;
//	if (linearVelocity.y + abs(linearVelocity.x) > 0) {
//		canJump = bottomLine || leftLine || rightLine;
//		if (leftLine) {
//			touchingWall -= 1;
//		}
//		if (rightLine) {
//			touchingWall += 1;
//		}
//	}
//	if (bottomLine) {
//		wallJumps = 0;
//	}
//}
void Player::inputCallback(sf::Event event) {
	if (event.key.code == sf::Keyboard::Key::Delete && event.type == sf::Event::EventType::KeyPressed) {
		die();
	}
}

void Player::draw(sf::RenderWindow& window) {
	eye.setPosition(rectangle.getPosition() + (eyeOffset * eye.getScale().x));
	window.draw(rectangle);
	window.draw(eye);
	path.push_back(sf::Vertex(rectangle.getPosition(), sf::Color::Green));
	if (path.size() > 14400) {
		path.erase(path.begin());
	}

	window.draw(&path[0], path.size(), sf::LinesStrip);
}