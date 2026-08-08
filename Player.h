#pragma once
#include "BaseCollider.h"

class Player : public BaseCollider
{
private:
	//hitboxes//
	// left hitbox
	// right hitbox
	// up hitbox
	// down hitbox
	//bool bottomLine = false;
	//bool topLine = false;
	//bool leftLine = false;
	//bool rightLine = false;

	//state//
	//Game::PlayerState state;
	//bool canJump = false;
	//bool touchingLeft = false;
	//bool touchingRight = false;
	//bool touchingFloor = false;

	//movement//
	int wallJumps = 0;
	const float walkForce = 40;
	const float maxWalkingSpeed = 15;
	const float jumpSpeed = 25;
	const float dashSpeed = 30;
	const int maxDashes = 1;
	int dashes = maxDashes;
	const float coyoteTime = 0.1f;
	float coyoteCounter = 0.0f;
	sf::Vector2f lastGroundedPosition;

	//body//
	const b2Vec2 size = b2Vec2{ 2, 2 };
	sf::RectangleShape eye;
	sf::Vector2f eyeOffset;

	//camera//
	bool snapCameraToPlayer = false;
	

	//debug//
	std::vector<sf::Vertex> path;
	sf::Vertex gravityPath[2];
	bool finishedLevel = false;
	bool dying = false;
public:

	Player(b2Vec2 spawnLocation);
	void die();
	sf::Vector2f getCameraPosition(sf::View& view);

	void update(float deltaTime) override;

	void collide(Object* otherObject, b2Vec2 normal) override;
	void touch(Object* otherObject) override;
	void inputCallback(std::optional<sf::Event> event) override;

	void draw(sf::RenderWindow& window) override;
};