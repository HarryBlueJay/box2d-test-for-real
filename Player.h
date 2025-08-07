#pragma once
#include "Level.h"

class Player
{
public:
	//hitboxes//
	// left hitbox
	// right hitbox
	// up hitbox
	// down hitbox
	bool bottomLine = false;
	bool topLine = false;
	bool leftLine = false;
	bool rightLine = false;

	//state//
	//Game::PlayerState state;
	bool canJump;
	float jumpCooldown;

	//movement//
	int leftWallJumps = 0;
	int rightWallJumps = 0;

	//textures//
	sf::Texture textureNotMoving;
	sf::Texture textureRight;
	sf::Texture textureLeft;

	//shapes//
	sf::RectangleShape playerRectangle;
	b2BodyId player;

	Player(b2BodyId playerId, sf::RectangleShape& rectangle) {
		player = playerId;
		playerRectangle = rectangle;
		textureNotMoving.loadFromFile("resources/eyestatic.png");
		textureRight.loadFromFile("resources/eyeright.png");
		textureLeft.loadFromFile("resources/eyeleft.png");
	}
	void die() {
		b2Body_SetTransform(player,Level::spawnLocation,b2Body_GetRotation(player));
	}

	void update(RenderWindow& window, float deltaTime) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			b2Body_ApplyForceToCenter(player, { -20,0 }, true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			b2Body_ApplyForceToCenter(player, { 20,0 }, true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && canJump && jumpCooldown <= 0) {
			b2Body_ApplyLinearImpulseToCenter(player, { 0,-10 }, true);
			canJump = false;
			jumpCooldown = 0.1;
		}
		onGround(window);
		jumpCooldown -= deltaTime;
	}

	void onGround(RenderWindow& window) {
		b2Vec2 topLeft = b2Body_GetWorldPoint(player, { -0.5 * 2,-0.5 * 2 });
		b2Vec2 topRight = b2Body_GetWorldPoint(player, { 0.5 * 2,-0.5 * 2 });
		b2Vec2 bottomLeft = b2Body_GetWorldPoint(player, { -0.5 * 2,0.5 * 2 });
		b2Vec2 bottomRight = b2Body_GetWorldPoint(player, { 0.5 * 2,0.5 * 2 });

		OverlapResult bottomResult = lineOverlap(bottomLeft + b2Vec2{ 0.1,0.1 }, bottomRight + b2Vec2{-0.1,0.1 }, b2DefaultQueryFilter(), window);
		OverlapResult topResult = lineOverlap(topLeft + b2Vec2{ 0.1,-0.1 }, topRight + b2Vec2{ -0.1,-0.1 }, b2DefaultQueryFilter(), window);
		OverlapResult leftResult = lineOverlap(topLeft+b2Vec2{-0.1,0.1 }, bottomLeft+b2Vec2{ -0.1,-0.1 }, b2DefaultQueryFilter(), window);
		OverlapResult rightResult = lineOverlap(topRight + b2Vec2{ 0.1,0.1 }, bottomRight + b2Vec2{ 0.1,-0.1 }, b2DefaultQueryFilter(), window);
		bottomLine = bottomResult.hit;
		topLine = topResult.hit;
		leftLine = leftResult.hit;
		rightLine = rightResult.hit;

		if (bottomLine && !canJump) {
			canJump = true;
		}
		else if (!bottomLine && canJump) {
			canJump = false;
		}
	}
	void movePlayerEvents(sf::Event event) {
		if (event.key.code == sf::Keyboard::Key::Delete) {
			die();
		}
	}

	void draw(sf::RenderWindow& window) {
		playerRectangle.setTexture(&textureNotMoving);
		move(playerRectangle, player);
		window.draw(playerRectangle);
	}
};