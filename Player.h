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
	bool bottomLine = false;
	bool topLine = false;
	bool leftLine = false;
	bool rightLine = false;

	//state//
	//Game::PlayerState state;
	bool canJump = false;
	bool touchingLeft = false;
	bool touchingRight = false;
	bool touchingFloor = false;

	//movement//
	int wallJumps = 0;
	float walkForce = 40;
	float maxWalkingSpeed = 15;
	float airFriction = 2;
	float groundFriction = 4;
	float jumpSpeed = 25;

	//body//
	sf::RectangleShape eye;
	sf::Vector2f eyeOffset;
	sf::Vector2f playerSize;

	//debug//
	std::vector<sf::Vertex> path;
	bool finishedLevel = false;
	bool dying = false;
public:

	Player();
	void die();
	void respawn();

	void update(sf::RenderWindow& window, float deltaTime) override;

	void collide(b2BodyId otherObject, b2Vec2 normal) override;
	void inputCallback(sf::Event event) override;

	void draw(sf::RenderWindow& window) override;
};