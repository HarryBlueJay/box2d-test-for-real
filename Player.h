#pragma once
#include "Object.h"

class Player : public Object
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
	int touchingWall = 0;

	//movement//
	int wallJumps = 0;
	float walkForce = 40;
	float maxWalkingSpeed = 15;
	float airFriction = 2;
	float groundFriction = 4;
	float jumpSpeed = 25;


	//textures//
	sf::Texture textureNotMoving;
	sf::Texture textureRight;
	sf::Texture textureLeft;

	//debug//
	std::vector<sf::Vertex> path;
public:

	Player();
	void die();

	void update(sf::RenderWindow& window, float deltaTime) override;

	void collide(b2BodyId otherObject, b2Vec2 normal) override;
	void movePlayerEvents(sf::Event event);

	void draw(sf::RenderWindow& window) override;
};