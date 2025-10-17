#include "Player.h"
#include "Level.h"
#include "Casts.h"
extern b2Vec2 spawnLocation;

Player::Player() {
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.fixedRotation = true;

	b2ShapeDef playerShapeDef = b2DefaultShapeDef();
	b2SurfaceMaterial bodyIdMaterial = b2DefaultSurfaceMaterial();
	playerShapeDef.density /= 4;
	bodyIdMaterial.friction = 0;
	playerShapeDef.material = bodyIdMaterial;
	Casts::makeBoxWithBodyDef(rectangle, bodyId, playerShapeDef, sf::Vector2f(50, 100), sf::Vector2f(64, 64), 0, bodyDef);

	textureNotMoving.loadFromFile("resources/eyestatic.png");
	textureRight.loadFromFile("resources/eyeright.png");
	textureLeft.loadFromFile("resources/eyeleft.png");

	die();
}
void Player::die() {
	b2Body_SetTransform(bodyId, spawnLocation, b2Body_GetRotation(bodyId));
}

void Player::update(sf::RenderWindow& window, float deltaTime) {
	float xForce = 0;
	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
	b2Body_SetLinearVelocity(bodyId, { std::clamp(linearVelocity.x,-maxWalkingSpeed,maxWalkingSpeed), linearVelocity.y });
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		xForce -= walkForce;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		xForce += walkForce;
	}
	if (xForce == 0 && linearVelocity.x != 0.0f) {
		float friction = canJump ? groundFriction : airFriction;
		friction *= maxWalkingSpeed;
		if (std::signbit(linearVelocity.x)) {
			xForce = friction;
		}
		else {
			xForce = -friction;
		}
	}

	b2Body_ApplyForceToCenter(bodyId, { xForce,0 }, true);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && canJump) {
		//b2Body_SetLinearVelocity(bodyId, { linearVelocity.x - touchingWall*jumpSpeed, -jumpSpeed});
		if (touchingWall == 0) {
			b2Body_SetLinearVelocity(bodyId, b2Vec2{ linearVelocity.x, -jumpSpeed });
		}
		else {
			if (std::signbit(wallJumps) == std::signbit(touchingWall)) {
				wallJumps += touchingWall;
			}
			else {
				wallJumps = touchingWall;
			}
			b2Body_SetLinearVelocity(bodyId, b2Normalize({ touchingWall * -1.0f, -2.0f + abs(wallJumps / 4.0f) }) * jumpSpeed);
		}


		//b2Body_ApplyLinearImpulseToCenter(bodyId, { 0,-10 }, true);
		canJump = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::End)) {
		b2Body_ApplyForceToCenter(bodyId, { 0,-100 }, true);
	}
	canJump = false;
	touchingWall = 0;
	//onGround(window);
}
void Player::collide(b2BodyId otherObject, b2Vec2 normal) {
	BaseCollider* base = static_cast<BaseCollider*>(b2Body_GetUserData(otherObject));
	LevelRectangle* rectangle = dynamic_cast<LevelRectangle*>(base);
	if (rectangle) {
		// might not be a levelrectangle, but it does exist
		if (rectangle->type == DOOR) {
			Level::loadLevel(Level::getCurrentLevel() + 1);
		}
	}

	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
	if (normal.y < -0.5f) {
		canJump = true;
		wallJumps = 0;
	}
	if (linearVelocity.y + abs(linearVelocity.x) > 0) {
		if (normal.x > 0.9 && touchingWall >= 0) {
			canJump = true;
			touchingWall -= 1;
		}
		else if (normal.x < -0.9 && touchingWall <= 0) {
			canJump = true;
			touchingWall += 1;
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
void Player::movePlayerEvents(sf::Event event) {
	if (event.key.code == sf::Keyboard::Key::Delete) {
		die();
	}
}

void Player::draw(sf::RenderWindow& window) {
	rectangle.setTexture(&textureNotMoving);
	Casts::move(rectangle, bodyId);
	window.draw(rectangle);
	path.push_back(sf::Vertex(rectangle.getPosition(), sf::Color::Green));
	if (path.size() > 14400) {
		path.erase(path.begin());
	}

	window.draw(&path[0], path.size(), sf::LinesStrip);
}