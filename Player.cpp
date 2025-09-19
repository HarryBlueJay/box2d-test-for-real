#include "Player.h"
#include "Level.h"
#include "Casts.h"
extern b2Vec2 spawnLocation;
extern std::vector<LevelRectangle> currentLevel;

Player::Player(b2BodyId playerId, sf::RectangleShape& rectangle) {
	player = playerId;
	playerRectangle = rectangle;
	textureNotMoving.loadFromFile("resources/eyestatic.png");
	textureRight.loadFromFile("resources/eyeright.png");
	textureLeft.loadFromFile("resources/eyeleft.png");
}
void Player::die() {
	b2Body_SetTransform(player, spawnLocation, b2Body_GetRotation(player));
}

void Player::update(sf::RenderWindow& window, float deltaTime) {
	float xForce = 0;
	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(player);
	b2Body_SetLinearVelocity(player, { std::clamp(linearVelocity.x,-maxWalkingSpeed,maxWalkingSpeed), linearVelocity.y });
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

	b2Body_ApplyForceToCenter(player, { xForce,0 }, true);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && canJump) {
		//b2Body_SetLinearVelocity(player, { linearVelocity.x - touchingWall*jumpSpeed, -jumpSpeed});
		if (touchingWall == 0) {
			b2Body_SetLinearVelocity(player, b2Vec2{ linearVelocity.x, -jumpSpeed });
		}
		else {
			if (std::signbit(wallJumps) == std::signbit(touchingWall)) {
				wallJumps += touchingWall;
			}
			else {
				wallJumps = touchingWall;
			}
			b2Body_SetLinearVelocity(player, b2Normalize({ touchingWall * -1.0f, -2.0f + abs(wallJumps / 4.0f) }) * jumpSpeed);
		}


		//b2Body_ApplyLinearImpulseToCenter(player, { 0,-10 }, true);
		canJump = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::End)) {
		b2Body_ApplyForceToCenter(player, { 0,-100 }, true);
	}
	onGround(window);
}

void Player::onGround(sf::RenderWindow& window) {
	b2Vec2 topLeft = b2Body_GetWorldPoint(player, { -0.5 * 2,-0.5 * 2 });
	b2Vec2 topRight = b2Body_GetWorldPoint(player, { 0.5 * 2,-0.5 * 2 });
	b2Vec2 bottomLeft = b2Body_GetWorldPoint(player, { -0.5 * 2,0.5 * 2 });
	b2Vec2 bottomRight = b2Body_GetWorldPoint(player, { 0.5 * 2,0.5 * 2 });

	Casts::OverlapResult bottomResult = Casts::lineOverlap(bottomLeft + b2Vec2{ 0.1,0.1 }, bottomRight + b2Vec2{ -0.1,0.1 }, b2DefaultQueryFilter(), window);
	Casts::OverlapResult topResult = Casts::lineOverlap(topLeft + b2Vec2{ 0.1,-0.1 }, topRight + b2Vec2{ -0.1,-0.1 }, b2DefaultQueryFilter(), window);
	Casts::OverlapResult leftResult = Casts::lineOverlap(topLeft + b2Vec2{ -0.1,0.1 }, bottomLeft + b2Vec2{ -0.1,-0.1 }, b2DefaultQueryFilter(), window);
	Casts::OverlapResult rightResult = Casts::lineOverlap(topRight + b2Vec2{ 0.1,0.1 }, bottomRight + b2Vec2{ 0.1,-0.1 }, b2DefaultQueryFilter(), window);
	bottomLine = bottomResult.hit;
	topLine = topResult.hit;
	leftLine = leftResult.hit;
	rightLine = rightResult.hit;

	b2Vec2 linearVelocity = b2Body_GetLinearVelocity(player);
	canJump = bottomLine;// || leftLine || rightLine;
	touchingWall = 0;
	if (linearVelocity.y + abs(linearVelocity.x) > 0) {
		canJump = bottomLine || leftLine || rightLine;
		if (leftLine) {
			touchingWall -= 1;
		}
		if (rightLine) {
			touchingWall += 1;
		}
	}
	if (bottomLine) {
		wallJumps = 0;
	}
}
void Player::movePlayerEvents(sf::Event event) {
	if (event.key.code == sf::Keyboard::Key::Delete) {
		die();
	}
}

void Player::draw(sf::RenderWindow& window) {
	playerRectangle.setTexture(&textureNotMoving);
	Casts::move(playerRectangle, player);
	window.draw(playerRectangle);
	path.push_back(sf::Vertex(playerRectangle.getPosition(), sf::Color::Green));
	if (path.size() > 14400) {
		path.erase(path.begin());
	}

	window.draw(&path[0], path.size(), sf::LinesStrip);
}