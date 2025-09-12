#pragma once
#include "BasicIncludes.h"

class Player
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
	bool canJump;
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

	//shapes//
	sf::RectangleShape playerRectangle;
	b2BodyId player;

	//debug//
	std::vector<sf::Vertex> path;
public:
	Player(b2BodyId playerId, sf::RectangleShape& rectangle);
	void die();

	void update(sf::RenderWindow& window, float deltaTime);

	void onGround(sf::RenderWindow& window);
	void movePlayerEvents(sf::Event event);

	void draw(sf::RenderWindow& window);
};